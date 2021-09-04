#include "key_sequencer.h"

#include "constants.h"

namespace keysequencer {

KeySequencer::KeySequencer(HINSTANCE instance_handle) : string_table_(instance_handle), midi_input_to_keyboard_(&setting_, &keyboard_), sequencer_manager_(&setting_, &keyboard_), main_window_(instance_handle, &string_table_, &setting_, &midi_input_to_keyboard_, &sequencer_manager_) {
}

int KeySequencer::Execute() {
  // マルチメディアタイマー精度を設定
  // https://docs.microsoft.com/ja-jp/windows/win32/multimedia/obtaining-and-setting-timer-resolution
  // 演奏処理でtimeGetTime()を使用するため
  // また、演奏処理ではスレッド制御にWaitForSingleObject()を使用しているが、そこにも効いてくるらしい？
  TIMECAPS timecaps;
  if (timeGetDevCaps(&timecaps, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
    MessageBox(nullptr, string_table_.get(IDS_ERROR_APPLICATION_INITIALIZE), string_table_.get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
    return 1;
  }
  timeBeginPeriod(min(max(timecaps.wPeriodMin, 1), timecaps.wPeriodMax));

  // 設定ファイルをロード
  if (!setting_.LoadIniFile()) {
    MessageBox(nullptr, string_table_.get(IDS_ERROR_APPLICATION_INITIALIZE), string_table_.get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
    return 2;
  }

  // メインウィンドウを生成
  if (!main_window_.Create()) {
    MessageBox(nullptr, string_table_.get(IDS_ERROR_APPLICATION_INITIALIZE), string_table_.get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
    return 3;
  }

  main_window_.Run();
  return 0;
}

}  // namespace keysequencer