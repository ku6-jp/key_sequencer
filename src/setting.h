#ifndef KEYSEQUENCER_SETTING_H_
#define KEYSEQUENCER_SETTING_H_

#include <Windows.h>
#include <tchar.h>

#include <cstddef>
#include <cstdint>

namespace keysequencer {

/**
 * @brief アプリケーションの設定値管理
 */
class Setting {
 public:
  /**
   * @brief キーバインド情報
   * 
   * 2バイトのデータで以下の意味を持つ
   * 上位1バイト: 修飾キー(ModifierKeyのOR)
   * 下位1バイト: 仮想キーコード(VK_Aなど)
   */
  using KeyBind = std::uint16_t;

  /**
   * @brief キーバインドでShiftキーを表すビット
   */
  constexpr static KeyBind kShiftKeyBind = 0x0100;

  /**
   * @brief キーバインドでCtrlキーを表すビット
   */
  constexpr static KeyBind kControlKeyBind = 0x0200;

  /**
   * @brief キーバインドでAltキーを表すビット
   */
  constexpr static KeyBind kAlternateKeyBind = 0x0400;

  /**
   * @brief アプリケーションが扱うノート番号の最小
   */
#ifdef KEY_SEQUENCER_FULL_NOTE
  constexpr static std::uint8_t kMinimumNoteNumber = 0x00;
#else
  constexpr static std::uint8_t kMinimumNoteNumber = 0x30;
#endif

  /**
   * @brief アプリケーションが扱うノート番号の最大
   */
#ifdef KEY_SEQUENCER_FULL_NOTE
  constexpr static std::uint8_t kMaximumNoteNumber = 0x7F;
#else
  constexpr static std::uint8_t kMaximumNoteNumber = 0x54;
#endif

  /**
   * @brief キーバインドデータ内のどこに何が入っているかの情報
   */
  enum KeyBindIndex : std::size_t {
    /**
     * @brief 扱うノート番号の最小値(ここから扱うノート分並ぶ)
     */
    kNoteNumber = 0,

    /**
     * @brief 再生/停止のホットキー
     */
    kPlayHotkey = kNoteNumber + kMaximumNoteNumber - kMinimumNoteNumber + 1,

    /**
     * @brief MIDI入力開始のホットキー
     */
    kMidiInputHotkey,

    /**
     * @brief 全停止のホットキー 
     */
    kAllStopHotkey,

    /**
     * @brief キーバインド総数
     */
    kLength,
  };

  /**
   * @brief ノートの表記方式
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  enum class NoteLabelStyle : int {
    /**
     * @brief 国際式(英語での呼称が不明。英語表記と一緒らしいのでとりあえずEnglish) 
     */
    kEnglish = 0,

    /**
     * @brief ヤマハ式
     */
    kYamaha = 1,
  };

  /**
   * @brief デフォルトコンストラクタ
   */
  Setting();

  /**
   * @brief コピーコンストラクタ
   */
  Setting(const Setting&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  Setting(Setting&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  Setting& operator=(const Setting&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  Setting& operator=(Setting&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~Setting() = default;

  /**
   * @brief .iniファイルからデータを読み出す
   * 
   * 実行ファイルパスの拡張子を.iniに変更したファイルパスを対象とする
   * .iniファイルが存在しない場合は新たに作成する
   * また、以降の設定読み書き操作ではこの.iniファイルを操作する
   *
   * @return 成功した場合trueを返す
   */
  bool LoadIniFile();

  /**
   * @brief キーバインドを設定
   *
   * @param index 対象のキーバインドの位置
   * @param key_bind キーバインド
   */
  void SetKeyBind(std::size_t index, KeyBind key_bind);

  /**
   * @brief キーバインドを取得
   * 
   * @param index 対象のキーバインドの位置
   * 
   * @return キーバインド
   */
  inline KeyBind GetKeyBind(std::size_t index) const {
    return key_binds_[index];
  }

  /**
   * @brief ノート表記方式を設定
   * 
   * @param note_label_style ノート表記方式
   */
  void SetNoteLabelStyle(NoteLabelStyle note_label_style);

  /**
   * @brief ノート表記方式を取得
   * 
   * @return ノート表記方式
   */
  inline NoteLabelStyle GetNoteLabelStyle() const {
    return note_label_style_;
  }

  /**
   * @brief ノート番号に対するキー割り当てを取得
   *
   * @param note_number ノート番号
   *
   * @return キー割り当て情報
   */
  inline KeyBind GetKeyBindByNoteNumber(std::uint8_t note_number) const {
    if (note_number < kMinimumNoteNumber || kMaximumNoteNumber < note_number)
      return 0;

    return key_binds_[KeyBindIndex::kNoteNumber + (note_number - kMinimumNoteNumber)];
  }

  /**
   * @brief 再生/停止のホットキー割り当てを取得
   *
   * @return キー割り当て情報
   */
  inline KeyBind GetPlayHotkeyBind() const {
    return key_binds_[KeyBindIndex::kPlayHotkey];
  }

  /**
   * @brief MIDI入力受付のホットキー割り当てを取得
   *
   * @return キー割り当て情報
   */
  inline KeyBind GetMidiInputHotkeyBind() const {
    return key_binds_[KeyBindIndex::kMidiInputHotkey];
  }

  /**
   * @brief 全停止のホットキー割り当てを取得
   *
   * @return キー割り当て情報
   */
  inline KeyBind GetAllStopHotkeyBind() const {
    return key_binds_[KeyBindIndex::kAllStopHotkey];
  }

  /**
   * @brief ノートOFFイベント時刻より何ミリ秒早くキーを離すかの設定を取得する
   * 
   * 現状Setは存在しない
   * 
   * @return ミリ秒
   */
  inline std::uint32_t GetEarlyKeyReleaseMillisecond() const {
    return early_key_release_millisecond_;
  }

  /**
   * @brief 修飾キーをノートONから何ミリ秒離してアップダウンするかの設定を取得する
   * 
   * 現状Setは存在しない
   * 
   * @return ミリ秒
   */
  inline std::uint32_t GetModifierKeyDelayMillisecond() const {
    return modifier_key_delay_millisecond_;
  }

  /**
   * @brief 同一修飾キー押下がこの時間以内なら一つにまとめる
   * 
   * 現状Setは存在しない
   * 
   * @return ミリ秒
   */
  inline std::uint32_t GetModifierKeyDownJoinMillisecond() const {
    return modifier_key_down_join_millisecond_;
  }

 private:
  /**
   * @brief .iniファイル セクション名: キーバインド
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  constexpr static const TCHAR* kKeyBindsIniSection = _T("KeyBinds");

  /**
   * @brief .iniファイル セクション名: オプション(設定が無くてもよいもの)
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  constexpr static const TCHAR* kOptionalIniSection = _T("Optional");

  /**
   * @brief .iniファイル キー文字列: キーバインド
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  constexpr static const TCHAR* kKeyBindIniKeys[KeyBindIndex::kLength] = {
#ifdef KEY_SEQUENCER_FULL_NOTE
      _T("C-1"),
      _T("Cs-1"),
      _T("D-1"),
      _T("Ds-1"),
      _T("E-1"),
      _T("F-1"),
      _T("Fs-1"),
      _T("G-1"),
      _T("Gs-1"),
      _T("A-1"),
      _T("As-1"),
      _T("B-1"),
      _T("C0"),
      _T("Cs0"),
      _T("D0"),
      _T("Ds0"),
      _T("E0"),
      _T("F0"),
      _T("Fs0"),
      _T("G0"),
      _T("Gs0"),
      _T("A0"),
      _T("As0"),
      _T("B0"),
      _T("C1"),
      _T("Cs1"),
      _T("D1"),
      _T("Ds1"),
      _T("E1"),
      _T("F1"),
      _T("Fs1"),
      _T("G1"),
      _T("Gs1"),
      _T("A1"),
      _T("As1"),
      _T("B1"),
      _T("C2"),
      _T("Cs2"),
      _T("D2"),
      _T("Ds2"),
      _T("E2"),
      _T("F2"),
      _T("Fs2"),
      _T("G2"),
      _T("Gs2"),
      _T("A2"),
      _T("As2"),
      _T("B2"),
#endif
      _T("C3"),
      _T("Cs3"),
      _T("D3"),
      _T("Ds3"),
      _T("E3"),
      _T("F3"),
      _T("Fs3"),
      _T("G3"),
      _T("Gs3"),
      _T("A3"),
      _T("As3"),
      _T("B3"),
      _T("C4"),
      _T("Cs4"),
      _T("D4"),
      _T("Ds4"),
      _T("E4"),
      _T("F4"),
      _T("Fs4"),
      _T("G4"),
      _T("Gs4"),
      _T("A4"),
      _T("As4"),
      _T("B4"),
      _T("C5"),
      _T("Cs5"),
      _T("D5"),
      _T("Ds5"),
      _T("E5"),
      _T("F5"),
      _T("Fs5"),
      _T("G5"),
      _T("Gs5"),
      _T("A5"),
      _T("As5"),
      _T("B5"),
      _T("C6"),
#ifdef KEY_SEQUENCER_FULL_NOTE
      _T("Cs6"),
      _T("D6"),
      _T("Ds6"),
      _T("E6"),
      _T("F6"),
      _T("Fs6"),
      _T("G6"),
      _T("Gs6"),
      _T("A6"),
      _T("As6"),
      _T("B6"),
      _T("C7"),
      _T("Cs7"),
      _T("D7"),
      _T("Ds7"),
      _T("E7"),
      _T("F7"),
      _T("Fs7"),
      _T("G7"),
      _T("Gs7"),
      _T("A7"),
      _T("As7"),
      _T("B7"),
      _T("C8"),
      _T("Cs8"),
      _T("D8"),
      _T("Ds8"),
      _T("E8"),
      _T("F8"),
      _T("Fs8"),
      _T("G8"),
      _T("Gs8"),
      _T("A8"),
      _T("As8"),
      _T("B8"),
      _T("C9"),
      _T("Cs9"),
      _T("D9"),
      _T("Ds9"),
      _T("E9"),
      _T("F9"),
      _T("Fs9"),
      _T("G9"),
#endif
      _T("HotkeyPlay"),
      _T("HotkeyMidiInput"),
      _T("HotkeyAllStop"),
  };

  /**
   * @brief .iniファイル キー文字列: ノート表記方式
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  constexpr static const TCHAR* kNoteLabelStyleIniKey = _T("NoteLabelStyle");

  /**
   * @brief .iniファイル キー文字列: ノートOFFイベント時刻より何ミリ秒早くキーを離すか
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  constexpr static const TCHAR* kEarlyKeyReleaseMillisecondIniKey = _T("EarlyKeyReleaseMilliseconds");

  /**
   * @brief .iniファイル キー文字列: 修飾キーをノートONから何ミリ秒離してアップダウンするか
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  constexpr static const TCHAR* kModifierKeyDelayMillisecondIniKey = _T("ModifierKeyDelayMilliseconds");

  /**
   * @brief .iniファイル キー文字列: 同一修飾キー押下がこの時間以内なら一つにまとめる
   * 
   * 変更するとバージョン間で互換性が無くなってしまうので変更してはならない
   */
  constexpr static const TCHAR* kModifierKeyDownJoinMillisecondIniKey = _T("ModifierKeyDownJoinMilliseconds");

  /**
   * @brief デフォルトのキーバインド
   *
   * 起動時に.iniファイルが読み込めない場合にこれを使用する
   */
  constexpr static KeyBind kDefaultKeyBinds[KeyBindIndex::kLength] = {
#ifdef KEY_SEQUENCER_FULL_NOTE
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
#endif
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      '0',
      'Z',
      'X',
      'Q',
      'W',
      'E',
      'R',
      'T',
      'Y',
      'U',
      'I',
      'O',
      'P',
      'C',
      'V',
      'A',
      'S',
      'D',
      'F',
      'G',
      'H',
      'J',
      'K',
      'L',
      'B',
      'N',
      'M',
      VK_OEM_COMMA,  // ,
#ifdef KEY_SEQUENCER_FULL_NOTE
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
#endif
      VK_F11,
      VK_F10,
      VK_F12,
  };

  /**
   * @brief .iniファイルパス
   */
  TCHAR ini_file_path_[MAX_PATH];

  /**
   * @brief キーバインドデータ
   */
  KeyBind key_binds_[KeyBindIndex::kLength];

  /**
   * @brief ノート表記方式
   */
  NoteLabelStyle note_label_style_ = NoteLabelStyle::kEnglish;

  /**
   * @brief ノートOFFイベント時刻より何ミリ秒早くキーを離すかの指定
   *
   * キーを離した直後にキーを押す操作をした場合に、キーを押す操作の受け付けが引っかかる現象があるので
   * キーを離す操作をMIDIイベント時刻よりこの設定値分早く行う
   */
  std::uint32_t early_key_release_millisecond_ = 0;

  /**
   * @brief 修飾キーをノートONから何ミリ秒離してアップダウンするか
   * 
   * 修飾キーのアップダウンがキーダウンの前後に確実に拾われるようにある程度猶予を持つ
   * 対象アプリが高FPSだったり入力順に忠実に処理するほど小さい値でよい
   * あまり大きな値にすると前後のノートONに影響してくる
   */
  std::uint32_t modifier_key_delay_millisecond_ = 50;

  /**
   * @brief 同一修飾キー押下がこの時間以内なら一つにまとめる
   */
  std::uint32_t modifier_key_down_join_millisecond_ = 1000;
};

}  // namespace keysequencer

#endif