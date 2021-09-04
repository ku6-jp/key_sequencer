#ifndef KEYSEQUENCER_KEYBOARD_H_
#define KEYSEQUENCER_KEYBOARD_H_

#include <Windows.h>

#include <cstdint>

namespace keysequencer {

/**
 * @brief �L�[�{�[�h
 *
 * Virtual key���󂯎��AScan code�Ƃ��ďo�͂���
 * �t������Virtual key��Scan code�̃}�b�s���O�̊Ǘ�
 * �L�[�̉����󋵂̊Ǘ����s��
 *
 * ���z�L�[�R�[�h���󂯎��Win32 API��UINT���g�p���Ă��邪
 * ��`�ς݂̉��z�L�[�R�[�h��BYTE�͈͓̔��ł���
 * �����I�ɂ�BYTE�ɍi���������ȗ����o����̂ŁA�����ł�BYTE�ň������ƂƂ���
 */
class Keyboard {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  Keyboard();

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  Keyboard(const Keyboard&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  Keyboard(Keyboard&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  Keyboard& operator=(const Keyboard&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  Keyboard& operator=(Keyboard&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~Keyboard();

  /**
   * @brief �L�[������
   *
   * @param virtual_key_code ���z�L�[�R�[�h
   */
  bool Down(std::uint8_t virtual_key_code);

  /**
   * @brief �L�[�𗣂�
   *
   * @param virtual_key_code ���z�L�[�R�[�h
   */
  bool Up(std::uint8_t virtual_key_code);

  /**
   * @brief �L�[�{�[�h�̏�Ԃ����Z�b�g����
   *
   * �S�ẴL�[�������ꂽ��ԂƂȂ�
   */
  void Reset();

 private:
  /**
   * @brief ���z�L�[�R�[�h�ɑ΂��ăX�L�����R�[�h���擾���Ă��Ȃ����
   */
  constexpr static std::uint16_t kScanCodeInitialized = 0xffff;

  /**
   * @brief ���z�L�[�R�[�h�ɑ΂���X�L�����R�[�h�����݂��Ȃ����邢�̓T�|�[�g���Ȃ�
   *
   * ���̒l��MapVirtualKey���Ή�����X�L�����R�[�h������Ԃ����Ƃ��Ɠ����Ƃ���
   */
  constexpr static std::uint16_t kScanCodeNotSupported = 0x0000;

  /**
   * @brief ���z�L�[�R�[�h�ɑ΂���X�L�����R�[�h
   *
   * �z��̃C���f�b�N�X�����z�L�[�R�[�h�A�l���X�L�����R�[�h�ƂȂ�
   * ���擾���(kScanCodeInitialized)�ŏ����������
   *
   * �N�����ɑS���擾���Ă����ׂ͑S���C�ɂ���K�v�������͂�����
   * �ꉞOS�ɖ₢���킹�鏈���ł͂���̂Ń}�i�[�I�ɕK�v�ɂȂ����������擾���L���b�V������
   */
  std::uint16_t virtual_key_code_to_scan_code[256];

  /**
   * @brief �L�[�̉�����Ԃ�ێ�����
   */
  bool downed_[256];

  /**
   * @brief SendInput�̈����Ɏw�肷�邽�߂�INPUT�\����
   */
  INPUT input_;

  /**
   * @brief ���z�L�[�R�[�h����X�L�����R�[�h���擾
   *
   * @param virtual_key_code ���z�L�[�R�[�h
   *
   * @return �X�L�����R�[�h
   */
  std::uint16_t VirtualKeyCodeToScanCode(std::uint8_t virtual_key_code);
};

}  // namespace keysequencer

#endif