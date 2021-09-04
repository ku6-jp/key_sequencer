#include "keyboard.h"

#include <cstddef>

namespace keysequencer {

Keyboard::Keyboard() {
  // ���z�L�[�R�[�h����X�L�����R�[�h�ւ̕ϊ��f�[�^��������Ԃŏ�����
  // �L�[�̉�����Ԃ�S�ĉ����Ă��Ȃ���Ԃŏ�����
  for (std::size_t i = 0; i < 256; ++i) {
    virtual_key_code_to_scan_code[i] = kScanCodeInitialized;
    downed_[i] = false;
  }

  // INPUT�\���̂̏�����
  ZeroMemory(&input_, sizeof(INPUT));
  input_.type = INPUT_KEYBOARD;
}

Keyboard::~Keyboard() {
  Reset();
}

bool Keyboard::Down(std::uint8_t const virtual_key_code) {
  // ���z�L�[�R�[�h�ɑ΂���X�L�����R�[�h���擾
  std::uint16_t scan_code = VirtualKeyCodeToScanCode(virtual_key_code);
  if (scan_code == 0) {
    return false;
  }

  // wScan�ɂ͊g���R�[�h�𔲂����X�L�����R�[�h��ݒ肷��
  input_.ki.wScan = scan_code & 0x00ff;

  // �X�L�����R�[�h���w�肷��t���O(�����t���O�͑��݂����AKEYEVENTF_KEYUP���w�肳��Ă��Ȃ���Ή������w�肵�������ƂȂ�)
  input_.ki.dwFlags = KEYEVENTF_SCANCODE;

  // �g���X�L�����R�[�h(0xe0)���K�v�Ȃ�KEYEVENTF_EXTENDEDKEY�Ŏw�肷��
  if ((scan_code & 0xff00) == 0xe000) {
    input_.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
  }

  // �L�[�_�E��
  SendInput(1, &input_, sizeof(INPUT));

  // �L�[�_�E�����L�^
  downed_[virtual_key_code] = true;

  return true;
}

bool Keyboard::Up(std::uint8_t const virtual_key_code) {
  // ���z�L�[�R�[�h�ɑ΂���X�L�����R�[�h���擾
  std::uint16_t scan_code = VirtualKeyCodeToScanCode(virtual_key_code);
  if (scan_code == 0) {
    return false;
  }

  // wScan�ɂ͊g���R�[�h�𔲂����X�L�����R�[�h��ݒ肷��
  input_.ki.wScan = scan_code & 0x00ff;

  // �X�L�����R�[�h���w�肷��t���O�ƃL�[�𗣂��t���O
  input_.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

  // �g���X�L�����R�[�h(0xe0)���K�v�Ȃ�KEYEVENTF_EXTENDEDKEY�Ŏw�肷��
  if ((scan_code & 0xff00) == 0xe000) {
    input_.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
  }

  // �L�[�A�b�v�𑗐M
  SendInput(1, &input_, sizeof(INPUT));

  // �L�[�A�b�v���L�^
  downed_[virtual_key_code] = false;

  return true;
}

void Keyboard::Reset() {
  for (std::size_t i = 0; i < 256; ++i) {
    if (downed_[i]) {
      Up(static_cast<std::uint8_t>(i));
    }
  }
}

std::uint16_t Keyboard::VirtualKeyCodeToScanCode(std::uint8_t const virtual_key_code) {
  // ���z�L�[�R�[�h���ɏ���Q�Ǝ��ɃX�L�����R�[�h���擾����
  if (virtual_key_code_to_scan_code[virtual_key_code] == kScanCodeInitialized) {
    // MAPVK_VK_TO_VSC_EX
    //
    // ���z�L�[�R�[�h���w�肵�đΉ�����X�L�����R�[�h���擾����
    // �w�肵�����z�L�[�R�[�h���E�ƍ��̋�ʂ��������Ȃ�΍��̃X�L�����R�[�h��Ԃ�
    // �Ή�����X�L�����R�[�h���g���X�L�����R�[�h�������ꍇ�A�߂�l�̏��Byte��0xe0�܂���0xe1��Ԃ�
    // �Ή�����X�L�����R�[�h���Ȃ��ꍇ��0��Ԃ�
    //
    // VK_PAUSE����0xe1��Ԃ��̂����A�����SendInput�ő��M������@������Ȃ�(�Ƃ������A�����悤�Ɍ�����)
    // ����āA0xe1����̊g���X�L�����R�[�h�͔�Ή��Ƃ���
    std::uint16_t scan_code = static_cast<std::uint16_t>(MapVirtualKey(virtual_key_code, MAPVK_VK_TO_VSC_EX));
    if ((scan_code & 0xff00) == 0 || (scan_code & 0xff00) == 0xe000) {
      // �擾�����X�L�����R�[�h���L�^����
      // MapVirtualKey���Ή�����X�L�����R�[�h������Ԃ����Ƃ���0(kScanCodeNotSupported)�ƂȂ�
      virtual_key_code_to_scan_code[virtual_key_code] = scan_code;
    } else {
      // �g���R�[�h0xe0�ȊO�͑Ή����Ȃ����ƂƂ���
      virtual_key_code_to_scan_code[virtual_key_code] = kScanCodeNotSupported;
    }
  }

  return virtual_key_code_to_scan_code[virtual_key_code];
}

}  // namespace keysequencer