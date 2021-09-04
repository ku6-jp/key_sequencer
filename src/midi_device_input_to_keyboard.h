#ifndef KEYSEQUENCER_MIDIINPUT_H_
#define KEYSEQUENCER_MIDIINPUT_H_

#include <Windows.h>

namespace keysequencer {

class Setting;
class Keyboard;

/**
 * @brief MIDI���͂��L�[�{�[�h���͂ɕϊ����鏈��
 */
class MidiDeviceInputToKeyboard {
 public:
  /**
    * @brief Open����
    */
  enum class OpenResult {
    /**
     * @brief ����I�� 
     */
    kSuccess,

    /**
     * @brief �s���ȃG���[
     */
    kFailureUnknown,

    /**
     * @brief �f�o�C�X���ڑ�����Ă��܂���G���[
     */
    kFailureDeviceNotFound,
  };

  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  MidiDeviceInputToKeyboard() = delete;

  /**
   * @brief �R���X�g���N�^
   * 
   * @param keyboard �L�[�{�[�h
   * @param setting �ݒ�
   */
  MidiDeviceInputToKeyboard(const Setting* setting, Keyboard* keyboard);

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  MidiDeviceInputToKeyboard(const MidiDeviceInputToKeyboard&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  MidiDeviceInputToKeyboard(MidiDeviceInputToKeyboard&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  MidiDeviceInputToKeyboard& operator=(const MidiDeviceInputToKeyboard&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  MidiDeviceInputToKeyboard& operator=(MidiDeviceInputToKeyboard&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~MidiDeviceInputToKeyboard();

  /**
   * @brief �ϊ����J�n���Ă��邩�m�F����
   * 
   * @return �ϊ����̏ꍇ��true��Ԃ�
   */
  inline bool IsOpened() {
    return midi_in_handle_ != nullptr;
  }

  /**
   * @brief �ϊ��J�n
   */
  OpenResult Open();

  /**
   * @brief �ϊ��I��
   */
  void Close();

 private:
  /**
   * @brief �ݒ�
   */
  const Setting* const setting_;

  /**
   * @brief �L�[�{�[�h
   */
  Keyboard* const keyboard_;

  /**
   * @brief �n���h��
   */
  HMIDIIN midi_in_handle_ = nullptr;

  /**
   * @brief midiInOpen����v�������MIDI���͂��󂯎�������̃R�[���o�b�N����
   */
  static void CALLBACK CallbackProcedure(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2);

  /**
   * @brief MIDI�L�[�{�[�h���͂�����
   * 
   * @param message ���b�Z�[�W
   * @param param1 ���b�Z�[�W�̃p�����[�^1(����1�����g���Ă���̂�)
   */
  void Procedure(UINT message, DWORD param1);
};

}  // namespace keysequencer

#endif