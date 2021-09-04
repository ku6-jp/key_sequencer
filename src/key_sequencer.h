#ifndef KEYSEQUENCER_KEYSEQUENCER_H_
#define KEYSEQUENCER_KEYSEQUENCER_H_

#include "keyboard.h"
#include "main_window.h"
#include "midi_device_input_to_keyboard.h"
#include "sequencer_manager.h"
#include "setting.h"
#include "string_table.h"

namespace keysequencer {

/**
 * @brief アプリ本体
 */
class KeySequencer {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  KeySequencer() = delete;

  /**
   * @brief コンストラクタ
   * 
   * @param instance_handle アプリケーションのインスタンスハンドル
   */
  KeySequencer(HINSTANCE instance_handle);

  /**
   * @brief コピーコンストラクタ
   */
  KeySequencer(const KeySequencer&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  KeySequencer(KeySequencer&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  KeySequencer& operator=(const KeySequencer&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  KeySequencer& operator=(KeySequencer&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~KeySequencer() = default;

  /**
   * @brief 実行
   *
   * @return WinMain用戻り値
   */
  int Execute();

 private:
  /**
   * @brief リソースの文字列テーブル
   */
  StringTable string_table_;

  /**
   * @brief 設定 
   */
  Setting setting_;

  /**
   * @brief キーボード 
   */
  Keyboard keyboard_;

  /**
   * @brief MIDI機器の入力をキーボード入力に変換する処理 
   */
  MidiDeviceInputToKeyboard midi_input_to_keyboard_;

  /**
   * @brief 自動演奏処理 
   */
  SequencerManager sequencer_manager_;

  /**
   * @brief メインウィンドウ 
   */
  MainWindow main_window_;
};

}  // namespace keysequencer

#endif