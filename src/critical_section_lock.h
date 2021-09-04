#ifndef KEYSEQUENCER_CRITICALSECTIONLOCK_H_
#define KEYSEQUENCER_CRITICALSECTIONLOCK_H_

#include <Windows.h>

namespace keysequencer {

/**
 * @brief �f�X�g���N�^�𗘗p���ăX�R�[�v�������b�N����
 */
class CriticalSectionLock {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  CriticalSectionLock() = delete;

  /**
   * @brief �R���X�g���N�^
   *
   * @param criticalSection ���p����CRITICAL_SECTION
   */
  CriticalSectionLock(CRITICAL_SECTION* criticalSection) : criticalSection_(criticalSection) {
    EnterCriticalSection(criticalSection_);
  }

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  CriticalSectionLock(const CriticalSectionLock&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  CriticalSectionLock(CriticalSectionLock&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  CriticalSectionLock& operator=(const CriticalSectionLock&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  CriticalSectionLock& operator=(CriticalSectionLock&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~CriticalSectionLock() {
    LeaveCriticalSection(criticalSection_);
  }

 private:
  /**
   * @brief ���p����CRITICAL_SECTION
   */
  CRITICAL_SECTION* criticalSection_;
};

}  // namespace keysequencer

#endif