#ifndef KEYSEQUENCER_MIDIINPUT_H_
#define KEYSEQUENCER_MIDIINPUT_H_

#include <Windows.h>

namespace keysequencer {

class Setting;
class Keyboard;

/**
 * @brief MIDI入力をキーボード入力に変換する処理
 */
class MidiDeviceInputToKeyboard {
 public:
  /**
    * @brief Open結果
    */
  enum class OpenResult {
    /**
     * @brief 正常終了 
     */
    kSuccess,

    /**
     * @brief 不明なエラー
     */
    kFailureUnknown,

    /**
     * @brief デバイスが接続されていませんエラー
     */
    kFailureDeviceNotFound,
  };

  /**
   * @brief デフォルトコンストラクタ
   */
  MidiDeviceInputToKeyboard() = delete;

  /**
   * @brief コンストラクタ
   * 
   * @param keyboard キーボード
   * @param setting 設定
   */
  MidiDeviceInputToKeyboard(const Setting* setting, Keyboard* keyboard);

  /**
   * @brief コピーコンストラクタ
   */
  MidiDeviceInputToKeyboard(const MidiDeviceInputToKeyboard&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  MidiDeviceInputToKeyboard(MidiDeviceInputToKeyboard&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  MidiDeviceInputToKeyboard& operator=(const MidiDeviceInputToKeyboard&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  MidiDeviceInputToKeyboard& operator=(MidiDeviceInputToKeyboard&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~MidiDeviceInputToKeyboard();

  /**
   * @brief 変換を開始しているか確認する
   * 
   * @return 変換中の場合にtrueを返す
   */
  inline bool IsOpened() {
    return midi_in_handle_ != nullptr;
  }

  /**
   * @brief 変換開始
   */
  OpenResult Open();

  /**
   * @brief 変換終了
   */
  void Close();

 private:
  /**
   * @brief 設定
   */
  const Setting* const setting_;

  /**
   * @brief キーボード
   */
  Keyboard* const keyboard_;

  /**
   * @brief ハンドル
   */
  HMIDIIN midi_in_handle_ = nullptr;

  /**
   * @brief midiInOpenから要求されるMIDI入力を受け取った時のコールバック処理
   */
  static void CALLBACK CallbackProcedure(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2);

  /**
   * @brief MIDIキーボード入力を処理
   * 
   * @param message メッセージ
   * @param param1 メッセージのパラメータ1(現状1だけ使っているので)
   */
  void Procedure(UINT message, DWORD param1);
};

}  // namespace keysequencer

#endif