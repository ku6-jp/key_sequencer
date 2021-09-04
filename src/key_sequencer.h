#ifndef KEYSEQUENCER_KEYSEQUENCER_H_
#define KEYSEQUENCER_KEYSEQUENCER_H_

#include "keyboard.h"
#include "main_window.h"
#include "midi_device_input_to_keyboard.h"
#include "sequencer_manager.h"
#include "setting.h"
#include "string_table.h"

namespace keysequencer {

/**
 * @brief �A�v���{��
 */
class KeySequencer {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  KeySequencer() = delete;

  /**
   * @brief �R���X�g���N�^
   * 
   * @param instance_handle �A�v���P�[�V�����̃C���X�^���X�n���h��
   */
  KeySequencer(HINSTANCE instance_handle);

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  KeySequencer(const KeySequencer&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  KeySequencer(KeySequencer&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  KeySequencer& operator=(const KeySequencer&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  KeySequencer& operator=(KeySequencer&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~KeySequencer() = default;

  /**
   * @brief ���s
   *
   * @return WinMain�p�߂�l
   */
  int Execute();

 private:
  /**
   * @brief ���\�[�X�̕�����e�[�u��
   */
  StringTable string_table_;

  /**
   * @brief �ݒ� 
   */
  Setting setting_;

  /**
   * @brief �L�[�{�[�h 
   */
  Keyboard keyboard_;

  /**
   * @brief MIDI�@��̓��͂��L�[�{�[�h���͂ɕϊ����鏈�� 
   */
  MidiDeviceInputToKeyboard midi_input_to_keyboard_;

  /**
   * @brief �������t���� 
   */
  SequencerManager sequencer_manager_;

  /**
   * @brief ���C���E�B���h�E 
   */
  MainWindow main_window_;
};

}  // namespace keysequencer

#endif