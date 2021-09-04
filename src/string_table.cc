#include "string_table.h"

#include <cstddef>

namespace keysequencer {

StringTable::StringTable(HINSTANCE instance_handle) {
  // 1文字列を入れるためのバッファ
  int string_buffer_length = 128;
  TCHAR* string_buffer = new TCHAR[string_buffer_length];

  // 全文字列を入れるために必要なサイズを取得
  int buffer_length = 0;
  for (UINT i = RESOURCE_ID_START; i < RESOURCE_ID_START + RESOURCE_LENGTH; ++i) {
    for (;;) {
      int result = LoadString(instance_handle, i, string_buffer, string_buffer_length);

      // 読み込めないIDはスキップ
      if (result == 0) {
        strings_[i - RESOURCE_ID_START] = nullptr;  // ついでにnullptr初期化しちゃう
        break;
      }

      // バッファより読み込みサイズが小さければそれが文字列の長さ
      if (result < string_buffer_length - 1) {
        strings_[i - RESOURCE_ID_START] = string_buffer;  // 区別が付くようにnullptrでない適当な値を入れとく
        buffer_length += result + 1;
        break;
      }

      // バッファ一杯まで読み込まれていたらバッファが足りなかった可能性があるので拡張してリトライ
      delete[] string_buffer;
      string_buffer_length *= 2;
      string_buffer = new TCHAR[string_buffer_length];
    }
  }
  delete[] string_buffer;

  buffer_ = new TCHAR[buffer_length];

  int written = 0;
  for (UINT i = RESOURCE_ID_START; i < RESOURCE_ID_START + RESOURCE_LENGTH; ++i) {
    // さっき読めたリソースIDだけを対象とする
    if (strings_[i - RESOURCE_ID_START] != nullptr) {
      int result = LoadString(instance_handle, i, &buffer_[written], buffer_length - written);
      if (result == 0) {
        strings_[i - RESOURCE_ID_START] = nullptr;
      } else {
        strings_[i - RESOURCE_ID_START] = &buffer_[written];
        written += result + 1;
      }
    }
  }
}

StringTable::~StringTable() {
  delete[] buffer_;
  delete[] strings_;
}

}  // namespace keysequencer