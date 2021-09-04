#ifndef KEYSEQUENCER_MIDISYNTHESIZER_H_
#define KEYSEQUENCER_MIDISYNTHESIZER_H_

#include <Windows.h>

#include <cstdint>

namespace keysequencer {
/**
 * @brief MIDIシンセサイザー
 *
 * SMFの演奏を制御するわけではなく、単に音の出力をON/OFFするのみ
 */
class MidiSynthesizer {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  MidiSynthesizer() = default;

  /**
   * @brief コピーコンストラクタ
   */
  MidiSynthesizer(const MidiSynthesizer&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  MidiSynthesizer(MidiSynthesizer&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  MidiSynthesizer& operator=(const MidiSynthesizer&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  MidiSynthesizer& operator=(MidiSynthesizer&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~MidiSynthesizer();

  /**
   * @brief 演奏を開始する
   */
  bool Open();

  /**
   * @brief 演奏を終了する
   */
  void Close();

  /**
   * @brief 指定ノートの音の出力を開始する
   */
  void NoteOn(std::uint8_t const note_number);

  /**
   * @brief 指定ノートの音の出力を終了する
   */
  void NoteOff(std::uint8_t const note_number);

 private:
  /**
   * MIDI出力ハンドル
   */
  HMIDIOUT midi_out_handle_ = nullptr;
};

}  // namespace keysequencer

#endif