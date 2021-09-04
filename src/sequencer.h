#ifndef KEYSEQUENCER_SEQUENCER_H_
#define KEYSEQUENCER_SEQUENCER_H_

#include <cstddef>
#include <cstdint>

#include "midi_synthesizer.h"
#include "setting.h"
#include "standard_midi_format.h"
#include "standard_midi_format_event.h"
#include "standard_midi_format_reader.h"

namespace keysequencer {

class Keyboard;

/**
 * @brief 演奏処理本体
 *
 * 演奏処理には専用のスレッドを使うが、
 * 演奏とスレッド操作両方の処理とデータを同一クラス内で扱うと複雑なため
 * 演奏用の処理とデータを分離した
 * スレッド制御やスレッド間の排他制御はSequencerManager側で行う
 */
class Sequencer {
 public:
  /**
   * @brief 出力先
   */
  enum class Output {
    /**
     * @brief キーボード
     */
    kKeyboard,

    /**
     * @brief MIDI音声
     */
    kMidiSynthesizer,
  };

  /**
   * @brief ファイル読み込み結果
   */
  enum class LoadFileResult {
    /**
     * @brief 正常終了 
     */
    kSuccess,

    /**
     * @brief ファイルが開けない
     */
    kFailureFileOpen,

    /**
     * @brief 4GB以上のファイルは非対応
     */
    kFailureFileTooLarge,

    /**
     * @brief ファイル読み込み中に不明なエラー
     */
    kFailureFileRead,

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
  Sequencer() = delete;

  /**
   * @brief コンストラクタ
   * 
   * @param setting 設定
   * @param keyboard キーボード
   */
  Sequencer(const Setting* setting, Keyboard* keyboard);

  /**
   * @brief コピーコンストラクタ
   */
  Sequencer(const Sequencer&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  Sequencer(Sequencer&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  Sequencer& operator=(const Sequencer&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  Sequencer& operator=(Sequencer&&) = delete;

  /**
   * @brief デストラクタ 
   */
  ~Sequencer() = default;

  /**
   * @brief 出力先を取得
   * 
   * Close()しても値をクリアしたりはしないので、Open() ～ Close()間以外では使用しないこと
   * 
   * @return 出力先
   */
  inline Output GetOutput() const {
    return output_;
  }

  /**
   * @brief ファイルが読み込み済みか確認
   * 
   * @return ファイルがエラー無く読み込まれ、スキャンしてチェックも済み使用出来る状態である場合にtrueを返す
   */
  inline bool IsFileLoaded() const {
    return file_loaded_;
  }

  /**
   * @brief ファイルを読み込む
   * 
   * @param filepath ファイル名
   * 
   * @return 読み込み結果
   */
  LoadFileResult LoadFile(const TCHAR* const filepath);

  /**
   * @brief 演奏の開始準備をする
   * 
   * 演奏開始位置を先頭に設定する
   * 
   * @param output 演奏の出力先
   * 
   * @return 開始に成功した場合にtrueを返す
   */
  bool Open(Output output);

  /**
   * @brief 演奏の終了処理をする
   */
  void Close();

  /**
   * @brief 演奏処理
   *
   * @param sleep_millisecond 次の演奏処理まで待機する時間(ミリ秒)
   * 
   * @return 演奏処理が継続する場合(次の演奏処理がある場合)にtrueを返す
   *         (falseを返した場合終端まで処理したということ)
   */
  bool Run(DWORD* sleep_millisecond);

 private:
  // 修飾キーのデータを配列として持つ時のための定数
  enum {
    /**
     * @brief Shift
     */
    kShiftKey,

    /**
     * @brief Ctrl 
     */
    kControlKey,

    /**
     * @brief Alt
     */
    kAlternateKey,

    /**
     * @brief 修飾キーの総数
     */
    kModifierKeyLength
  };

  /**
   * @brief 修飾キーのキーバインド上の仮想キーコード
   */
  constexpr static Setting::KeyBind kModifierKeys[kModifierKeyLength] = {Setting::kShiftKeyBind, Setting::kControlKeyBind, Setting::kAlternateKeyBind};

  /**
   * @brief 修飾キーのWindows仮想キーコード 
   */
  constexpr static std::uint8_t kVirtualModifierKeyCodes[kModifierKeyLength] = {VK_SHIFT, VK_CONTROL, VK_MENU};

  /**
   * @brief キー操作イベントの動作時刻
   */
  struct KeyEventTime {
    /**
     * @brief 構造体が空であることを示すindexの値
     * 
     * 4GB以上のMIDIには対応しないためindexが32bit int maxまで到達することは無い
     */
    constexpr static std::uint32_t kNullIndex = 0xffffffff;

    /**
     * @brief キー操作イベントの元となるMIDIイベントのインデックス
     * 
     * 複数のKeyEventTimeが同一時刻(millisecond)だった場合に処理順を決定するために使用する
     */
    uint32_t index = kNullIndex;

    /**
     * @brief キー操作イベントを動作される時刻
     *
     * 演奏開始からの経過時間(ミリ秒)
     */
    uint32_t millisecond = 0;

    /**
     * @brief 構造体がnull(使用していない状態)であるか判定する
     * 
     * @return 構造体がnullの場合にtrueを返す
     */
    inline bool IsNull() const {
      return index == kNullIndex;
    }

    /**
     * @brief 値を一括でセットする
     *
     * @param idx indexに設定する値
     * @param ms millisecondに設定する値
     */
    inline void Set(std::uint32_t idx, std::uint32_t ms) {
      index = idx;
      millisecond = ms;
    }

    /**
     * @brief 構造体をnull(使用していない状態)にする
     */
    inline void SetNull() {
      index = kNullIndex;
    }

    /**
     * @brief 時刻が一致するか判定する
     *
     * @param destination 比較相手
     * 
     * @return indexとmillisecondが両方一致したら一致したとしてtrueを返す
     *         どちらか、もしくは両方がnullの場合は一致していないと判定する
     */
    inline bool Equals(const KeyEventTime& destination) const {
      return !IsNull() && !destination.IsNull() && index == destination.index && millisecond == destination.millisecond;
    }

    /**
     * @brief 自身が相手より前か判定する
     *
     * @param destination 比較相手
     * 
     * @return millisecond, indexの順で比較し自身の方が前ならtrueを返す
     *         nullは無限大扱いとする
     *         両方がnullの場合はfalseを返す
     */
    inline bool Before(const KeyEventTime& destination) const {
      // nullは無限最大扱いなので自身がnullならば自身が前となることはない
      if (IsNull()) {
        return false;
      }

      // nullは無限最大扱いなので自身がnot nullで相手がnullならば自身が前
      if (destination.IsNull()) {
        return true;
      }

      // 両方not nullの場合はmillisecond -> indexの順で小さい方が前
      return millisecond < destination.millisecond || (millisecond == destination.millisecond && index < destination.index);
    }
  };

  /**
   * @brief 設定
   */
  const Setting* const setting_;

  /**
   * @brief キーボード 
   */
  Keyboard* const keyboard_;

  /**
   * @brief 楽譜データ(SMF)読み込み済みフラグ
   * 
   * ファイルがエラー無く読み込まれ、スキャンしてチェックも済み使用出来る状態である場合にtrueとなる
   */
  bool file_loaded_ = false;

  /**
   * @brief 出力先
   */
  Output output_ = Output::kKeyboard;

  /**
   * @brief MIDI音声出力処理
   */
  MidiSynthesizer midi_synthesizer_;

  /**
   * 楽譜データ(SMF)
   */
  StandardMidiFormat standard_midi_format_;

  /**
   * @brief MIDIのノートONイベントを読み取るためのReader
   * 
   * ノートON同士の並び方により変動する処理が存在するため専用のReaderが必要
   */
  StandardMidiFormatReader note_on_reader_;

  /**
   * @brief MIDIのノートOFFイベントを読み取るためのReader
   *
   * ノートONイベントのReaderが分離したため残りがノートOFFイベントのみとなった
   */
  StandardMidiFormatReader note_off_reader_;

  /**
   * @brief 次に処理するSMFイベント
   * 
   * MIDI音声出力の場合でのみ使用(キーボード出力の場合は別データで管理するので)
   */
  StandardMidiFormatEvent event_;

  /**
   * @brief ノートON用Readerが読み取ったイベント数 
   */
  std::uint32_t note_on_reader_count_ = 0;

  /**
   * @brief ノートOFF用Readerが読み取ったイベント数 
   */
  std::uint32_t note_off_reader_count_ = 0;

  /**
   * @brief 演奏開始時刻
   * 
   * Windows起動からの経過時間(timeGetTime()で取れる値)
   */
  std::uint32_t play_start_millisecond_ = 0;

  /**
   * @brief 次のノートON時刻
   */
  KeyEventTime note_on_time_;

  /**
   * @brief 次のノートOFF時刻
   */
  KeyEventTime note_off_time_;

  /**
   * @brief 次にONするノート
   * 
   * note_on_time_がnot nullの場合のみ有効
   */
  std::uint8_t on_note_number_ = 0;

  /**
   * @brief 次にOFFするノート
   * 
   * note_off_time_がnot nullの場合のみ有効
   */
  std::uint8_t off_note_number_ = 0;

  /**
   * @brief 修飾キーダウン時刻 
   */
  KeyEventTime modifier_key_down_times_[kModifierKeyLength];

  /**
   * @brief 修飾キーアップ時刻 
   */
  KeyEventTime modifier_key_up_times_[kModifierKeyLength];

  /**
   * @brief ノート毎の最後にONにされたMIDIイベント時刻
   * 
   * ノートOFFをMIDIイベント時刻より早めた場合にONより前にならないよう調整するために記録しておく
   */
  std::uint32_t last_note_on_milliseconds_[Setting::kMaximumNoteNumber - Setting::kMinimumNoteNumber];

  /**
   * @brief SMFのイベントがノートONとして扱う物か判定する
   * 
   * 以下の条件を全て満たす場合にノートONとして扱う
   * ・種別がMIDIイベント
   * ・チャンネル0
   * ・MIDIイベント種別がノートONまたはポリフォニックキープレッシャー(ポリフォニックキープレッシャーは除外してもいいかも)
   * ・ベロシティが0より大きい(ベロシティ0はノートOFFとして扱うため)
   * ・対応するノート番号の範囲
   * 
   * @param event 判定対象のイベント
   *
   * @return 判定対象のイベントをノートONとして扱う場合にtrueを返す
   */
  constexpr static bool StandardMidiFormatEventIsValidNoteOn(const StandardMidiFormatEvent& event) {
    return event.type == StandardMidiFormatEvent::Type::kMidi &&
           event.channel == 0 &&
           (event.midi_type == StandardMidiFormatEvent::MidiType::kNoteOn || event.midi_type == StandardMidiFormatEvent::MidiType::kPolyphonicKeyPressure) &&
           event.velocity > 0 &&
           Setting::kMinimumNoteNumber <= event.note_number && event.note_number <= Setting::kMaximumNoteNumber;
  }

  /**
   * @brief SMFのイベントがノートOFFとして扱う物か判定する
   * 
   * 以下の条件を全て満たす場合にノートOFFとして扱う
   * ・種別がMIDIイベント
   * ・チャンネル0
   * ・MIDIイベント種別がノートOFFまたはベロシティが0のノートON
   * ・対応するノート番号の範囲
   * 
   * @param event 判定対象のイベント
   *
   * @return 判定対象のイベントをノートOFFとして扱う場合にtrueを返す
   */
  constexpr static bool StandardMidiFormatEventIsValidNoteOff(const StandardMidiFormatEvent& event) {
    return event.type == StandardMidiFormatEvent::Type::kMidi &&
           event.channel == 0 &&
           (event.midi_type == StandardMidiFormatEvent::MidiType::kNoteOff || (event.midi_type == StandardMidiFormatEvent::MidiType::kNoteOn && event.velocity == 0)) &&
           Setting::kMinimumNoteNumber <= event.note_number && event.note_number <= Setting::kMaximumNoteNumber;
  }

  /**
   * @brief MIDI演奏出力処理を実行
   *
   * @param sleep_millisecond 次の演奏処理まで待機する時間(ミリ秒)
   * 
   * @return 処理が継続する場合(次の処理がある場合)にtrueを返す
   *         (falseを返した場合終端まで処理したということ)
   */
  bool RunMidiSynthesizerOutput(DWORD* sleep_millisecond);

  /**
   * @brief キーボード出力処理を実行
   *
   * @param sleep_millisecond 次の演奏処理まで待機する時間(ミリ秒)
   * 
   * @return 処理が継続する場合(次の処理がある場合)にtrueを返す
   *         (falseを返した場合終端まで処理したということ)
   */
  bool RunKeyboardOutput(DWORD* sleep_millisecond);

  /**
   * @brief キーボード出力の場合に次のノートONを読み出して予定する
   */
  void ScheduleKeyboardNoteOn();

  /**
   * @brief キーボード出力の場合に次のノートOFFを読み出して予定する
   */
  void ScheduleKeyboardNoteOff();
};

}  // namespace keysequencer

#endif