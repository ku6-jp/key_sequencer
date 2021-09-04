#include "standard_midi_format.h"

#include <Windows.h>

namespace keysequencer {

StandardMidiFormat::~StandardMidiFormat() {
  if (data_ != nullptr) {
    delete[] data_;
  }
}

StandardMidiFormat::Result StandardMidiFormat::LoadFile(const TCHAR* const filepath) {
  // 現在読み込んでいるデータを破棄
  if (data_ != nullptr) {
    delete[] data_;
  }
  data_ = nullptr;
  length_ = 0;

  // ファイルを開く
  HANDLE handle = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    return Result::kFailureFileOpen;
  }

  // ファイルサイズを取得
  LARGE_INTEGER filesize;
  if (!GetFileSizeEx(handle, &filesize)) {
    CloseHandle(handle);
    return Result::kFailureFileOpen;  // サイズが取れないとかオープン失敗扱いでいい
  }

  // 4GB以上のファイルには非対応
  // 32bit版だともっと小さいサイズでもメモリ確保エラーになるけど
  if (filesize.QuadPart > 0x00000000ffffffff) {
    CloseHandle(handle);
    return Result::kFailureFileTooLarge;
  }

  // サイズを保存
  length_ = static_cast<std::size_t>(filesize.LowPart);

  // メモリを確保
  data_ = new std::uint8_t[length_];

  // ファイルを読み込み
  DWORD read;
  BOOL result = ReadFile(handle, data_, filesize.LowPart, &read, nullptr);
  CloseHandle(handle);
  if (!result || filesize.LowPart != read) {
    return Result::kFailureFileRead;
  }

  return Result::kSuccess;
}

}  // namespace keysequencer