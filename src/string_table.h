#ifndef KEYSEQUENCER_STRINGTABLE_H_
#define KEYSEQUENCER_STRINGTABLE_H_

#include <Windows.h>
#include <tchar.h>

namespace keysequencer {

/**
 * @brief リソースの文字列テーブル取得処理
 */
class StringTable {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  StringTable() = delete;

  /**
   * @brief コンストラクタ
   */
  StringTable(HINSTANCE instance_handle);

  /**
   * @brief コピーコンストラクタ
   */
  StringTable(const StringTable&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  StringTable(StringTable&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  StringTable& operator=(const StringTable&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  StringTable& operator=(StringTable&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~StringTable();

  /**
   * @brief 指定IDの文字列を取得
   *
   * @param resource_id リソースID
   *
   * @return 文字列のバッファ
   */
  inline const TCHAR* get(int resource_id) const {
    if (resource_id < RESOURCE_ID_START || RESOURCE_ID_START + RESOURCE_LENGTH <= resource_id) {
      return nullptr;
    }

    if (strings_[resource_id - RESOURCE_ID_START] == nullptr) {
      return nullptr;
    } else {
      return strings_[resource_id - RESOURCE_ID_START];
    }
  }

 private:
  /**
   * @brief StringTable内のリソースの開始ID
   *
   * StringTable内のリソースはこのIDから連番となっている前提で処理する
   */
  constexpr static int RESOURCE_ID_START = 1001;

  /**
   * @brief StringTable内のリソースの個数
   *
   * 足りないとリソースが読めなくなるが余る分には構わない
   */
  constexpr static int RESOURCE_LENGTH = 192;

  /**
   * @brief リソースID毎の文字列の先頭ポインタ
   * 
   * セットされる文字列の実体はbuffer_に格納するので個別にメモリを解放する必要は無い
   */
  TCHAR** const strings_ = new TCHAR*[RESOURCE_LENGTH];

  /**
   * @brief 文字列読出し用のバッファ
   */
  TCHAR* buffer_ = nullptr;
};

}  // namespace keysequencer

#endif