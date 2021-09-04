#ifndef KEYSEQUENCER_STANDARDMIDIFORMATREADER_H_
#define KEYSEQUENCER_STANDARDMIDIFORMATREADER_H_

#include <cstddef>
#include <cstdint>

namespace keysequencer {

struct StandardMidiFormatEvent;

/**
 * @brief SMF読み取り処理 
 * 
 * SMFバイナリデータからSMFイベントとして読み取る
 */
class StandardMidiFormatReader {
 public:
  /**
   * @brief 操作結果
   */
  enum class Result {
    /**
     * @brief 正常終了 
     */
    kSuccess,

    /**
     * @brief 初期化処理されていないため操作できない 
     */
    kFailureUninitialized,

    /**
     * @brief 終端に達したため読み出せない
     */
    kFailureEndOfFile,

    /**
     * @brief SMFとして不正
     */
    kFailureIllegalSmf,

    /**
     * @brief SMFとしては正しいが本プログラムがサポートしていないSMF形式
     */
    kFailureNotSupportSmf,
  };

  /**
   * @brief デフォルトコンストラクタ
   */
  StandardMidiFormatReader() = default;

  /**
   * @brief コピーコンストラクタ
   */
  StandardMidiFormatReader(const StandardMidiFormatReader&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  StandardMidiFormatReader(StandardMidiFormatReader&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  StandardMidiFormatReader& operator=(const StandardMidiFormatReader&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  StandardMidiFormatReader& operator=(StandardMidiFormatReader&&) = delete;

  /**
   * @brief デストラクタ 
   */
  ~StandardMidiFormatReader();

  /**
   * @brief SMFバイナリデータを渡して読み取り処理を初期化する
   * 
   * @param start SMFバイナリデータ
   * @param length SMFバイナリデータの長さ
   * 
   * @return 読み取り結果
   */
  Result Initialize(const std::uint8_t* const start, std::size_t const length);

  /**
   * @brief 次のSMFイベントを1つ読み取る
   *
   * 同時に読み取りカーソルが移動する
   * 
   * @param event 読み取り結果を書き込む構造体のポインタ
   *              読み取り結果が正常終了の場合以外はeventに半端な内容を書き込むため参照しないこと
   * 
   * @return 読み取り結果
   */
  Result Next(StandardMidiFormatEvent* event);

  /**
   * @brief 読み取り位置を先頭に戻す
   */
  Result Rewind();

 private:
  /**
   * @brief フォーマットタイプ
   *
   * もう一つフォーマット2というのも規格上存在するのだが
   * 使用することが稀であり構造もイマイチよくわからんので現時点では非対応とする
   */
  enum class Format : std::uint16_t {
    kType0 = 0,  // フォーマット0: 1トラックに全てのイベントをまとめた形式
    kType1 = 1,  // フォーマット1: 同期演奏される複数のトラックで構成される
  };

  /**
   * @brief トラック読出し用データ
   */
  struct TrackReader {
   public:
    /**
     * @brief トラック開始位置
     *
     * トラック最初のメッセージの先頭バイトのポインタ
     * MTrkやトラックデータ長の位置ではない
     */
    const std::uint8_t* start;

    /**
     * @brief トラック終了位置
     *
     * トラックエンドの最終バイトの次のポインタ
     * start + トラックデータ長
     */
    const std::uint8_t* end;

    /**
     * @brief 読出しカーソル
     *
     * デルタタイムは予め読み取っておくので、デルタタイムの次にあるイベントデータの開始位置を指す
     * (終端に達した場合は終端を指す)
     */
    const std::uint8_t* cursor;

    /**
     * @brief
     * 
     * ランニングステータスのために1つ前のMIDIイベントのステータスバイトを記録しておく
     */
    std::uint8_t last_midi_event_status;

    /**
     * @brief 読出しカーソルが指すイベントのデルタタイム
     *
     * 直近のメッセージを検索するために複数回参照するのでデコードの上保持しておく
     */
    std::uint32_t delta_time;

    /**
     * @brief 読出しカーソル直前のイベントの再生開始からの経過時間
     *
     * イベントのソートのために経過時間を保持しておく(単位: マイクロ秒)
     */
    std::uint64_t last_event_microsecond;
  };

  /**
   * @brief フォーマット
   */
  Format format_ = Format::kType0;

  /**
   * @brief 四分音符の分解能
   *
   * タイムベースがテンポベースの場合に四分音符の分解能が入る(単位: Tick)
   * 各イベントのデルタタイムはこの数字に対する割合が入る
   *
   * 例:
   * タイムベース(四分音符の分解能)が480Tickの場合、デルタタイムが全音符なら1960Tick、八分音符なら240Tickとなる)
   * これを実時間にするためにはテンポが必要となり、以下の計算式で求められる
   * デルタタイム(マイクロ秒) =
   * (デルタタイム(Tick) / タイムベース(Tick)) *  テンポ設定(マイクロ秒/Tick)
   */
  std::uint16_t quarter_note_tick_ = 0;

  /**
   * @brief トラック数
   *
   * フォーマット0の場合は1固定 それ以外の場合はトラック数
   */
  std::uint16_t track_ = 0;

  /**
   * @brief トラック読出しデータ(1トラックにつき1個)
   */
  TrackReader* track_readers_ = nullptr;

  /**
   * @brief 読み取りカーソル位置でのOMNIモードのON/OFF
   *
   * Next関数等の呼び出し結果により内部的にステータスが変動する
   */
  bool omni_ = false;

  /**
   * @brief 読み取りカーソル位置でのMONOモードのON/OFF
   *
   * Next関数等の呼び出し結果により内部的にステータスが変動する
   */
  bool mono_ = false;

  /**
   * @brief 読み取りカーソル位置でのテンポ
   *
   * Next関数等の呼び出し結果により内部的にステータスが変動する
   */
  std::uint32_t tempo_ = 0;

  /**
   * @brief 2Byte整数値を読み取る
   *
   * @param p  読み取る位置のポインタ(読み取り後の位置に移動する)
   *
   * @return 読み取った整数値
   */
  constexpr static std::uint16_t ReadUint16(const std::uint8_t*& p) {
    std::uint16_t result = static_cast<std::uint16_t>(*p++) << 8;
    return result | static_cast<std::uint16_t>(*p++);
  }

  /**
   * @brief 4Byte整数値を読み取る
   *
   * @param p 読み取る位置のポインタ(読み取り後の位置に移動する)
   *
   * @return 読み取った整数値
   */
  constexpr static std::uint32_t ReadUint32(const std::uint8_t*& p) {
    std::uint32_t result = static_cast<std::uint32_t>(*p++) << 8;
    result = (result | static_cast<std::uint32_t>(*p++)) << 8;
    result = (result | static_cast<std::uint32_t>(*p++)) << 8;
    return result | static_cast<std::uint32_t>(*p++);
  }

  /**
   * @brief 可変長整数値を読み取る
   * 
   * 決まったByte数を読み取る場合は呼び出し側で読み取り可能なデータ内に収まるかを事前に判断出来るが
   * 可変長整数の場合は読んでみないと分からないため、読み取り可能かのチェックも行う
   *
   * @param cursor 読み取る位置のポインタ(読み取り後の位置に移動する)
   * @param end 読み取り可能なデータの終端
   * @param destination 読み取ったデータがここに格納される
   *
   * @return 読み取り結果
   */
  constexpr static bool ReadVariableUint(const std::uint8_t*& cursor, const std::uint8_t* const end, std::uint32_t& destination) {
    std::uint32_t value = 0;
    for (std::size_t i = 0; i < 4; ++i) {
      // 読み取り終端に達したらデータがおかしい
      if (cursor + i >= end) {
        return false;
      }

      // 下位7bitのデータを取得
      value |= static_cast<std::uint32_t>(cursor[i]) & 0x7f;

      if ((cursor[i] & 0x80) == 0) {
        // 最上位bitが立っていなければ読み取りはそこまで

        // 読み取った分ポインタを移動する
        cursor += i + 1;

        // 読み取った値を設定
        destination = value;

        return true;
      } else {
        // 最上位bitが立っていれば次があるはずなのでその分空けておく
        value <<= 7;
      }
    }

    // 最長4Byteなのでそれ以上あったらデータがおかしい
    return false;
  }
};

}  // namespace keysequencer

#endif