#include "standard_midi_format_reader.h"

#include "standard_midi_format_event.h"

namespace keysequencer {

StandardMidiFormatReader::~StandardMidiFormatReader() {
  if (track_readers_ != nullptr) {
    delete[] track_readers_;
  }
}

StandardMidiFormatReader::Result StandardMidiFormatReader::Initialize(const std::uint8_t* const start, std::size_t const length) {
  const std::uint8_t* cursor = start;
  const std::uint8_t* const end = start + length;

  // �Œ�14Byte������SMF�Ƃ��Đ������Ȃ�
  if (length < 14) {
    return Result::kFailureIllegalSmf;
  }

  // MThd�̕�������m�F
  if (ReadUint32(cursor) != 0x4d546864) {
    return Result::kFailureIllegalSmf;
  }

  // �w�b�_�`�����N�̑傫��(6�Œ�)
  if (ReadUint32(cursor) != 6) {
    return Result::kFailureIllegalSmf;
  }

  // �t�H�[�}�b�g���擾(Format1�܂���2�̂ݑΉ�)
  std::uint16_t format = ReadUint16(cursor);
  if (format != static_cast<std::uint16_t>(Format::kType0) && format != static_cast<std::uint16_t>(Format::kType1)) {
    return Result::kFailureNotSupportSmf;
  }

  // �g���b�N�����擾(�Œ�1�͕K�v)
  std::uint16_t track = ReadUint16(cursor);
  if (track == 0) {
    return Result::kFailureNotSupportSmf;
  }

  std::uint16_t quarter_note_tick = ReadUint16(cursor);
  if ((quarter_note_tick & 0x8000) != 0) {
    return Result::kFailureNotSupportSmf;
  } else if (quarter_note_tick == 0) {
    return Result::kFailureIllegalSmf;
  }

  TrackReader* track_readers = new TrackReader[track];
  for (std::size_t i = 0; i < static_cast<std::size_t>(track); ++i) {
    // �g���b�N�w�b�_8Byte���K�{
    if (cursor + 7 >= end) {
      delete[] track_readers;
      return Result::kFailureIllegalSmf;
    }

    // MTrk�̕�����
    if (ReadUint32(cursor) != 0x4d54726b) {
      delete[] track_readers;
      return Result::kFailureIllegalSmf;
    }

    // �g���b�N�f�[�^���̓ǂݎ��(�ꉞ0���Ή����Ă��邪0�͂��蓾��̂��H)
    std::uint32_t track_length = ReadUint32(cursor);

    // �g���b�N�f�[�^��������
    track_readers[i].start = cursor;
    track_readers[i].end = cursor + track_length;
    track_readers[i].cursor = cursor;
    track_readers[i].last_midi_event_status = 0;
    track_readers[i].last_event_microsecond = 0;

    if (track_readers[i].start < track_readers[i].end) {
      // �ŏ��̃C�x���g�̃f���^�^�C����ǂݎ��
      if (!ReadVariableUint(track_readers[i].cursor, track_readers[i].end, track_readers[i].delta_time)) {
        delete[] track_readers;
        return Result::kFailureIllegalSmf;
      }

      // �g���b�N�f�[�^���X�L�b�v
      cursor += track_length;
      if (cursor > end) {
        delete[] track_readers;
        return Result::kFailureIllegalSmf;
      }
    }
  }

  // �t�H�[�}�b�g���m�F
  if (format == static_cast<std::uint16_t>(Format::kType0)) {
    format_ = Format::kType0;
  } else if (format == static_cast<std::uint16_t>(Format::kType1)) {
    format_ = Format::kType1;
  }

  // �^�C���x�[�X��ǂݎ��
  quarter_note_tick_ = quarter_note_tick;

  // �g���b�N����ǂݎ��
  track_ = track;
  if (track_readers_ != nullptr) {
    delete[] track_readers_;
  }
  track_readers_ = track_readers;

  // �ǂݎ��f�[�^�̏�����
  omni_ = false;
  mono_ = false;
  tempo_ = 60 * 1000000 / 120;  // BPM120

  return Result::kSuccess;
}

StandardMidiFormatReader::Result StandardMidiFormatReader::Rewind() {
  if (track_readers_ == nullptr) {
    return Result::kFailureUninitialized;
  }

  for (std::size_t i = 0; i < track_; ++i) {
    // �ǂݎ��J�[�\����擪��
    track_readers_[i].cursor = track_readers_[i].start;

    // �X�e�[�^�X��������
    track_readers_[i].last_midi_event_status = 0;
    track_readers_[i].last_event_microsecond = 0;

    // �ŏ��̃C�x���g�̃f���^�^�C����ǂݎ��
    // Open�œǂݎ��邱�Ƃ��m�F���Ă���̂ł����ł͍ă`�F�b�N���Ȃ�
    ReadVariableUint(track_readers_[i].cursor, track_readers_[i].end, track_readers_[i].delta_time);
  }

  omni_ = false;
  mono_ = false;
  tempo_ = 60 * 1000000 / 120;  // BPM120

  return Result::kSuccess;
}

StandardMidiFormatReader::Result StandardMidiFormatReader::Next(StandardMidiFormatEvent* event) {
  if (track_readers_ == nullptr) {
    return Result::kFailureUninitialized;
  }

  // �S�g���b�N���玟�ɏ�������C�x���g������
  // �Z�b�g�e���|�̑��݂�����̂Ńf���^�^�C���ł͂Ȃ������Ԃ̗݌v�Ŕ��f����
  // �f���^�^�C�����ɃZ�b�g�e���|�������ꍇ�A
  // �Z�b�g�e���|�O��̃f���^�^�C�������̗������Z�b�g�e���|��̒l�Ōv�Z���Ă��邪
  // �����̐��m�Ȏd�l��������Ȃ����߂�����������Ԉ���Ă��邩������Ȃ�
  // TODO: ���ۂɂ��������f�[�^������đ��̃\�t�g�̓���𒲍�����
  std::size_t track = 0;
  std::size_t target_track = 0;
  std::uint64_t minimum_microsecond = 0;
  for (; track < track_; ++track) {
    if (track_readers_[track].cursor < track_readers_[track].end) {
      target_track = track;

      // ���t�J�n����̌o�ߎ���(�}�C�N���b)���v�Z����
      // �f���^�^�C��(�O�̃C�x���g����̑��Ε���\) �� ���ԒP��(����\/�l������) �~ �e���|(�}�C�N���b/�l������)
      // �������������_�ȉ����o��悤�Ȓl�ɐݒ�͂���Ȃ���
      // ���ꂽ�Ƃ��Ă���ʓI�ɐ؂�̂Ă�z��炵���̂ł��̂܂܏�������
      minimum_microsecond = track_readers_[track].last_event_microsecond + (static_cast<std::uint64_t>(track_readers_[track].delta_time) * static_cast<std::uint64_t>(tempo_)) / quarter_note_tick_;
      break;
    }
  }

  // �C�x���g���c���Ă��Ȃ���ΏI��
  if (track >= track_) {
    return Result::kFailureEndOfFile;
  }

  // ���ɃC�x���g�̎c���Ă���g���b�N������΂��̒������ԏ������o�ߎ��Ԃ�I��
  // (�f���^�^�C��������Ȃ�g���b�N�ԍ����Ⴂ����I��)
  for (++track; track < track_; ++track) {
    if (track_readers_[track].cursor < track_readers_[track].end) {
      std::uint64_t microsecond = track_readers_[track].last_event_microsecond + (static_cast<std::uint64_t>(track_readers_[track].delta_time) * static_cast<std::uint64_t>(tempo_)) / quarter_note_tick_;
      if (microsecond < minimum_microsecond) {
        target_track = track;
        minimum_microsecond = microsecond;
      }
    }
  }

  // �I�΂ꂽ�C�x���g�̎��Ԃ��Z�b�g
  event->microsecond = minimum_microsecond;

  // �L�q�̊ȗ����̂��߂Ɉꎞ�ϐ��ɃR�s�[
  const std::uint8_t* cursor = track_readers_[target_track].cursor;
  const std::uint8_t* const end = track_readers_[target_track].end;

  // �����j���O�X�e�[�^�X���l�����ăX�e�[�^�X�o�C�g��ǂݎ��
  std::uint8_t const status = (*cursor & 0x80) == 0 ? track_readers_[target_track].last_midi_event_status : *cursor;
  if ((*cursor & 0x80) == 0) {
    --cursor;  // �X�e�[�^�X�o�C�g���L��Ƃ��Ɩ������œ����������o����l�A��������-1���ĕ␳����
  }

  switch (status & 0xf0) {
    // �m�[�g OFF
    case static_cast<std::uint8_t>(StandardMidiFormatEvent::MidiType::kNoteOff):
      if (cursor + 2 >= end) {
        return Result::kFailureIllegalSmf;
      }
      event->type = StandardMidiFormatEvent::Type::kMidi;
      event->midi_type = StandardMidiFormatEvent::MidiType::kNoteOff;
      event->channel = (status & 0x0f);
      event->note_number = cursor[1];
      event->velocity = cursor[2];
      cursor += 3;
      break;

    // �m�[�g ON
    case static_cast<std::uint8_t>(StandardMidiFormatEvent::MidiType::kNoteOn):
      if (cursor + 2 >= end) {
        return Result::kFailureIllegalSmf;
      }
      event->type = StandardMidiFormatEvent::Type::kMidi;
      event->midi_type = StandardMidiFormatEvent::MidiType::kNoteOn;
      event->channel = (status & 0x0f);
      event->note_number = cursor[1];
      event->velocity = cursor[2];
      cursor += 3;
      break;

    // �|���t�H�j�b�N�L�[�v���b�V���[
    case static_cast<std::uint8_t>(StandardMidiFormatEvent::MidiType::kPolyphonicKeyPressure):
      if (cursor + 2 >= end) {
        return Result::kFailureIllegalSmf;
      }
      event->type = StandardMidiFormatEvent::Type::kMidi;
      event->midi_type = StandardMidiFormatEvent::MidiType::kPolyphonicKeyPressure;
      event->channel = (status & 0x0f);
      event->note_number = cursor[1];
      event->velocity = cursor[2];
      cursor += 3;
      break;

    // �R���g���[���`�F���W or �`�����l�����[�h���b�Z�[�W
    case static_cast<std::uint8_t>(StandardMidiFormatEvent::MidiType::kControlChange):
      if (cursor + 2 >= end) {
        return Result::kFailureIllegalSmf;
      }
      event->type = StandardMidiFormatEvent::Type::kMidi;
      event->midi_type = StandardMidiFormatEvent::MidiType::kControlChange;
      event->channel = (status & 0x0f);
      event->control_number = cursor[1];
      event->control_value = cursor[2];

      if (cursor[1] == 0x7c) {
        // OMNI OFF
        omni_ = false;
      } else if (cursor[1] == 0x7d) {
        // OMNI ON
        omni_ = true;
      } else if (cursor[1] == 0x7e) {
        // POLY���[�h
        mono_ = false;
      } else if (cursor[1] == 0x7f) {
        // MONO���[�h
        mono_ = true;
        if (!omni_) {
          // ���̃P�[�X�����C�x���g�̃T�C�Y��4byte�ɂȂ�
          if (cursor + 3 >= end) {
            return Result::kFailureIllegalSmf;
          }
          ++cursor;
        }
      }
      cursor += 3;
      break;

    // �v���O�����`�F���W
    case static_cast<std::uint8_t>(StandardMidiFormatEvent::MidiType::kProgramChange):
      if (cursor + 1 >= end) {
        return Result::kFailureIllegalSmf;
      }
      event->type = StandardMidiFormatEvent::Type::kMidi;
      event->midi_type = StandardMidiFormatEvent::MidiType::kProgramChange;
      event->channel = (status & 0x0f);
      event->program = cursor[1];
      cursor += 2;
      break;

    // �`�����l���v���b�V���[
    case static_cast<std::uint8_t>(StandardMidiFormatEvent::MidiType::kChannelPressure):
      if (cursor + 1 >= end) {
        return Result::kFailureIllegalSmf;
      }
      event->type = StandardMidiFormatEvent::Type::kMidi;
      event->midi_type = StandardMidiFormatEvent::MidiType::kChannelPressure;
      event->channel = (status & 0x0f);
      event->velocity = cursor[1];
      cursor += 2;
      break;

    // �s�b�`�x���h
    case static_cast<std::uint8_t>(StandardMidiFormatEvent::MidiType::kPitchBend):
      if (cursor + 2 >= end) {
        return Result::kFailureIllegalSmf;
      }
      event->type = StandardMidiFormatEvent::Type::kMidi;
      event->midi_type = StandardMidiFormatEvent::MidiType::kPitchBend;
      event->pitch_bend = static_cast<std::uint16_t>(cursor[1] & 0x7f) | (static_cast<std::uint16_t>(cursor[2] & 0x7f) << 7);
      cursor += 3;
      break;

    case 0xf0:
      if (status == 0xf0) {
        // SysEx�C�x���g (F0)
        event->type = StandardMidiFormatEvent::Type::kSysEx;
        event->sys_ex_type = StandardMidiFormatEvent::SysExType::kF0;
        if (!ReadVariableUint(++cursor, end, event->data_length)) {
          return Result::kFailureIllegalSmf;
        }

        // �f�[�^���ɏI�[(0xf7)���܂܂��̂�1�ȏ�ł���͂�)
        if (event->data_length == 0) {
          return Result::kFailureIllegalSmf;
        }

        // �{���Ȃ�擪��0xf0���t���̂����ASysEx�C�x���g�͎g���\�肪�Ȃ����ƂƁA�����̊ȗ����̂��߁A�擪0xf0���t���Ȃ��f�[�^��n��
        if (cursor + event->data_length > end) {
          return Result::kFailureIllegalSmf;
        }
        event->data = cursor;

        cursor += event->data_length;
      } else if (status == 0xf7) {
        // SysEx�C�x���g (F7)
        event->type = StandardMidiFormatEvent::Type::kSysEx;
        event->sys_ex_type = StandardMidiFormatEvent::SysExType::kF7;
        if (!ReadVariableUint(++cursor, end, event->data_length)) {
          return Result::kFailureIllegalSmf;
        }

        if (event->data_length == 0) {
          event->data = nullptr;
        } else {
          if (cursor + event->data_length > end) {
            return Result::kFailureIllegalSmf;
          }
          event->data = cursor;

          cursor += event->data_length;
        }
      } else if (status == 0xff) {
        // ���^�C�x���g
        event->type = StandardMidiFormatEvent::Type::kMeta;

        // ���^�C�x���g��� + �f�[�^��(�Œ�1Byte)���K�v
        if (cursor + 2 >= end) {
          return Result::kFailureIllegalSmf;
        }

        // ���^�C�x���g��ʂ�ǂݎ��
        switch (*(++cursor)) {
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kSequenceNumber):
            event->meta_type = StandardMidiFormatEvent::MetaType::kSequenceNumber;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kText):
            event->meta_type = StandardMidiFormatEvent::MetaType::kText;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kCopyright):
            event->meta_type = StandardMidiFormatEvent::MetaType::kCopyright;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kSequence):
            event->meta_type = StandardMidiFormatEvent::MetaType::kSequence;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kInstrumentName):
            event->meta_type = StandardMidiFormatEvent::MetaType::kInstrumentName;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kLyric):
            event->meta_type = StandardMidiFormatEvent::MetaType::kLyric;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kMarker):
            event->meta_type = StandardMidiFormatEvent::MetaType::kMarker;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kCuePoint):
            event->meta_type = StandardMidiFormatEvent::MetaType::kCuePoint;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kProgramName):
            event->meta_type = StandardMidiFormatEvent::MetaType::kProgramName;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kDeviceName):
            event->meta_type = StandardMidiFormatEvent::MetaType::kDeviceName;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kMidiChannelPrefix):
            event->meta_type = StandardMidiFormatEvent::MetaType::kMidiChannelPrefix;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kMidiPort):
            event->meta_type = StandardMidiFormatEvent::MetaType::kMidiPort;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kEndOfTrack):
            event->meta_type = StandardMidiFormatEvent::MetaType::kEndOfTrack;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kTempo):
            event->meta_type = StandardMidiFormatEvent::MetaType::kTempo;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kSmpteOffset):
            event->meta_type = StandardMidiFormatEvent::MetaType::kSmpteOffset;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kTimeSignature):
            event->meta_type = StandardMidiFormatEvent::MetaType::kTimeSignature;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kKeySignature):
            event->meta_type = StandardMidiFormatEvent::MetaType::kKeySignature;
            break;
          case static_cast<std::uint8_t>(StandardMidiFormatEvent::MetaType::kSequencerSpecificEvent):
            event->meta_type = StandardMidiFormatEvent::MetaType::kSequencerSpecificEvent;
            break;
          default:
            return Result::kFailureIllegalSmf;
        }

        if (!ReadVariableUint(++cursor, end, event->data_length)) {
          return Result::kFailureIllegalSmf;
        }

        if (event->data_length == 0) {
          event->data = nullptr;
        } else {
          if (cursor + event->data_length > end) {
            return Result::kFailureIllegalSmf;
          }
          event->data = cursor;

          cursor += event->data_length;
        }

        // �g���b�N�G���h�������ꍇ�ʏ�͏I�[�ɂȂ��Ă���͂������O�̂��ߖ����I�ɏI�[�ɃZ�b�g����
        if (event->meta_type == StandardMidiFormatEvent::MetaType::kEndOfTrack) {
          cursor = end;
        }
      }
      break;

    default:
      return Result::kFailureIllegalSmf;
  }

  // �e���|�ݒ�C�x���g�̏ꍇ�݂̂̓��ʏ���
  // ���������p�Ƀe���|�̒l��ێ����Ă���
  if (event->type == StandardMidiFormatEvent::Type::kMeta && event->meta_type == StandardMidiFormatEvent::MetaType::kTempo) {
    if (event->data_length != 3) {
      return Result::kFailureIllegalSmf;
    }

    tempo_ = (static_cast<std::uint32_t>(event->data[0]) << 16) | (static_cast<std::uint32_t>(event->data[1]) << 8) | static_cast<std::uint32_t>(event->data[2]);
  }

  // TrackReader���X�V

  // �I�[�łȂ���Ύ��̃��b�Z�[�W�̃f���^�^�C����ǂݎ���Ă���
  if (cursor < end) {
    if (!ReadVariableUint(cursor, end, track_readers_[target_track].delta_time)) {
      return Result::kFailureIllegalSmf;
    }
  }

  // �ǂݍ��݃J�[�\�����X�V
  track_readers_[target_track].cursor = cursor;

  // MIDI�C�x���g�̏ꍇ�Ƀ����j���O�X�e�[�^�X�̂��߂ɃX�e�[�^�X�o�C�g���L�^���Ă���
  if (status < 0xf0) {
    track_readers_[target_track].last_midi_event_status = status;
  }

  // �������X�V
  track_readers_[target_track].last_event_microsecond = event->microsecond;

  return Result::kSuccess;
}

}  // namespace keysequencer