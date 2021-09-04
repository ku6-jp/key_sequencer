#include "sequencer_manager.h"

#include <process.h>

#include "constants.h"
#include "critical_section_lock.h"

namespace keysequencer {

SequencerManager::SequencerManager(const Setting* setting, Keyboard* keyboard) : sequencer_(setting, keyboard) {
  // Critical section������
  InitializeCriticalSection(&critical_section_);

  // �X���b�h����p�̃C�x���g���쐬
  thread_active_event_ = CreateEvent(nullptr, FALSE, FALSE, _T("Active"));
}

SequencerManager::~SequencerManager() {
  // �f�X�g���N�^�ɗ���O�Ɏ蓮�Ŏ~�߂Ă���͂������ŏI��i�Ƃ���
  ShutdownThread();

  // �X���b�h����p�̃C�x���g���I��
  CloseHandle(thread_active_event_);

  // Critical section���폜
  DeleteCriticalSection(&critical_section_);
}

void SequencerManager::BeginThread(HWND main_window_handle) {
  // ���ɊJ�n���Ă����牽�����Ȃ�
  if (thread_handle_ != nullptr) {
    return;
  }

  // �N���X�̏������ŃZ�b�g���Ă��邪�ꉞ�����I�ɃX���b�h�I���t���O���I�t��
  thread_close_ = false;

  // �ʒm��ƂȂ郁�C���E�B���h�E�̃n���h����ۑ�
  main_window_handle_ = main_window_handle;

  // �X���b�h�J�n
  thread_handle_ = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, Thread, this, 0, nullptr));
}

void SequencerManager::ShutdownThread() {
  if (thread_handle_ == nullptr) {
    return;
  }

  {
    CriticalSectionLock lock(&critical_section_);

    // �X���b�h�I�����Z�b�g
    thread_close_ = true;

    // �X���b�h�𓮂���
    SetEvent(thread_active_event_);
  }

  // �X���b�h�I����҂�(�I�������Ȃ̂Ŗ����ɑ҂�)
  WaitForSingleObject(thread_handle_, INFINITE);

  // �X���b�h�n���h�������
  CloseHandle(thread_handle_);

  thread_handle_ = nullptr;
}

Sequencer::LoadFileResult SequencerManager::LoadFile(const TCHAR* const filepath) {
  CriticalSectionLock lock(&critical_section_);

  return sequencer_.LoadFile(filepath);
}

bool SequencerManager::Play(Sequencer::Output const output) {
  CriticalSectionLock lock(&critical_section_);

  // �t�@�C�����ǂݍ��܂�Ă��Ȃ���΍Đ����ł��Ȃ�
  if (!sequencer_.IsFileLoaded()) {
    return false;
  }

  // �Đ��J�n
  if (!sequencer_.Open(output)) {
    return false;
  }
  playing_ = true;
  SetEvent(thread_active_event_);

  return true;
}

void SequencerManager::Stop() {
  CriticalSectionLock lock(&critical_section_);

  // �Đ��t���O���~�ɐݒ�
  // ���t�X���b�h�����삵�Ă�����(�C�x���g���A�N�e�B�u�Ȃ�)����ɂ���~����
  // ���t�X���b�h�����삵�Ă��Ȃ�������(�C�x���g����A�N�e�B�u�Ȃ�)���X�~�܂��Ă���̂Ŗ�薳��
  // ���ɃA�N�e�B�u�ɂȂ鎞���Đ��J�n���̂��߂�����ŏ��������̂Ŗ�薳��
  playing_ = false;
}

unsigned __stdcall SequencerManager::Thread(void* object) {
  // �C���X�^���X�̏����ɈϏ�����
  reinterpret_cast<SequencerManager*>(object)->RunThread();
  return 0;
}

void SequencerManager::RunThread() {
  // �X���[�v����(�~���b)
  DWORD sleep_millisecond = INFINITE;

  for (;;) {
    // �X���[�v���Ԃ�1�~���b�ȏ�ݒ肳��Ă�����X���[�v����������
    if (sleep_millisecond != 0) {
      WaitForSingleObject(thread_active_event_, sleep_millisecond);
      sleep_millisecond = 0;
    }

    // �X���b�h�I�����w�肳��Ă�����I��
    if (thread_close_) {
      return;
    }

    {
      CriticalSectionLock lock(&critical_section_);

      if (!playing_) {
        // ���t���������
        sequencer_.Close();

        // �X���b�h��~
        sleep_millisecond = INFINITE;

        // ���C���E�B���h�E�ɕ\���X�V���b�Z�[�W�𑗐M
        PostMessage(main_window_handle_, WM_APP_REFRESH_STATUS, 0, 0);
        continue;
      }

      // ���t�������s���A�I�[�܂ŒB�������~���Z�b�g
      if (!sequencer_.Run(&sleep_millisecond)) {
        playing_ = false;
        sleep_millisecond = 0;
      }
    }
  }
}

}  // namespace keysequencer