#ifndef KEYSEQUENCER_STANDARDMIDIFORMATEVENT_H_
#define KEYSEQUENCER_STANDARDMIDIFORMATEVENT_H_

#include <Windows.h>
#include <tchar.h>

#include <cstddef>
#include <cstdint>

namespace keysequencer {

/**
 * @brief SMFに含まれるイベント1つ分の情報
 */
struct StandardMidiFormatEvent {
 public:
  /**
   * @brief イベントの種類
   */
  enum class Type : std::uint8_t {
    /**
     * @brief MIDIイベント 
     */
    kMidi,

    /**
     * @brief SysExイベント 
     */
    kSysEx,

    /**
     * @brief メタイベント 
     */
    kMeta,
  };

  /**
   * @brief MIDIイベントの種類
   *
   * SMF内の実際のステータスバイトからチャンネル番号である下位4bitをクリアした値を使用する
   */
  enum class MidiType : std::uint8_t {
    /**
     * @brief ノートオフ
     */
    kNoteOff = 0x80,

    /**
     * @brief ノートオン
     */
    kNoteOn = 0x90,

    /**
     * @brief ポリフォニックキープレッシャー
     */
    kPolyphonicKeyPressure = 0xa0,

    /**
     * @brief コントロールチェンジ
     */
    kControlChange = 0xb0,

    /**
     * @brief プログラムチェンジ
     */
    kProgramChange = 0xc0,

    /**
     * @brief チャンネルプレッシャー
     */
    kChannelPressure = 0xd0,

    /**
     * @brief ピッチベンド
     */
    kPitchBend = 0xe0,
  };

  /**
   * @brief SysExイベントの種類
   *
   * SMF内の実際のステータスバイトの値を使用する
   */
  enum class SysExType : std::uint8_t {
    /**
     * @brief F0
     */
    kF0 = 0xf0,

    /**
     * @brief F7
     */
    kF7 = 0xf7,
  };

  /**
   * @brief メタイベントの種類
   *
   * SMF内の実際のメタイベントの値を使用する
   */
  enum class MetaType : std::uint8_t {
    /**
     * @brief シーケンス番号
     */
    kSequenceNumber = 0x00,

    /**
     * @brief テキスト
     */
    kText = 0x01,

    /**
     * @brief 著作権表示
     */
    kCopyright = 0x02,

    /**
     * @brief シーケンス名(曲タイトル)・トラック名
     */
    kSequence = 0x03,

    /**
     * @brief 楽器名
     */
    kInstrumentName = 0x04,

    /**
     * @brief 歌詞
     */
    kLyric = 0x05,

    /**
     * @brief マーカー
     */
    kMarker = 0x06,

    /**
     * @brief キューポイント
     */
    kCuePoint = 0x07,

    /**
     * @brief プログラム名 (音色名) 
     */
    kProgramName = 0x08,

    /**
     * @brief デバイス名 (音源名) 
     */
    kDeviceName = 0x09,

    /**
     * @brief MIDIチャンネルプリフィックス
     */
    kMidiChannelPrefix = 0x20,

    /**
     * @brief ポート指定
     */
    kMidiPort = 0x21,

    /**
     * @brief トラック終端
     */
    kEndOfTrack = 0x2f,

    /**
     * @brief テンポ設定
     */
    kTempo = 0x51,

    /**
     * @brief SMPTEオフセット
     */
    kSmpteOffset = 0x54,

    /**
     * @brief 拍子の設定
     */
    kTimeSignature = 0x58,

    /**
     * @brief 調の設定
     */
    kKeySignature = 0x59,

    /**
     * @brief シーケンサ特定メタイベント
     */
    kSequencerSpecificEvent = 0x7f,
  };

  /**
   * @brief イベントを実行する再生開始からの時間(マイクロ秒)
   * 
   * 直接イベントに含まれるデータではないが、読み取り時に計算した結果をセットする
   */
  std::uint64_t microsecond;

  /**
   * @brief トラック番号
   */
  std::uint16_t track_number;

  /**
   * @brief イベントの種類
   */
  Type type;

  // イベント種類によって使用するデータが排他的に変わるのでunionを使用
  union {
    //  MIDIイベントで使用する値
    struct {
      /**
       * @brief MIDIイベントの種類
       */
      MidiType midi_type;

      /**
       * @brief 対象のチャンネル
       */
      std::uint8_t channel;

      // MIDIイベントの種類によっても使用するデータが排他的に変わるのでさらにunionを使用
      union {
        /**
         * @brief ピッチベンド
         */
        std::uint16_t pitch_bend;

        struct {
          /**
           * @brief ノート番号
           *
           * いわゆる音階
           */
          std::uint8_t note_number;

          /**
           * @brief ベロシティ
           *
           * 鍵盤を押したり離したりする時の速度情報
           * 基本的には音量的な意味
           */
          std::uint8_t velocity;
        };

        struct {
          /**
           * @brief コントロール番号
           *
           * コントロールチェンジイベントの場合に対象のコントロール番号がセットされる
           * チャンネルモードメッセージの場合メッセージ番号(0x78-0x7f)が入る
           */
          std::uint8_t control_number;

          /**
           * @brief コントロールチェンジの値
           */
          std::uint8_t control_value;
        };

        /**
         * @brief プログラムチェンジの値
         *
         * ギターとかピアノとかみたいな音色の指定
         */
        std::uint8_t program;
      };
    };

    /**
     * @brief SysExイベント
     */
    struct {
      /**
       * @brief SysExイベントの種類
       */
      SysExType sys_ex_type;

      /**
       * @brief 転送データ長
       *
       * dataに格納されているデータサイズ
       */
      std::uint32_t data_length;

      /**
       * @brief データの中身
       *
       * データサイズはdataLengthを参照
       * データサイズが0の場合はnullptrとなる
       */
      const std::uint8_t* data;
    };

    /**
     * @brief メタイベント
     */
    struct {
      /**
       * @brief メタイベントの種類
       */
      MetaType meta_type;

      /**
       * @brief データ長
       *
       * dataに格納されているデータサイズ
       */
      std::uint32_t data_length;

      /**
       * @brief データの中身
       *
       * データサイズはdataLengthを参照
       * データサイズが0の場合はnullptrとなる
       */
      const std::uint8_t* data;
    };
  };

  /**
   * @brief コンストラクタ
   *
   * 生成直後の状態そのままで使うことは無い想定であるが一応ノートOFFで初期化する
   */
  StandardMidiFormatEvent() : microsecond(0), track_number(0), type(Type::kMidi), midi_type(MidiType::kNoteOff), channel(0), pitch_bend(0), data_length(0), data(nullptr) {
  }
};
}  // namespace keysequencer
#endif