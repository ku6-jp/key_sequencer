#ifndef KEYSEQUENCER_CRITICALSECTIONLOCK_H_
#define KEYSEQUENCER_CRITICALSECTIONLOCK_H_

#include <Windows.h>

namespace keysequencer {

/**
 * @brief デストラクタを利用してスコープ内をロックする
 */
class CriticalSectionLock {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  CriticalSectionLock() = delete;

  /**
   * @brief コンストラクタ
   *
   * @param criticalSection 利用するCRITICAL_SECTION
   */
  CriticalSectionLock(CRITICAL_SECTION* criticalSection) : criticalSection_(criticalSection) {
    EnterCriticalSection(criticalSection_);
  }

  /**
   * @brief コピーコンストラクタ
   */
  CriticalSectionLock(const CriticalSectionLock&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  CriticalSectionLock(CriticalSectionLock&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  CriticalSectionLock& operator=(const CriticalSectionLock&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  CriticalSectionLock& operator=(CriticalSectionLock&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~CriticalSectionLock() {
    LeaveCriticalSection(criticalSection_);
  }

 private:
  /**
   * @brief 利用するCRITICAL_SECTION
   */
  CRITICAL_SECTION* criticalSection_;
};

}  // namespace keysequencer

#endif