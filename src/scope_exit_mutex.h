#ifndef KEYSEQUENCER_SCOPEEXITMUTEX_H_
#define KEYSEQUENCER_SCOPEEXITMUTEX_H_

#include <Windows.h>

namespace keysequencer {
/**
 * @brief デストラクタを利用してMutexの解放を行う
 */
class ScopeExitMutex {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  ScopeExitMutex() = delete;

  /**
   * @brief コンストラクタ
   * 
   * @param handle 解放対象のMutexハンドル
   */
  ScopeExitMutex(HANDLE handle) : handle_(handle) {
  }

  /**
   * @brief コピーコンストラクタ
   */
  ScopeExitMutex(const ScopeExitMutex&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  ScopeExitMutex(ScopeExitMutex&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  ScopeExitMutex& operator=(const ScopeExitMutex&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  ScopeExitMutex& operator=(ScopeExitMutex&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~ScopeExitMutex() {
    ReleaseMutex(handle_);
    CloseHandle(handle_);
  }

 private:
  /**
   * @brief ハンドル
   */
  HANDLE const handle_;
};
}  // namespace keysequencer

#endif
