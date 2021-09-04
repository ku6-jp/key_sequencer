#ifndef KEYSEQUENCER_SEQUENCERMANAGER_H_
#define KEYSEQUENCER_SEQUENCERMANAGER_H_

#include <Windows.h>
#include <tchar.h>

#include "sequencer.h"

namespace keysequencer {

class Setting;
class Keyboard;

/**
 * @brief ���t�����̊Ǘ��N���X
 * 
 * ���t�����͂��̃N���X��ʂ��čs��
 */
class SequencerManager {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  SequencerManager() = delete;

  /**
   * @brief �R���X�g���N�^
   *
   * @param setting �ݒ�
   * @param keyboard �L�[�{�[�h
   */
  SequencerManager(const Setting* setting, Keyboard* keyboard);

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  SequencerManager(const SequencerManager&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  SequencerManager(SequencerManager&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  SequencerManager& operator=(const SequencerManager&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  SequencerManager& operator=(SequencerManager&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~SequencerManager();
  
  /**
   * @brief ���t���Ă��邩�m�F����
   * 
   * @return ���t���̏ꍇtrue��Ԃ�
   */
  inline bool IsPlaying() const {
    return playing_;
  }

  /**
   * @brief �o�͐���擾
   * 
   * Stop()���Ă��l���N���A������͂��Ȃ��̂ŁAPlay() �` Stop()�ԈȊO�ł͎g�p���Ȃ�����(IsPlaying()�Ŋm�F����)
   * 
   * @return �o�͐�
   */
  inline Sequencer::Output GetOutput() const {
    return sequencer_.GetOutput();
  }

  /**
   * @brief ���t�p�̃X���b�h���J�n����
   * 
   * ���t���̒ʒm�����C���E�B���h�E�ɑ΂��čs�����߂ɃE�B���h�E�n���h�����K�v
   * �E�B���h�E�n���h��������悤�ɂȂ�(���C���E�B���h�E�����������)�^�C�~���O�������x���̂ŃR���X�g���N�^�Ƃ͕ʂɊJ�n������݂���
   * �����J�n��ɍēx�Ăяo�����ꍇ�͉������Ȃ�
   *
   * @param main_window_handle ���C���E�B���h�E�n���h��
   */
  void BeginThread(HWND main_window_handle);

  /**
   * @brief ���t�p�̃X���b�h���I������
   * 
   * �I�u�W�F�N�g�̔j���^�C�~���O�̑O�ɏI�����s���������߃f�X�g���N�^�Ƃ͕ʂɏI��������݂���
   * �X���b�h�ɑ΂��ďI���v�����s������ŏI���𖳌��ɑ҂�
   */
  void ShutdownThread();

  /**
   * @brief �t�@�C��(SMF)��ǂݍ���
   *
   * @param filepath �t�@�C���p�X
   *
   * @return �ǂݍ��݌���
   */
  Sequencer::LoadFileResult LoadFile(const TCHAR* filepath);

  /**
   * ���t���J�n����(���߂���)
   *
   * @param output �o�͐�
   *
   * @return �Đ��J�n����������ɍs�����ꍇ��true��Ԃ�
   */
  bool Play(Sequencer::Output const output);

  /**
   * ���t���~����
   */
  void Stop();

 private:
  /**
   * @brief �X���b�h�I���t���O
   *
   * �X���b�h���I������ꍇ��true�ɂ���
   */
  bool thread_close_ = false;

  /**
   * @brief ���t���t���O
   */
  bool playing_ = false;

  /**
   * @brief ���C���E�B���h�E�̃n���h��
   *
   * ���C���E�B���h�E�Ƀ��b�Z�[�W�𑗂�ۂɎg�p����
   */
  HWND main_window_handle_ = nullptr;

  /**
   * @brief ���t�X���b�h�̃n���h��
   */
  HANDLE thread_handle_ = nullptr;

  /**
   * @brief ���t�X���b�h����p�̃V�O�i���n���h��
   *
   * �Đ����ɃV�O�i����Ԃɂ��ăX���b�h�𓮂���
   * ��~���܂��̓E�F�C�g���ɔ�V�O�i����Ԃɂ��ăX���b�h���X���[�v������
   * 
   * ���t�̑ҋ@�����͖{���}���`���f�B�A�^�C�}�[���g�������̂���
   * �}���`���f�B�A�^�C�}�[�̓��삪�X���b�h����ƍ��킹�ɂ����̂Œ��߂đf���ɂ�����ŏ�������
   * 
   * �ꉞ������̃^�C�}�[���}���`���f�B�A�^�C�}�[�̐��x�ݒ�̉e���ɂ���炵���H
   */
  HANDLE thread_active_event_ = nullptr;

  /**
   * @brief �X���b�h�Ԕr������p�̃N���e�B�J���Z�N�V����
   */
  CRITICAL_SECTION critical_section_;

  /**
   * @brief ���t�����{�� 
   */
  Sequencer sequencer_;

  /**
   * @brief �X���b�h�N�����ɌĂяo�����֐�
   *
   * _beginthreadex�Ɏw�肷�邽�߂̊֐�
   * ���ۂ̏�����threadRun�Ɉڏ�����
   * 
   * @param object _beginthreadex�œn�������[�U�[�I�u�W�F�N�g(���̃N���X�̃C���X�^���X)
   */
  static unsigned __stdcall Thread(void* object);

  /**
   * @brief ���t�X���b�h�����{��
   *
   * ���t�X���b�h�̓��C���E�B���h�E�쐬���ɋN������A���t���Ă��Ȃ����ɂ͑ҋ@���Ă���
   */
  void RunThread();
};
}  // namespace keysequencer

#endif