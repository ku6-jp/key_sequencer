#ifndef KEYSEQUENCER_SCOPEEXITMUTEX_H_
#define KEYSEQUENCER_SCOPEEXITMUTEX_H_

#include <Windows.h>

namespace keysequencer {
/**
 * @brief �f�X�g���N�^�𗘗p����Mutex�̉�����s��
 */
class ScopeExitMutex {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  ScopeExitMutex() = delete;

  /**
   * @brief �R���X�g���N�^
   * 
   * @param handle ����Ώۂ�Mutex�n���h��
   */
  ScopeExitMutex(HANDLE handle) : handle_(handle) {
  }

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  ScopeExitMutex(const ScopeExitMutex&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  ScopeExitMutex(ScopeExitMutex&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  ScopeExitMutex& operator=(const ScopeExitMutex&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  ScopeExitMutex& operator=(ScopeExitMutex&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~ScopeExitMutex() {
    ReleaseMutex(handle_);
    CloseHandle(handle_);
  }

 private:
  /**
   * @brief �n���h��
   */
  HANDLE const handle_;
};
}  // namespace keysequencer

#endif
