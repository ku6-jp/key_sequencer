#include "keyboard.h"

#include <cstddef>

namespace keysequencer {

Keyboard::Keyboard() {
  // 仮想キーコードからスキャンコードへの変換データを初期状態で初期化
  // キーの押下状態を全て押していない状態で初期化
  for (std::size_t i = 0; i < 256; ++i) {
    virtual_key_code_to_scan_code[i] = kScanCodeInitialized;
    downed_[i] = false;
  }

  // INPUT構造体の初期化
  ZeroMemory(&input_, sizeof(INPUT));
  input_.type = INPUT_KEYBOARD;
}

Keyboard::~Keyboard() {
  Reset();
}

bool Keyboard::Down(std::uint8_t const virtual_key_code) {
  // 仮想キーコードに対するスキャンコードを取得
  std::uint16_t scan_code = VirtualKeyCodeToScanCode(virtual_key_code);
  if (scan_code == 0) {
    return false;
  }

  // wScanには拡張コードを抜いたスキャンコードを設定する
  input_.ki.wScan = scan_code & 0x00ff;

  // スキャンコードを指定するフラグ(押すフラグは存在せず、KEYEVENTF_KEYUPが指定されていなければ押すを指定した扱いとなる)
  input_.ki.dwFlags = KEYEVENTF_SCANCODE;

  // 拡張スキャンコード(0xe0)が必要ならKEYEVENTF_EXTENDEDKEYで指定する
  if ((scan_code & 0xff00) == 0xe000) {
    input_.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
  }

  // キーダウン
  SendInput(1, &input_, sizeof(INPUT));

  // キーダウンを記録
  downed_[virtual_key_code] = true;

  return true;
}

bool Keyboard::Up(std::uint8_t const virtual_key_code) {
  // 仮想キーコードに対するスキャンコードを取得
  std::uint16_t scan_code = VirtualKeyCodeToScanCode(virtual_key_code);
  if (scan_code == 0) {
    return false;
  }

  // wScanには拡張コードを抜いたスキャンコードを設定する
  input_.ki.wScan = scan_code & 0x00ff;

  // スキャンコードを指定するフラグとキーを離すフラグ
  input_.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

  // 拡張スキャンコード(0xe0)が必要ならKEYEVENTF_EXTENDEDKEYで指定する
  if ((scan_code & 0xff00) == 0xe000) {
    input_.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
  }

  // キーアップを送信
  SendInput(1, &input_, sizeof(INPUT));

  // キーアップを記録
  downed_[virtual_key_code] = false;

  return true;
}

void Keyboard::Reset() {
  for (std::size_t i = 0; i < 256; ++i) {
    if (downed_[i]) {
      Up(static_cast<std::uint8_t>(i));
    }
  }
}

std::uint16_t Keyboard::VirtualKeyCodeToScanCode(std::uint8_t const virtual_key_code) {
  // 仮想キーコード毎に初回参照時にスキャンコードを取得する
  if (virtual_key_code_to_scan_code[virtual_key_code] == kScanCodeInitialized) {
    // MAPVK_VK_TO_VSC_EX
    //
    // 仮想キーコードを指定して対応するスキャンコードを取得する
    // 指定した仮想キーコードが右と左の区別が無い物ならば左のスキャンコードを返す
    // 対応するスキャンコードが拡張スキャンコードだった場合、戻り値の上位Byteに0xe0または0xe1を返す
    // 対応するスキャンコードがない場合は0を返す
    //
    // VK_PAUSEだけ0xe1を返すのだが、これをSendInputで送信する方法が解らない(というか、無いように見える)
    // よって、0xe1からの拡張スキャンコードは非対応とする
    std::uint16_t scan_code = static_cast<std::uint16_t>(MapVirtualKey(virtual_key_code, MAPVK_VK_TO_VSC_EX));
    if ((scan_code & 0xff00) == 0 || (scan_code & 0xff00) == 0xe000) {
      // 取得したスキャンコードを記録する
      // MapVirtualKeyが対応するスキャンコード無しを返したときは0(kScanCodeNotSupported)となる
      virtual_key_code_to_scan_code[virtual_key_code] = scan_code;
    } else {
      // 拡張コード0xe0以外は対応しないこととする
      virtual_key_code_to_scan_code[virtual_key_code] = kScanCodeNotSupported;
    }
  }

  return virtual_key_code_to_scan_code[virtual_key_code];
}

}  // namespace keysequencer