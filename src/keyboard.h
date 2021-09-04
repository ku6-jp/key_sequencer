#ifndef KEYSEQUENCER_KEYBOARD_H_
#define KEYSEQUENCER_KEYBOARD_H_

#include <Windows.h>

#include <cstdint>

namespace keysequencer {

/**
 * @brief キーボード
 *
 * Virtual keyを受け取り、Scan codeとして出力する
 * 付随してVirtual keyとScan codeのマッピングの管理
 * キーの押下状況の管理を行う
 *
 * 仮想キーコードを受け取るWin32 APIはUINTを使用しているが
 * 定義済みの仮想キーコードがBYTEの範囲内であり
 * 処理的にもBYTEに絞った方が簡略化出来るので、ここではBYTEで扱うこととする
 */
class Keyboard {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  Keyboard();

  /**
   * @brief コピーコンストラクタ
   */
  Keyboard(const Keyboard&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  Keyboard(Keyboard&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  Keyboard& operator=(const Keyboard&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  Keyboard& operator=(Keyboard&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~Keyboard();

  /**
   * @brief キーを押す
   *
   * @param virtual_key_code 仮想キーコード
   */
  bool Down(std::uint8_t virtual_key_code);

  /**
   * @brief キーを離す
   *
   * @param virtual_key_code 仮想キーコード
   */
  bool Up(std::uint8_t virtual_key_code);

  /**
   * @brief キーボードの状態をリセットする
   *
   * 全てのキーが離された状態となる
   */
  void Reset();

 private:
  /**
   * @brief 仮想キーコードに対してスキャンコードを取得していない状態
   */
  constexpr static std::uint16_t kScanCodeInitialized = 0xffff;

  /**
   * @brief 仮想キーコードに対するスキャンコードが存在しないあるいはサポートしない
   *
   * この値はMapVirtualKeyが対応するスキャンコード無しを返したときと同じとする
   */
  constexpr static std::uint16_t kScanCodeNotSupported = 0x0000;

  /**
   * @brief 仮想キーコードに対するスキャンコード
   *
   * 配列のインデックスが仮想キーコード、値がスキャンコードとなる
   * 未取得状態(kScanCodeInitialized)で初期化される
   *
   * 起動時に全部取得しても負荷は全く気にする必要も無いはずだが
   * 一応OSに問い合わせる処理ではあるのでマナー的に必要になった物だけ取得しキャッシュする
   */
  std::uint16_t virtual_key_code_to_scan_code[256];

  /**
   * @brief キーの押下状態を保持する
   */
  bool downed_[256];

  /**
   * @brief SendInputの引数に指定するためのINPUT構造体
   */
  INPUT input_;

  /**
   * @brief 仮想キーコードからスキャンコードを取得
   *
   * @param virtual_key_code 仮想キーコード
   *
   * @return スキャンコード
   */
  std::uint16_t VirtualKeyCodeToScanCode(std::uint8_t virtual_key_code);
};

}  // namespace keysequencer

#endif