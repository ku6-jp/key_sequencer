#include "sequencer.h"

#include "keyboard.h"
#include "setting.h"

namespace keysequencer {

Sequencer::Sequencer(const Setting* setting, Keyboard* keyboard) : setting_(setting), keyboard_(keyboard) {
  for (std::size_t i = 0; i < Setting::kMaximumNoteNumber - Setting::kMinimumNoteNumber; ++i) {
    last_note_on_milliseconds_[i] = 0;
  }
}

Sequencer::LoadFileResult Sequencer::LoadFile(const TCHAR* filepath) {
  file_loaded_ = false;

  // �t�@�C����ǂݍ���
  switch (standard_midi_format_.LoadFile(filepath)) {
    case StandardMidiFormat::Result::kSuccess:
      break;
    case StandardMidiFormat::Result::kFailureFileOpen:
      return LoadFileResult::kFailureFileOpen;
    case StandardMidiFormat::Result::kFailureFileTooLarge:
      return LoadFileResult::kFailureFileTooLarge;
    case StandardMidiFormat::Result::kFailureFileRead:
      return LoadFileResult::kFailureFileRead;
    default:
      return LoadFileResult::kFailureNotSupportSmf;
  }

  // Reader��������
  switch (note_on_reader_.Initialize(standard_midi_format_.GetData(), standard_midi_format_.GetLength())) {
    case StandardMidiFormatReader::Result::kSuccess:
      break;
    case StandardMidiFormatReader::Result::kFailureIllegalSmf:
      return LoadFileResult::kFailureIllegalSmf;
    case StandardMidiFormatReader::Result::kFailureNotSupportSmf:
      return LoadFileResult::kFailureNotSupportSmf;
    default:
      // ���̑��͕Ԃ�Ȃ��z��ł��邪�ꉞ�s����SMF�����Ƃ��Ă���
      return LoadFileResult::kFailureIllegalSmf;
  }

  // �������ʂɂȂ�̂Ń`�F�b�N�͂��Ȃ�
  note_off_reader_.Initialize(standard_midi_format_.GetData(), standard_midi_format_.GetLength());

  // ��x�S�̂��X�L�������ăG���[���������Ȃ����m�F
  StandardMidiFormatEvent event;
  for (;;) {
    switch (note_on_reader_.Next(&event)) {
      case StandardMidiFormatReader::Result::kSuccess:
        // ���̃C�x���g
        break;
      case StandardMidiFormatReader::Result::kFailureEndOfFile:
        // �I�[�ɒB�����琳��I��
        note_on_reader_.Rewind();
        file_loaded_ = true;
        return LoadFileResult::kSuccess;
      case StandardMidiFormatReader::Result::kFailureIllegalSmf:
        return LoadFileResult::kFailureIllegalSmf;
      case StandardMidiFormatReader::Result::kFailureNotSupportSmf:
        return LoadFileResult::kFailureNotSupportSmf;
      default:
        return LoadFileResult::kFailureNotSupportSmf;
    }
  }
}

bool Sequencer::Open(Output output) {
  if (!file_loaded_) {
    return false;
  }

  if (output == Output::kKeyboard) {
    // �o�͐悪�L�[�{�[�h�̏ꍇ

    // �L�[�{�[�h���͏�Ԃ���U���Z�b�g
    keyboard_->Reset();

    // Reader�̓ǂݍ��݈ʒu��擪�ɖ߂�
    note_on_reader_.Rewind();
    note_off_reader_.Rewind();
    note_on_reader_count_ = 0;
    note_off_reader_count_ = 0;

    // ���̃m�[�gON/OFF�����Z�b�g(�����ɐݒ�)
    note_on_time_.SetNull();
    note_off_time_.SetNull();
    on_note_number_ = 0;
    off_note_number_ = 0;

    // ���̏C���L�[�A�b�v�_�E�����Ԃ����Z�b�g(�����ɐݒ�)
    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      modifier_key_down_times_[i].SetNull();
      modifier_key_up_times_[i].SetNull();
    }

    // �Ō�Ƀm�[�g��ON�ɂ������Ԃ����Z�b�g
    for (std::size_t i = 0; i < Setting::kMaximumNoteNumber - Setting::kMinimumNoteNumber; ++i) {
      last_note_on_milliseconds_[i] = 0;
    }

    // ���̃m�[�gON��ǂݏo���ė\�肷��
    ScheduleKeyboardNoteOn();

    // ���̃m�[�gOFF��ǂݏo���ė\�肷��
    ScheduleKeyboardNoteOff();
  } else if (output == Output::kMidiSynthesizer) {
    // �o�͐悪MIDI�����̏ꍇ

    // MIDI�����o�͂��J��
    if (!midi_synthesizer_.Open()) {
      return false;
    }

    // Reader�̓ǂݍ��݈ʒu��擪�ɖ߂�
    // MIDI�����o�͎���Reader��1�L��΂��������Ȃ̂�OFF�p���g�p����
    note_off_reader_.Rewind();

    // ���ɏ�������C�x���g�Ƃ��čŏ���1��ǂݍ���
    if (note_off_reader_.Next(&event_) != StandardMidiFormatReader::Result::kSuccess) {
      return false;
    }
  }

  output_ = output;

  play_start_millisecond_ = timeGetTime();

  return true;
}

void Sequencer::Close() {
  if (output_ == Output::kKeyboard) {
    keyboard_->Reset();
  } else if (output_ == Output::kMidiSynthesizer) {
    midi_synthesizer_.Close();
  }
}

bool Sequencer::Run(DWORD* sleep_millisecond) {
  if (output_ == Output::kKeyboard) {
    return RunKeyboardOutput(sleep_millisecond);
  } else {
    return RunMidiSynthesizerOutput(sleep_millisecond);
  }
}

bool Sequencer::RunMidiSynthesizerOutput(DWORD* sleep_millisecond) {
  // ���t�J�n����̌o�ߎ���(�~���b)���擾
  std::uint32_t cursor = timeGetTime() - play_start_millisecond_;

  // �擾�����o�ߎ��Ԃ܂łɏ������ׂ��C�x���g�����ɏ���
  while (static_cast<std::uint32_t>(event_.microsecond / 1000) <= cursor) {
    // �擾�ς݂̃C�x���g������
    if (StandardMidiFormatEventIsValidNoteOn(event_)) {
      midi_synthesizer_.NoteOn(event_.note_number);
    } else if (StandardMidiFormatEventIsValidNoteOff(event_)) {
      midi_synthesizer_.NoteOff(event_.note_number);
    }

    // ���̃C�x���g���擾
    if (note_off_reader_.Next(&event_) != StandardMidiFormatReader::Result::kSuccess) {
      return false;
    }
  }

  // ���̃C�x���g�܂ł̑҂����Ԃ��Z�b�g
  *sleep_millisecond = static_cast<std::uint32_t>(event_.microsecond / 1000) - cursor;

  return true;
}

bool Sequencer::RunKeyboardOutput(DWORD* sleep_millisecond) {
  // ���t�J�n����̌o�ߎ���(�~���b)���擾
  std::uint32_t cursor = timeGetTime() - play_start_millisecond_;

  for (;;) {
    KeyEventTime time;

    // �ł���ɏ������ׂ��C�x���g�𒲂ׂ�

    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      if (!modifier_key_down_times_[i].IsNull() && cursor >= modifier_key_down_times_[i].millisecond && modifier_key_down_times_[i].Before(time)) {
        time = modifier_key_down_times_[i];
      }
    }

    if (!note_on_time_.IsNull() && cursor >= note_on_time_.millisecond && note_on_time_.Before(time)) {
      time = note_on_time_;
    }

    if (!note_off_time_.IsNull() && cursor >= note_off_time_.millisecond && note_off_time_.Before(time)) {
      time = note_off_time_;
    }

    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      if (!modifier_key_up_times_[i].IsNull() && cursor >= modifier_key_up_times_[i].millisecond && modifier_key_up_times_[i].Before(time)) {
        time = modifier_key_up_times_[i];
      }
    }

    // �����Ώۂ�������ΏI��
    if (time.IsNull()) {
      break;
    }

    // �����Ώۂ���v����C�x���g������(�C���L�[�_�E���ƃm�[�gON�͓����ɂȂ�\�������邪���̏ꍇ�͂��̂܂ܓ����ɏ�������)

    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      if (modifier_key_down_times_[i].Equals(time)) {
        keyboard_->Down(kVirtualModifierKeyCodes[i]);
        modifier_key_down_times_[i].SetNull();
      }
    }

    if (note_on_time_.Equals(time)) {
      // �C���L�[�𔲂����L�[�̃_�E�������s����
      keyboard_->Down(static_cast<std::uint8_t>(setting_->GetKeyBindByNoteNumber(on_note_number_)));

      // ���̃m�[�gON���X�P�W���[������
      ScheduleKeyboardNoteOn();
    }

    if (note_off_time_.Equals(time)) {
      // �C���L�[�𔲂����L�[�̃A�b�v�����s����
      keyboard_->Up(static_cast<std::uint8_t>(setting_->GetKeyBindByNoteNumber(off_note_number_)));

      // ���̃m�[�gOFF���X�P�W���[������
      ScheduleKeyboardNoteOff();
    }

    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      if (modifier_key_up_times_[i].Equals(time)) {
        keyboard_->Up(kVirtualModifierKeyCodes[i]);
        modifier_key_up_times_[i].SetNull();
      }
    }
  }

  // �ł������C�x���g�������擾

  bool hasNextEvent = false;
  std::uint32_t minium = 0;

  for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
    if (!modifier_key_down_times_[i].IsNull()) {
      if (!hasNextEvent) {
        minium = modifier_key_down_times_[i].millisecond;
        hasNextEvent = true;
      } else if (minium > modifier_key_down_times_[i].millisecond) {
        minium = modifier_key_down_times_[i].millisecond;
      }
    }
  }

  if (!note_on_time_.IsNull()) {
    if (!hasNextEvent) {
      minium = note_on_time_.millisecond;
      hasNextEvent = true;
    } else if (minium > note_on_time_.millisecond) {
      minium = note_on_time_.millisecond;
    }
  }

  if (!note_off_time_.IsNull()) {
    if (!hasNextEvent) {
      minium = note_off_time_.millisecond;
      hasNextEvent = true;
    } else if (minium > note_off_time_.millisecond) {
      minium = note_off_time_.millisecond;
    }
  }

  for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
    if (!modifier_key_up_times_[i].IsNull()) {
      if (!hasNextEvent) {
        minium = modifier_key_up_times_[i].millisecond;
        hasNextEvent = true;
      } else if (minium > modifier_key_up_times_[i].millisecond) {
        minium = modifier_key_up_times_[i].millisecond;
      }
    }
  }

  if (hasNextEvent) {
    *sleep_millisecond = minium - cursor;
  }

  return hasNextEvent;
}

void Sequencer::ScheduleKeyboardNoteOn() {
  // ����̃m�[�gON�������o�b�N�A�b�v
  KeyEventTime current_time = note_on_time_;

  // ����̃L�[�o�C���h���o�b�N�A�b�v
  Setting::KeyBind current_key = note_on_time_.IsNull() ? 0 : setting_->GetKeyBindByNoteNumber(on_note_number_);

  // ���̃m�[�gON�����Ɣԍ�����U�N���A
  note_on_time_.SetNull();
  on_note_number_ = 0;

  // ���̃m�[�gON�C�x���g��T��
  // ���[�h���Ɉ�x�`�F�b�N���Ă���̂Ō��ʂ͐������I�[�����Ȃ�
  StandardMidiFormatEvent event;
  while (note_on_reader_.Next(&event) == StandardMidiFormatReader::Result::kSuccess) {
    ++note_on_reader_count_;

    if (StandardMidiFormatEventIsValidNoteOn(event)) {
      note_on_time_.Set(note_on_reader_count_ - 1, static_cast<std::uint32_t>(event.microsecond / 1000));
      on_note_number_ = event.note_number;
      break;
    }
  }

  // ���̃L�[�o�C���h���擾
  Setting::KeyBind after_key = note_on_time_.IsNull() ? 0 : setting_->GetKeyBindByNoteNumber(on_note_number_);

  // �C���L�[�����ꂼ�ꏈ��
  for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
    if ((current_key & kModifierKeys[i]) == 0) {
      if ((after_key & kModifierKeys[i]) == 0) {
        // ����̃m�[�gON�Ǝ���̃m�[�gON�������Ƃ��C���L�[���g�p���Ȃ�

        // �������Ȃ�
      } else {
        // ����̃m�[�gON�������C���L�[���g�p����

        if (current_time.IsNull()) {
          // ����̃m�[�gON�������ꍇ

          if (note_on_time_.millisecond > setting_->GetModifierKeyDelayMillisecond()) {
            modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - setting_->GetModifierKeyDelayMillisecond());
          } else {
            modifier_key_down_times_[i].Set(note_on_time_.index, 0);
          }
        } else {
          // ����̃m�[�gON������̃m�[�gON���L��ꍇ(����̃m�[�gON�̏C���L�[���L��Ɣ��肳��Ă���̂Ŏ���̃m�[�gON�͗L��)

          // ����̃m�[�gON�Ǝ���̃m�[�gON�̊ԂɏC���L�[�p�̗P�\�����邩���m�F���A
          // ����Ȃ�P�\�̒ʂ�A����Ȃ���Η��m�[�gON�̒��ԂɎ���̃m�[�gON�̏C���L�[�_�E����\�肷��
          if (note_on_time_.millisecond - current_time.millisecond < setting_->GetModifierKeyDelayMillisecond() * 2) {
            modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - ((note_on_time_.millisecond - current_time.millisecond) / 2));
          } else {
            modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - setting_->GetModifierKeyDelayMillisecond());
          }
        }
      }
    } else {
      if ((after_key & kModifierKeys[i]) == 0) {
        // ����̃m�[�gON�������C���L�[���g�p����

        if (note_on_time_.IsNull()) {
          // ����̃m�[�gON�������ꍇ

          modifier_key_down_times_[i].Set(current_time.index, current_time.millisecond + setting_->GetModifierKeyDelayMillisecond());
        } else {
          // ����̃m�[�gON������̃m�[�gON���L��ꍇ(����̃m�[�gON�̏C���L�[���L��Ɣ��肳��Ă���̂ō���̃m�[�gON�͗L��)

          // ����̃m�[�gON�Ǝ���̃m�[�gON�̊ԂɏC���L�[�p�̗P�\�����邩���m�F���A
          // ����Ȃ�P�\�̒ʂ�A����Ȃ���Η��m�[�gON�̒��Ԃɍ���̃m�[�gON�̏C���L�[�A�b�v��\�肷��
          if (note_on_time_.millisecond - current_time.millisecond < setting_->GetModifierKeyDelayMillisecond() * 2) {
            modifier_key_up_times_[i].Set(current_time.index, current_time.millisecond + ((note_on_time_.millisecond - current_time.millisecond) / 2));
          } else {
            modifier_key_up_times_[i].Set(current_time.index, current_time.millisecond + setting_->GetModifierKeyDelayMillisecond());
          }
        }
      } else {
        // ����̃m�[�gON�Ǝ���̃m�[�gON�̗������C���L�[���g�p����

        // ����̃m�[�gON�Ǝ���̃m�[�gON���\������Ă���ꍇ�̂ݏC���L�[�̃A�b�v�_�E�����s��(�߂��ꍇ�̓A�b�v�_�E�����s�킸�_�E�������܂܂Ƃ���)
        if (note_on_time_.millisecond - current_time.millisecond > setting_->GetModifierKeyDownJoinMillisecond() && note_on_time_.millisecond - current_time.millisecond > setting_->GetModifierKeyDelayMillisecond() * 2) {
          modifier_key_up_times_[i].Set(current_time.index, current_time.millisecond + setting_->GetModifierKeyDelayMillisecond());
          modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - setting_->GetModifierKeyDelayMillisecond());
        }
      }
    }
  }
}

void Sequencer::ScheduleKeyboardNoteOff() {
  // ���̃m�[�gOFF��������ԂɃ��Z�b�g
  note_off_time_.SetNull();
  off_note_number_ = 0;

  // ���̃m�[�gOFF��T��
  // ���[�h���Ɉ�x�`�F�b�N���Ă���̂Ō��ʂ͐������I�[�����Ȃ�
  StandardMidiFormatEvent event;
  while (note_off_reader_.Next(&event) == StandardMidiFormatReader::Result::kSuccess) {
    ++note_off_reader_count_;

    if (StandardMidiFormatEventIsValidNoteOn(event)) {
      // �m�[�gON��������Ō�Ƀm�[�g��ON�ɂ��������Ƃ��ċL�^
      last_note_on_milliseconds_[event.note_number - Setting::kMinimumNoteNumber] = static_cast<std::uint32_t>(event.microsecond / 1000);
    } else if (StandardMidiFormatEventIsValidNoteOff(event)) {
      // �m�[�gOFF�������玟�̃m�[�gOFF�Ƃ��Đݒ肷��
      off_note_number_ = event.note_number;
      note_off_time_.Set(note_off_reader_count_ - 1, static_cast<std::uint32_t>(event.microsecond / 1000));

      // �m�[�gOFF�����͐ݒ肳��Ă��镪�������ۂ̎�����葁������
      // �������A����ɂ��Ō�̃m�[�gON��葁���Ȃ��Ă��܂����ꍇ�͍Ō�̃m�[�gON�Ɠ����Ƃ���(�����͓��������������Ƃ���ON�̌�ƂȂ�)
      if (note_off_time_.millisecond < last_note_on_milliseconds_[event.note_number - Setting::kMinimumNoteNumber] + setting_->GetEarlyKeyReleaseMillisecond()) {
        note_off_time_.millisecond = last_note_on_milliseconds_[event.note_number - Setting::kMinimumNoteNumber];
      } else {
        note_off_time_.millisecond -= setting_->GetEarlyKeyReleaseMillisecond();
      }

      break;
    }
  }
}

}  // namespace keysequencer