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

  // 最低14Byte無いとSMFとして成立しない
  if (length < 14) {
    return Result::kFailureIllegalSmf;
  }

  // MThdの文字列を確認
  if (ReadUint32(cursor) != 0x4d546864) {
    return Result::kFailureIllegalSmf;
  }

  // ヘッダチャンクの大きさ(6固定)
  if (ReadUint32(cursor) != 6) {
    return Result::kFailureIllegalSmf;
  }

  // フォーマットを取得(Format1または2のみ対応)
  std::uint16_t format = ReadUint16(cursor);
  if (format != static_cast<std::uint16_t>(Format::kType0) && format != static_cast<std::uint16_t>(Format::kType1)) {
    return Result::kFailureNotSupportSmf;
  }

  // トラック数を取得(最低1つは必要)
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
    // トラックヘッダ8Byteが必須
    if (cursor + 7 >= end) {
      delete[] track_readers;
      return Result::kFailureIllegalSmf;
    }

    // MTrkの文字列
    if (ReadUint32(cursor) != 0x4d54726b) {
      delete[] track_readers;
      return Result::kFailureIllegalSmf;
    }

    // トラックデータ長の読み取り(一応0も対応しているが0はあり得るのか？)
    std::uint32_t track_length = ReadUint32(cursor);

    // トラックデータを初期化
    track_readers[i].start = cursor;
    track_readers[i].end = cursor + track_length;
    track_readers[i].cursor = cursor;
    track_readers[i].last_midi_event_status = 0;
    track_readers[i].last_event_microsecond = 0;

    if (track_readers[i].start < track_readers[i].end) {
      // 最初のイベントのデルタタイムを読み取る
      if (!ReadVariableUint(track_readers[i].cursor, track_readers[i].end, track_readers[i].delta_time)) {
        delete[] track_readers;
        return Result::kFailureIllegalSmf;
      }

      // トラックデータをスキップ
      cursor += track_length;
      if (cursor > end) {
        delete[] track_readers;
        return Result::kFailureIllegalSmf;
      }
    }
  }

  // フォーマットを確認
  if (format == static_cast<std::uint16_t>(Format::kType0)) {
    format_ = Format::kType0;
  } else if (format == static_cast<std::uint16_t>(Format::kType1)) {
    format_ = Format::kType1;
  }

  // タイムベースを読み取り
  quarter_note_tick_ = quarter_note_tick;

  // トラック数を読み取り
  track_ = track;
  if (track_readers_ != nullptr) {
    delete[] track_readers_;
  }
  track_readers_ = track_readers;

  // 読み取りデータの初期化
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
    // 読み取りカーソルを先頭に
    track_readers_[i].cursor = track_readers_[i].start;

    // ステータスを初期化
    track_readers_[i].last_midi_event_status = 0;
    track_readers_[i].last_event_microsecond = 0;

    // 最初のイベントのデルタタイムを読み取る
    // Openで読み取れることを確認しているのでここでは再チェックしない
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

  // 全トラックから次に処理するイベントを検索
  // セットテンポの存在があるのでデルタタイムではなく実時間の累計で判断する
  // デルタタイム中にセットテンポが来た場合、
  // セットテンポ前後のデルタタイム部分の両方をセットテンポ後の値で計算しているが
  // ここの正確な仕様が見つからないためもしかしたら間違っているかもしれない
  // TODO: 実際にそういうデータを作って他のソフトの動作を調査する
  std::size_t track = 0;
  std::size_t target_track = 0;
  std::uint64_t minimum_microsecond = 0;
  for (; track < track_; ++track) {
    if (track_readers_[track].cursor < track_readers_[track].end) {
      target_track = track;

      // 演奏開始からの経過時間(マイクロ秒)を計算する
      // デルタタイム(前のイベントからの相対分解能) ÷ 時間単位(分解能/四分音符) × テンポ(マイクロ秒/四分音符)
      // そもそも小数点以下が出るような値に設定はされないし
      // されたとしても一般的に切り捨てる想定らしいのでそのまま処理する
      minimum_microsecond = track_readers_[track].last_event_microsecond + (static_cast<std::uint64_t>(track_readers_[track].delta_time) * static_cast<std::uint64_t>(tempo_)) / quarter_note_tick_;
      break;
    }
  }

  // イベントが残っていなければ終了
  if (track >= track_) {
    return Result::kFailureEndOfFile;
  }

  // 他にイベントの残っているトラックがあればその中から一番小さい経過時間を選ぶ
  // (デルタタイムが同一ならトラック番号が低い方を選ぶ)
  for (++track; track < track_; ++track) {
    if (track_readers_[track].cursor < track_readers_[track].end) {
      std::uint64_t microsecond = track_readers_[track].last_event_microsecond + (static_cast<std::uint64_t>(track_readers_[track].delta_time) * static_cast<std::uint64_t>(tempo_)) / quarter_note_tick_;
      if (microsecond < minimum_microsecond) {
        target_track = track;
        minimum_microsecond = microsecond;
      }
    }
  }

  // 選ばれたイベントの時間をセット
  event->microsecond = minimum_microsecond;

  // 記述の簡略化のために一時変数にコピー
  const std::uint8_t* cursor = track_readers_[target_track].cursor;
  const std::uint8_t* const end = track_readers_[target_track].end;

  // ランニングステータスを考慮してステータスバイトを読み取り
  std::uint8_t const status = (*cursor & 0x80) == 0 ? track_readers_[target_track].last_midi_event_status : *cursor;
  if ((*cursor & 0x80) == 0) {
    --cursor;  // ステータスバイトが有るときと無い時で同じ処理が出来る様、無い時に-1して補正する
  }

  switch (status & 0xf0) {
    // ノート OFF
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

    // ノート ON
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

    // ポリフォニックキープレッシャー
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

    // コントロールチェンジ or チャンネルモードメッセージ
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
        // POLYモード
        mono_ = false;
      } else if (cursor[1] == 0x7f) {
        // MONOモード
        mono_ = true;
        if (!omni_) {
          // このケースだけイベントのサイズが4byteになる
          if (cursor + 3 >= end) {
            return Result::kFailureIllegalSmf;
          }
          ++cursor;
        }
      }
      cursor += 3;
      break;

    // プログラムチェンジ
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

    // チャンネルプレッシャー
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

    // ピッチベンド
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
        // SysExイベント (F0)
        event->type = StandardMidiFormatEvent::Type::kSysEx;
        event->sys_ex_type = StandardMidiFormatEvent::SysExType::kF0;
        if (!ReadVariableUint(++cursor, end, event->data_length)) {
          return Result::kFailureIllegalSmf;
        }

        // データ長に終端(0xf7)が含まれるので1以上であるはず)
        if (event->data_length == 0) {
          return Result::kFailureIllegalSmf;
        }

        // 本来なら先頭に0xf0が付くのだが、SysExイベントは使う予定がないことと、処理の簡略化のため、先頭0xf0が付かないデータを渡す
        if (cursor + event->data_length > end) {
          return Result::kFailureIllegalSmf;
        }
        event->data = cursor;

        cursor += event->data_length;
      } else if (status == 0xf7) {
        // SysExイベント (F7)
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
        // メタイベント
        event->type = StandardMidiFormatEvent::Type::kMeta;

        // メタイベント種別 + データ長(最低1Byte)が必要
        if (cursor + 2 >= end) {
          return Result::kFailureIllegalSmf;
        }

        // メタイベント種別を読み取り
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

        // トラックエンドが来た場合通常は終端になっているはずだが念のため明示的に終端にセットする
        if (event->meta_type == StandardMidiFormatEvent::MetaType::kEndOfTrack) {
          cursor = end;
        }
      }
      break;

    default:
      return Result::kFailureIllegalSmf;
  }

  // テンポ設定イベントの場合のみの特別処理
  // 内部処理用にテンポの値を保持しておく
  if (event->type == StandardMidiFormatEvent::Type::kMeta && event->meta_type == StandardMidiFormatEvent::MetaType::kTempo) {
    if (event->data_length != 3) {
      return Result::kFailureIllegalSmf;
    }

    tempo_ = (static_cast<std::uint32_t>(event->data[0]) << 16) | (static_cast<std::uint32_t>(event->data[1]) << 8) | static_cast<std::uint32_t>(event->data[2]);
  }

  // TrackReaderを更新

  // 終端でなければ次のメッセージのデルタタイムを読み取っておく
  if (cursor < end) {
    if (!ReadVariableUint(cursor, end, track_readers_[target_track].delta_time)) {
      return Result::kFailureIllegalSmf;
    }
  }

  // 読み込みカーソルを更新
  track_readers_[target_track].cursor = cursor;

  // MIDIイベントの場合にランニングステータスのためにステータスバイトを記録しておく
  if (status < 0xf0) {
    track_readers_[target_track].last_midi_event_status = status;
  }

  // 時刻を更新
  track_readers_[target_track].last_event_microsecond = event->microsecond;

  return Result::kSuccess;
}

}  // namespace keysequencer