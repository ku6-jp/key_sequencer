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

  // ファイルを読み込み
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

  // Readerを初期化
  switch (note_on_reader_.Initialize(standard_midi_format_.GetData(), standard_midi_format_.GetLength())) {
    case StandardMidiFormatReader::Result::kSuccess:
      break;
    case StandardMidiFormatReader::Result::kFailureIllegalSmf:
      return LoadFileResult::kFailureIllegalSmf;
    case StandardMidiFormatReader::Result::kFailureNotSupportSmf:
      return LoadFileResult::kFailureNotSupportSmf;
    default:
      // その他は返らない想定であるが一応不正なSMF扱いとしておく
      return LoadFileResult::kFailureIllegalSmf;
  }

  // 同じ結果になるのでチェックはしない
  note_off_reader_.Initialize(standard_midi_format_.GetData(), standard_midi_format_.GetLength());

  // 一度全体をスキャンしてエラーが発生しないか確認
  StandardMidiFormatEvent event;
  for (;;) {
    switch (note_on_reader_.Next(&event)) {
      case StandardMidiFormatReader::Result::kSuccess:
        // 次のイベント
        break;
      case StandardMidiFormatReader::Result::kFailureEndOfFile:
        // 終端に達したら正常終了
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
    // 出力先がキーボードの場合

    // キーボード入力状態を一旦リセット
    keyboard_->Reset();

    // Readerの読み込み位置を先頭に戻す
    note_on_reader_.Rewind();
    note_off_reader_.Rewind();
    note_on_reader_count_ = 0;
    note_off_reader_count_ = 0;

    // 次のノートON/OFFをリセット(無しに設定)
    note_on_time_.SetNull();
    note_off_time_.SetNull();
    on_note_number_ = 0;
    off_note_number_ = 0;

    // 次の修飾キーアップダウン時間をリセット(無しに設定)
    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      modifier_key_down_times_[i].SetNull();
      modifier_key_up_times_[i].SetNull();
    }

    // 最後にノートをONにした時間をリセット
    for (std::size_t i = 0; i < Setting::kMaximumNoteNumber - Setting::kMinimumNoteNumber; ++i) {
      last_note_on_milliseconds_[i] = 0;
    }

    // 次のノートONを読み出して予定する
    ScheduleKeyboardNoteOn();

    // 次のノートOFFを読み出して予定する
    ScheduleKeyboardNoteOff();
  } else if (output == Output::kMidiSynthesizer) {
    // 出力先がMIDI音声の場合

    // MIDI音声出力を開く
    if (!midi_synthesizer_.Open()) {
      return false;
    }

    // Readerの読み込み位置を先頭に戻す
    // MIDI音声出力時はReaderは1つ有ればいいだけなのでOFF用を使用する
    note_off_reader_.Rewind();

    // 次に処理するイベントとして最初の1つを読み込む
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
  // 演奏開始からの経過時間(ミリ秒)を取得
  std::uint32_t cursor = timeGetTime() - play_start_millisecond_;

  // 取得した経過時間までに処理すべきイベントを順に処理
  while (static_cast<std::uint32_t>(event_.microsecond / 1000) <= cursor) {
    // 取得済みのイベントを処理
    if (StandardMidiFormatEventIsValidNoteOn(event_)) {
      midi_synthesizer_.NoteOn(event_.note_number);
    } else if (StandardMidiFormatEventIsValidNoteOff(event_)) {
      midi_synthesizer_.NoteOff(event_.note_number);
    }

    // 次のイベントを取得
    if (note_off_reader_.Next(&event_) != StandardMidiFormatReader::Result::kSuccess) {
      return false;
    }
  }

  // 次のイベントまでの待ち時間をセット
  *sleep_millisecond = static_cast<std::uint32_t>(event_.microsecond / 1000) - cursor;

  return true;
}

bool Sequencer::RunKeyboardOutput(DWORD* sleep_millisecond) {
  // 演奏開始からの経過時間(ミリ秒)を取得
  std::uint32_t cursor = timeGetTime() - play_start_millisecond_;

  for (;;) {
    KeyEventTime time;

    // 最も先に処理すべきイベントを調べる

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

    // 処理対象が無ければ終了
    if (time.IsNull()) {
      break;
    }

    // 処理対象が一致するイベントを処理(修飾キーダウンとノートONは同時になる可能性があるがその場合はそのまま同時に処理する)

    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      if (modifier_key_down_times_[i].Equals(time)) {
        keyboard_->Down(kVirtualModifierKeyCodes[i]);
        modifier_key_down_times_[i].SetNull();
      }
    }

    if (note_on_time_.Equals(time)) {
      // 修飾キーを抜いたキーのダウンを実行する
      keyboard_->Down(static_cast<std::uint8_t>(setting_->GetKeyBindByNoteNumber(on_note_number_)));

      // 次のノートONをスケジュールする
      ScheduleKeyboardNoteOn();
    }

    if (note_off_time_.Equals(time)) {
      // 修飾キーを抜いたキーのアップを実行する
      keyboard_->Up(static_cast<std::uint8_t>(setting_->GetKeyBindByNoteNumber(off_note_number_)));

      // 次のノートOFFをスケジュールする
      ScheduleKeyboardNoteOff();
    }

    for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
      if (modifier_key_up_times_[i].Equals(time)) {
        keyboard_->Up(kVirtualModifierKeyCodes[i]);
        modifier_key_up_times_[i].SetNull();
      }
    }
  }

  // 最も早いイベント時刻を取得

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
  // 今回のノートON時刻をバックアップ
  KeyEventTime current_time = note_on_time_;

  // 今回のキーバインドをバックアップ
  Setting::KeyBind current_key = note_on_time_.IsNull() ? 0 : setting_->GetKeyBindByNoteNumber(on_note_number_);

  // 次のノートON時刻と番号を一旦クリア
  note_on_time_.SetNull();
  on_note_number_ = 0;

  // 次のノートONイベントを探す
  // ロード時に一度チェックしているので結果は成功か終端しかない
  StandardMidiFormatEvent event;
  while (note_on_reader_.Next(&event) == StandardMidiFormatReader::Result::kSuccess) {
    ++note_on_reader_count_;

    if (StandardMidiFormatEventIsValidNoteOn(event)) {
      note_on_time_.Set(note_on_reader_count_ - 1, static_cast<std::uint32_t>(event.microsecond / 1000));
      on_note_number_ = event.note_number;
      break;
    }
  }

  // 次のキーバインドを取得
  Setting::KeyBind after_key = note_on_time_.IsNull() ? 0 : setting_->GetKeyBindByNoteNumber(on_note_number_);

  // 修飾キーをそれぞれ処理
  for (std::size_t i = 0; i < kModifierKeyLength; ++i) {
    if ((current_key & kModifierKeys[i]) == 0) {
      if ((after_key & kModifierKeys[i]) == 0) {
        // 今回のノートONと次回のノートONが両方とも修飾キーを使用しない

        // 何もしない
      } else {
        // 次回のノートONだけが修飾キーを使用する

        if (current_time.IsNull()) {
          // 今回のノートONが無い場合

          if (note_on_time_.millisecond > setting_->GetModifierKeyDelayMillisecond()) {
            modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - setting_->GetModifierKeyDelayMillisecond());
          } else {
            modifier_key_down_times_[i].Set(note_on_time_.index, 0);
          }
        } else {
          // 今回のノートONも次回のノートONも有る場合(次回のノートONの修飾キーが有ると判定されているので次回のノートONは有る)

          // 今回のノートONと次回のノートONの間に修飾キー用の猶予が入るかを確認し、
          // 入るなら猶予の通り、入らなければ両ノートONの中間に次回のノートONの修飾キーダウンを予定する
          if (note_on_time_.millisecond - current_time.millisecond < setting_->GetModifierKeyDelayMillisecond() * 2) {
            modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - ((note_on_time_.millisecond - current_time.millisecond) / 2));
          } else {
            modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - setting_->GetModifierKeyDelayMillisecond());
          }
        }
      }
    } else {
      if ((after_key & kModifierKeys[i]) == 0) {
        // 今回のノートONだけが修飾キーを使用する

        if (note_on_time_.IsNull()) {
          // 次回のノートONが無い場合

          modifier_key_down_times_[i].Set(current_time.index, current_time.millisecond + setting_->GetModifierKeyDelayMillisecond());
        } else {
          // 今回のノートONも次回のノートONも有る場合(今回のノートONの修飾キーが有ると判定されているので今回のノートONは有る)

          // 今回のノートONと次回のノートONの間に修飾キー用の猶予が入るかを確認し、
          // 入るなら猶予の通り、入らなければ両ノートONの中間に今回のノートONの修飾キーアップを予定する
          if (note_on_time_.millisecond - current_time.millisecond < setting_->GetModifierKeyDelayMillisecond() * 2) {
            modifier_key_up_times_[i].Set(current_time.index, current_time.millisecond + ((note_on_time_.millisecond - current_time.millisecond) / 2));
          } else {
            modifier_key_up_times_[i].Set(current_time.index, current_time.millisecond + setting_->GetModifierKeyDelayMillisecond());
          }
        }
      } else {
        // 今回のノートONと次回のノートONの両方が修飾キーを使用する

        // 今回のノートONと次回のノートONが十分離れている場合のみ修飾キーのアップダウンを行う(近い場合はアップダウンを行わずダウンしたままとする)
        if (note_on_time_.millisecond - current_time.millisecond > setting_->GetModifierKeyDownJoinMillisecond() && note_on_time_.millisecond - current_time.millisecond > setting_->GetModifierKeyDelayMillisecond() * 2) {
          modifier_key_up_times_[i].Set(current_time.index, current_time.millisecond + setting_->GetModifierKeyDelayMillisecond());
          modifier_key_down_times_[i].Set(note_on_time_.index, note_on_time_.millisecond - setting_->GetModifierKeyDelayMillisecond());
        }
      }
    }
  }
}

void Sequencer::ScheduleKeyboardNoteOff() {
  // 次のノートOFFが無い状態にリセット
  note_off_time_.SetNull();
  off_note_number_ = 0;

  // 次のノートOFFを探す
  // ロード時に一度チェックしているので結果は成功か終端しかない
  StandardMidiFormatEvent event;
  while (note_off_reader_.Next(&event) == StandardMidiFormatReader::Result::kSuccess) {
    ++note_off_reader_count_;

    if (StandardMidiFormatEventIsValidNoteOn(event)) {
      // ノートONが来たら最後にノートをONにした時刻として記録
      last_note_on_milliseconds_[event.note_number - Setting::kMinimumNoteNumber] = static_cast<std::uint32_t>(event.microsecond / 1000);
    } else if (StandardMidiFormatEventIsValidNoteOff(event)) {
      // ノートOFFが来たら次のノートOFFとして設定する
      off_note_number_ = event.note_number;
      note_off_time_.Set(note_off_reader_count_ - 1, static_cast<std::uint32_t>(event.microsecond / 1000));

      // ノートOFF時刻は設定されている分だけ実際の時刻より早くする
      // ただし、それにより最後のノートONより早くなってしまった場合は最後のノートONと同時とする(時刻は同時だが処理順としてONの後となる)
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