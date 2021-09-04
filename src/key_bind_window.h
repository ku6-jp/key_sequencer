#ifndef KEYSEQUENCER_KEYBINDWINDOW_H_
#define KEYSEQUENCER_KEYBINDWINDOW_H_

#include <Windows.h>
#include <tchar.h>

#include "constants.h"
#include "setting.h"

namespace keysequencer {

class StringTable;

/**
 * @brief �L�[�o�C���h�ݒ�E�B���h�E
 */
class KeyBindWindow {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  KeyBindWindow() = delete;

  /**
   * @brief �R���X�g���N�^
   *
   * @param instance_handle �A�v���P�[�V�����̃C���X�^���X�n���h��
   * @param parent_window_handle �e�E�B���h�E�̃n���h��
   * @param string_table ���\�[�X�̕�����e�[�u��
   * @param setting �ݒ�
   */
  KeyBindWindow(HINSTANCE instance_handle, HWND parent_window_handle, const StringTable* string_table, Setting* setting);

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  KeyBindWindow(const KeyBindWindow&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  KeyBindWindow(KeyBindWindow&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  KeyBindWindow& operator=(const KeyBindWindow&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  KeyBindWindow& operator=(KeyBindWindow&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~KeyBindWindow() = default;

  /**
   * @brief �E�B���h�E�̐�������
   * 
   * �E�B���h�E�����̐��ۂ𔻒肷�邽�߂ɃR���X�g���N�^�Ƃ͕ʂɂ����E�B���h�E��������
   * 1�x�������s����
   * ����2��ڈȍ~�̌Ăяo���������ꍇ�͉������Ȃ�
   *
   * @return ����ɏI�������ꍇ��true��Ԃ�
   */
  bool Create();

  /**
   * @brief �E�B���h�E�̃��b�Z�[�W����
   *
   * �E�B���h�E�����݂�����菈�����p������
   * �E�B���h�E������ꂽ���ɏ������Ԃ�
   */
  void Run();

 private:
  /**
   * @brief WM_COMMAND��wParam�Ńm�[�g�̕\�L�����ύX���W�I�{�^�������ʂ���l
   * ����1�o�C�g��Setting::NoteLabelStyle������
   */
  constexpr static WORD kNoteLabelStyleRadioControl = 0x0100;

  /**
   * @brief WM_COMMAND��wParam�ŃL�[�o�C���h�̕ύX�{�^�������ʂ���l
   * ����1�o�C�g�ɃL�[�o�C���h�̃C���f�b�N�X������
   */
  constexpr static WORD kChangeButtonControl = 0x0200;

  /**
   * @brief WM_COMMAND��wParam�ŃL�[�o�C���h�̕ύX�m��{�^�������ʂ���l
   * ����1�o�C�g�ɃL�[�o�C���h�̃C���f�b�N�X������
   */
  constexpr static WORD kCommitButtonControl = 0x0300;

  /**
   * @brief WM_COMMAND��wParam�ŃL�[�o�C���h�̕ύX�L�����Z���{�^�������ʂ���l
   * ����1�o�C�g�ɃL�[�o�C���h�̃C���f�b�N�X������
   */
  constexpr static WORD kCancelButtonControl = 0x0400;

  /**
   * @brief WM_COMMAND��wParam�����ʂł�����1�o�C�g���擾���邽�߂̃}�X�N
   */
  constexpr static WORD kControlMask = 0xff00;

  /**
   * @brief Window class��o�^���鎞�̖���
   */
  constexpr static const TCHAR* kWindowClassName = _T("KeyBindWindow");

  /**
   * @brief �N���C�A���g�̈�̗]�� 
   */
  constexpr static int kClientPadding = 8;

  /**
   * @brief �m�[�g�\�L�ύX�̐����X�^�e�B�b�N�R���g���[���̕�
   */
  constexpr static int kNoteLabelStyleStaticWidth = 100;

  /**
   * @brief �m�[�g�\�L�ύX�̃{�^���R���g���[���̕�
   */
  constexpr static int kNoteLabelStyleButtonWidth = 100;

  /**
   * @brief �L�[�o�C���h�̑Ώۂ�\������e�L�X�g�R���g���[���̕� 
   */
  constexpr static int kNameTextWidth = 160;

  /**
   * @brief �L�[�o�C���h�̃L�[��\������e�L�X�g�R���g���[���̕�
   */
  constexpr static int kKeyTextWidthH = 300;

  /**
   * @brief �L�[�o�C���h�ݒ�ύX�{�^���R���g���[���̕�
   */
  constexpr static int kChangeButtonWidth = 100;

  /**
   * @brief �L�[�o�C���h�ݒ�ύX�m��{�^���R���g���[���̕�
   */
  constexpr static int kCommitButtonWidth = 100;

  /**
   * @brief �L�[�o�C���h�ݒ�ύX�L�����Z���{�^���R���g���[���̕�
   */
  constexpr static int kCancelButtonWidth = 100;

  /**
   * @brief �L�[�o�C���h1�s�̍���
   */
  constexpr static int kLineHeight = 30;

  /**
   * @brief �R���g���[�������ɕ��ׂ�Ƃ��̗]��
   */
  constexpr static int kControlMarginRight = 8;

  /**
   * @brief �R���g���[�����c�ɕ��ׂ�Ƃ��̗]��
   */
  constexpr static int kControlMarginBottom = 8;

  /**
   * @brief �L�[�o�C���h��1��ʂɉ��s�\�����邩
   */
  constexpr static int kLinesOnPage = 12;

  /**
   * @brief �L�[���͂�\�����邽�߂̃e�L�X�g�p�o�b�t�@�T�C�Y
   */
  constexpr static size_t kKeyTextBufferLength = 64;

  /**
   * @brief �L�[�o�C���h�Ώۂ̋@�\�̖��̂̃��\�[�XID(���ێ�)
   */
  constexpr static int kKeyBindNameEnglishStyleResourceIds[Setting::KeyBindIndex::kLength] = {
#ifdef KEY_SEQUENCER_FULL_NOTE
      IDS_KEYBINDWINDOW_NOTE_CMINUS1,
      IDS_KEYBINDWINDOW_NOTE_CSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_DMINUS1,
      IDS_KEYBINDWINDOW_NOTE_DSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_EMINUS1,
      IDS_KEYBINDWINDOW_NOTE_FMINUS1,
      IDS_KEYBINDWINDOW_NOTE_FSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_GMINUS1,
      IDS_KEYBINDWINDOW_NOTE_GSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_AMINUS1,
      IDS_KEYBINDWINDOW_NOTE_ASMINUS1,
      IDS_KEYBINDWINDOW_NOTE_BMINUS1,
      IDS_KEYBINDWINDOW_NOTE_C0,
      IDS_KEYBINDWINDOW_NOTE_CS0,
      IDS_KEYBINDWINDOW_NOTE_D0,
      IDS_KEYBINDWINDOW_NOTE_DS0,
      IDS_KEYBINDWINDOW_NOTE_E0,
      IDS_KEYBINDWINDOW_NOTE_F0,
      IDS_KEYBINDWINDOW_NOTE_FS0,
      IDS_KEYBINDWINDOW_NOTE_G0,
      IDS_KEYBINDWINDOW_NOTE_GS0,
      IDS_KEYBINDWINDOW_NOTE_A0,
      IDS_KEYBINDWINDOW_NOTE_AS0,
      IDS_KEYBINDWINDOW_NOTE_B0,
      IDS_KEYBINDWINDOW_NOTE_C1,
      IDS_KEYBINDWINDOW_NOTE_CS1,
      IDS_KEYBINDWINDOW_NOTE_D1,
      IDS_KEYBINDWINDOW_NOTE_DS1,
      IDS_KEYBINDWINDOW_NOTE_E1,
      IDS_KEYBINDWINDOW_NOTE_F1,
      IDS_KEYBINDWINDOW_NOTE_FS1,
      IDS_KEYBINDWINDOW_NOTE_G1,
      IDS_KEYBINDWINDOW_NOTE_GS1,
      IDS_KEYBINDWINDOW_NOTE_A1,
      IDS_KEYBINDWINDOW_NOTE_AS1,
      IDS_KEYBINDWINDOW_NOTE_B1,
      IDS_KEYBINDWINDOW_NOTE_C2,
      IDS_KEYBINDWINDOW_NOTE_CS2,
      IDS_KEYBINDWINDOW_NOTE_D2,
      IDS_KEYBINDWINDOW_NOTE_DS2,
      IDS_KEYBINDWINDOW_NOTE_E2,
      IDS_KEYBINDWINDOW_NOTE_F2,
      IDS_KEYBINDWINDOW_NOTE_FS2,
      IDS_KEYBINDWINDOW_NOTE_G2,
      IDS_KEYBINDWINDOW_NOTE_GS2,
      IDS_KEYBINDWINDOW_NOTE_A2,
      IDS_KEYBINDWINDOW_NOTE_AS2,
      IDS_KEYBINDWINDOW_NOTE_B2,
#endif
      IDS_KEYBINDWINDOW_NOTE_C3,
      IDS_KEYBINDWINDOW_NOTE_CS3,
      IDS_KEYBINDWINDOW_NOTE_D3,
      IDS_KEYBINDWINDOW_NOTE_DS3,
      IDS_KEYBINDWINDOW_NOTE_E3,
      IDS_KEYBINDWINDOW_NOTE_F3,
      IDS_KEYBINDWINDOW_NOTE_FS3,
      IDS_KEYBINDWINDOW_NOTE_G3,
      IDS_KEYBINDWINDOW_NOTE_GS3,
      IDS_KEYBINDWINDOW_NOTE_A3,
      IDS_KEYBINDWINDOW_NOTE_AS3,
      IDS_KEYBINDWINDOW_NOTE_B3,
      IDS_KEYBINDWINDOW_NOTE_C4,
      IDS_KEYBINDWINDOW_NOTE_CS4,
      IDS_KEYBINDWINDOW_NOTE_D4,
      IDS_KEYBINDWINDOW_NOTE_DS4,
      IDS_KEYBINDWINDOW_NOTE_E4,
      IDS_KEYBINDWINDOW_NOTE_F4,
      IDS_KEYBINDWINDOW_NOTE_FS4,
      IDS_KEYBINDWINDOW_NOTE_G4,
      IDS_KEYBINDWINDOW_NOTE_GS4,
      IDS_KEYBINDWINDOW_NOTE_A4,
      IDS_KEYBINDWINDOW_NOTE_AS4,
      IDS_KEYBINDWINDOW_NOTE_B4,
      IDS_KEYBINDWINDOW_NOTE_C5,
      IDS_KEYBINDWINDOW_NOTE_CS5,
      IDS_KEYBINDWINDOW_NOTE_D5,
      IDS_KEYBINDWINDOW_NOTE_DS5,
      IDS_KEYBINDWINDOW_NOTE_E5,
      IDS_KEYBINDWINDOW_NOTE_F5,
      IDS_KEYBINDWINDOW_NOTE_FS5,
      IDS_KEYBINDWINDOW_NOTE_G5,
      IDS_KEYBINDWINDOW_NOTE_GS5,
      IDS_KEYBINDWINDOW_NOTE_A5,
      IDS_KEYBINDWINDOW_NOTE_AS5,
      IDS_KEYBINDWINDOW_NOTE_B5,
      IDS_KEYBINDWINDOW_NOTE_C6,
#ifdef KEY_SEQUENCER_FULL_NOTE
      IDS_KEYBINDWINDOW_NOTE_CS6,
      IDS_KEYBINDWINDOW_NOTE_D6,
      IDS_KEYBINDWINDOW_NOTE_DS6,
      IDS_KEYBINDWINDOW_NOTE_E6,
      IDS_KEYBINDWINDOW_NOTE_F6,
      IDS_KEYBINDWINDOW_NOTE_FS6,
      IDS_KEYBINDWINDOW_NOTE_G6,
      IDS_KEYBINDWINDOW_NOTE_GS6,
      IDS_KEYBINDWINDOW_NOTE_A6,
      IDS_KEYBINDWINDOW_NOTE_AS6,
      IDS_KEYBINDWINDOW_NOTE_B6,
      IDS_KEYBINDWINDOW_NOTE_C7,
      IDS_KEYBINDWINDOW_NOTE_CS7,
      IDS_KEYBINDWINDOW_NOTE_D7,
      IDS_KEYBINDWINDOW_NOTE_DS7,
      IDS_KEYBINDWINDOW_NOTE_E7,
      IDS_KEYBINDWINDOW_NOTE_F7,
      IDS_KEYBINDWINDOW_NOTE_FS7,
      IDS_KEYBINDWINDOW_NOTE_G7,
      IDS_KEYBINDWINDOW_NOTE_GS7,
      IDS_KEYBINDWINDOW_NOTE_A7,
      IDS_KEYBINDWINDOW_NOTE_AS7,
      IDS_KEYBINDWINDOW_NOTE_B7,
      IDS_KEYBINDWINDOW_NOTE_C8,
      IDS_KEYBINDWINDOW_NOTE_CS8,
      IDS_KEYBINDWINDOW_NOTE_D8,
      IDS_KEYBINDWINDOW_NOTE_DS8,
      IDS_KEYBINDWINDOW_NOTE_E8,
      IDS_KEYBINDWINDOW_NOTE_F8,
      IDS_KEYBINDWINDOW_NOTE_FS8,
      IDS_KEYBINDWINDOW_NOTE_G8,
      IDS_KEYBINDWINDOW_NOTE_GS8,
      IDS_KEYBINDWINDOW_NOTE_A8,
      IDS_KEYBINDWINDOW_NOTE_AS8,
      IDS_KEYBINDWINDOW_NOTE_B8,
      IDS_KEYBINDWINDOW_NOTE_C9,
      IDS_KEYBINDWINDOW_NOTE_CS9,
      IDS_KEYBINDWINDOW_NOTE_D9,
      IDS_KEYBINDWINDOW_NOTE_DS9,
      IDS_KEYBINDWINDOW_NOTE_E9,
      IDS_KEYBINDWINDOW_NOTE_F9,
      IDS_KEYBINDWINDOW_NOTE_FS9,
      IDS_KEYBINDWINDOW_NOTE_G9,
#endif
      IDS_KEYBINDWINDOW_BIND_HOTKEY_PLAY,
      IDS_KEYBINDWINDOW_BIND_MIDI_INPUT,
      IDS_KEYBINDWINDOW_BIND_ALL_STOP,
  };

  /**
   * @brief �L�[�o�C���h�Ώۂ̋@�\�̖��̂̃��\�[�XID(���}�n��)
   */
  constexpr static int kKeyBindNameYamahaStyleResourceIds[Setting::KeyBindIndex::kLength] = {
#ifdef KEY_SEQUENCER_FULL_NOTE
      IDS_KEYBINDWINDOW_NOTE_CMINUS2,
      IDS_KEYBINDWINDOW_NOTE_CSMINUS2,
      IDS_KEYBINDWINDOW_NOTE_DMINUS2,
      IDS_KEYBINDWINDOW_NOTE_DSMINUS2,
      IDS_KEYBINDWINDOW_NOTE_EMINUS2,
      IDS_KEYBINDWINDOW_NOTE_FMINUS2,
      IDS_KEYBINDWINDOW_NOTE_FSMINUS2,
      IDS_KEYBINDWINDOW_NOTE_GMINUS2,
      IDS_KEYBINDWINDOW_NOTE_GSMINUS2,
      IDS_KEYBINDWINDOW_NOTE_AMINUS2,
      IDS_KEYBINDWINDOW_NOTE_ASMINUS2,
      IDS_KEYBINDWINDOW_NOTE_BMINUS2,
      IDS_KEYBINDWINDOW_NOTE_CMINUS1,
      IDS_KEYBINDWINDOW_NOTE_CSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_DMINUS1,
      IDS_KEYBINDWINDOW_NOTE_DSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_EMINUS1,
      IDS_KEYBINDWINDOW_NOTE_FMINUS1,
      IDS_KEYBINDWINDOW_NOTE_FSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_GMINUS1,
      IDS_KEYBINDWINDOW_NOTE_GSMINUS1,
      IDS_KEYBINDWINDOW_NOTE_AMINUS1,
      IDS_KEYBINDWINDOW_NOTE_ASMINUS1,
      IDS_KEYBINDWINDOW_NOTE_BMINUS1,
      IDS_KEYBINDWINDOW_NOTE_C0,
      IDS_KEYBINDWINDOW_NOTE_CS0,
      IDS_KEYBINDWINDOW_NOTE_D0,
      IDS_KEYBINDWINDOW_NOTE_DS0,
      IDS_KEYBINDWINDOW_NOTE_E0,
      IDS_KEYBINDWINDOW_NOTE_F0,
      IDS_KEYBINDWINDOW_NOTE_FS0,
      IDS_KEYBINDWINDOW_NOTE_G0,
      IDS_KEYBINDWINDOW_NOTE_GS0,
      IDS_KEYBINDWINDOW_NOTE_A0,
      IDS_KEYBINDWINDOW_NOTE_AS0,
      IDS_KEYBINDWINDOW_NOTE_B0,
      IDS_KEYBINDWINDOW_NOTE_C1,
      IDS_KEYBINDWINDOW_NOTE_CS1,
      IDS_KEYBINDWINDOW_NOTE_D1,
      IDS_KEYBINDWINDOW_NOTE_DS1,
      IDS_KEYBINDWINDOW_NOTE_E1,
      IDS_KEYBINDWINDOW_NOTE_F1,
      IDS_KEYBINDWINDOW_NOTE_FS1,
      IDS_KEYBINDWINDOW_NOTE_G1,
      IDS_KEYBINDWINDOW_NOTE_GS1,
      IDS_KEYBINDWINDOW_NOTE_A1,
      IDS_KEYBINDWINDOW_NOTE_AS1,
      IDS_KEYBINDWINDOW_NOTE_B1,
#endif
      IDS_KEYBINDWINDOW_NOTE_C2,
      IDS_KEYBINDWINDOW_NOTE_CS2,
      IDS_KEYBINDWINDOW_NOTE_D2,
      IDS_KEYBINDWINDOW_NOTE_DS2,
      IDS_KEYBINDWINDOW_NOTE_E2,
      IDS_KEYBINDWINDOW_NOTE_F2,
      IDS_KEYBINDWINDOW_NOTE_FS2,
      IDS_KEYBINDWINDOW_NOTE_G2,
      IDS_KEYBINDWINDOW_NOTE_GS2,
      IDS_KEYBINDWINDOW_NOTE_A2,
      IDS_KEYBINDWINDOW_NOTE_AS2,
      IDS_KEYBINDWINDOW_NOTE_B2,
      IDS_KEYBINDWINDOW_NOTE_C3,
      IDS_KEYBINDWINDOW_NOTE_CS3,
      IDS_KEYBINDWINDOW_NOTE_D3,
      IDS_KEYBINDWINDOW_NOTE_DS3,
      IDS_KEYBINDWINDOW_NOTE_E3,
      IDS_KEYBINDWINDOW_NOTE_F3,
      IDS_KEYBINDWINDOW_NOTE_FS3,
      IDS_KEYBINDWINDOW_NOTE_G3,
      IDS_KEYBINDWINDOW_NOTE_GS3,
      IDS_KEYBINDWINDOW_NOTE_A3,
      IDS_KEYBINDWINDOW_NOTE_AS3,
      IDS_KEYBINDWINDOW_NOTE_B3,
      IDS_KEYBINDWINDOW_NOTE_C4,
      IDS_KEYBINDWINDOW_NOTE_CS4,
      IDS_KEYBINDWINDOW_NOTE_D4,
      IDS_KEYBINDWINDOW_NOTE_DS4,
      IDS_KEYBINDWINDOW_NOTE_E4,
      IDS_KEYBINDWINDOW_NOTE_F4,
      IDS_KEYBINDWINDOW_NOTE_FS4,
      IDS_KEYBINDWINDOW_NOTE_G4,
      IDS_KEYBINDWINDOW_NOTE_GS4,
      IDS_KEYBINDWINDOW_NOTE_A4,
      IDS_KEYBINDWINDOW_NOTE_AS4,
      IDS_KEYBINDWINDOW_NOTE_B4,
      IDS_KEYBINDWINDOW_NOTE_C5,
#ifdef KEY_SEQUENCER_FULL_NOTE
      IDS_KEYBINDWINDOW_NOTE_CS5,
      IDS_KEYBINDWINDOW_NOTE_D5,
      IDS_KEYBINDWINDOW_NOTE_DS5,
      IDS_KEYBINDWINDOW_NOTE_E5,
      IDS_KEYBINDWINDOW_NOTE_F5,
      IDS_KEYBINDWINDOW_NOTE_FS5,
      IDS_KEYBINDWINDOW_NOTE_G5,
      IDS_KEYBINDWINDOW_NOTE_GS5,
      IDS_KEYBINDWINDOW_NOTE_A5,
      IDS_KEYBINDWINDOW_NOTE_AS5,
      IDS_KEYBINDWINDOW_NOTE_B5,
      IDS_KEYBINDWINDOW_NOTE_C6,
      IDS_KEYBINDWINDOW_NOTE_CS6,
      IDS_KEYBINDWINDOW_NOTE_D6,
      IDS_KEYBINDWINDOW_NOTE_DS6,
      IDS_KEYBINDWINDOW_NOTE_E6,
      IDS_KEYBINDWINDOW_NOTE_F6,
      IDS_KEYBINDWINDOW_NOTE_FS6,
      IDS_KEYBINDWINDOW_NOTE_G6,
      IDS_KEYBINDWINDOW_NOTE_GS6,
      IDS_KEYBINDWINDOW_NOTE_A6,
      IDS_KEYBINDWINDOW_NOTE_AS6,
      IDS_KEYBINDWINDOW_NOTE_B6,
      IDS_KEYBINDWINDOW_NOTE_C7,
      IDS_KEYBINDWINDOW_NOTE_CS7,
      IDS_KEYBINDWINDOW_NOTE_D7,
      IDS_KEYBINDWINDOW_NOTE_DS7,
      IDS_KEYBINDWINDOW_NOTE_E7,
      IDS_KEYBINDWINDOW_NOTE_F7,
      IDS_KEYBINDWINDOW_NOTE_FS7,
      IDS_KEYBINDWINDOW_NOTE_G7,
      IDS_KEYBINDWINDOW_NOTE_GS7,
      IDS_KEYBINDWINDOW_NOTE_A7,
      IDS_KEYBINDWINDOW_NOTE_AS7,
      IDS_KEYBINDWINDOW_NOTE_B7,
      IDS_KEYBINDWINDOW_NOTE_C8,
      IDS_KEYBINDWINDOW_NOTE_CS8,
      IDS_KEYBINDWINDOW_NOTE_D8,
      IDS_KEYBINDWINDOW_NOTE_DS8,
      IDS_KEYBINDWINDOW_NOTE_E8,
      IDS_KEYBINDWINDOW_NOTE_F8,
      IDS_KEYBINDWINDOW_NOTE_FS8,
      IDS_KEYBINDWINDOW_NOTE_G8,
#endif
      IDS_KEYBINDWINDOW_BIND_HOTKEY_PLAY,
      IDS_KEYBINDWINDOW_BIND_MIDI_INPUT,
      IDS_KEYBINDWINDOW_BIND_ALL_STOP,
  };

  /**
   * @brief �o�C���h���ꂽ�L�[��\������ۂ̖���
   *
   * nullptr�̃L�[�͐ݒ�o���Ȃ�
   */
  constexpr static const TCHAR* kVirtualKeyNames[256] = {
      /*0x00*/ nullptr,
      /*0x01*/ nullptr,
      /*0x02*/ nullptr,
      /*0x03*/ nullptr,
      /*0x04*/ nullptr,
      /*0x05*/ nullptr,
      /*0x06*/ nullptr,
      /*0x07*/ nullptr,
      /*0x08*/ _T("Backspace"),
      /*0x09*/ _T("Tab"),
      /*0x0a*/ nullptr,
      /*0x0b*/ nullptr,
      /*0x0c*/ nullptr,
      /*0x0d*/ _T("Enter"),
      /*0x0e*/ nullptr,
      /*0x0f*/ nullptr,
      /*0x10*/ nullptr,
      /*0x11*/ nullptr,
      /*0x12*/ nullptr,
      /*0x13*/ _T("Pause"),
      /*0x14*/ nullptr,
      /*0x15*/ nullptr,
      /*0x16*/ nullptr,
      /*0x17*/ nullptr,
      /*0x18*/ nullptr,
      /*0x19*/ nullptr,
      /*0x1a*/ nullptr,
      /*0x1b*/ _T("Esc"),
      /*0x1c*/ nullptr,
      /*0x1d*/ nullptr,
      /*0x1e*/ nullptr,
      /*0x1f*/ nullptr,
      /*0x20*/ _T("Space"),
      /*0x21*/ _T("PageUp"),
      /*0x22*/ _T("PageDown"),
      /*0x23*/ _T("End"),
      /*0x24*/ _T("Home"),
      /*0x25*/ _T("Left"),
      /*0x26*/ _T("Up"),
      /*0x27*/ _T("Right"),
      /*0x28*/ _T("Down"),
      /*0x29*/ nullptr,
      /*0x2a*/ nullptr,
      /*0x2b*/ nullptr,
      /*0x2c*/ _T("PrintScreen"),
      /*0x2d*/ _T("Insert"),
      /*0x2e*/ _T("Delete"),
      /*0x2f*/ nullptr,
      /*0x30*/ _T("0"),
      /*0x31*/ _T("1"),
      /*0x32*/ _T("2"),
      /*0x33*/ _T("3"),
      /*0x34*/ _T("4"),
      /*0x35*/ _T("5"),
      /*0x36*/ _T("6"),
      /*0x37*/ _T("7"),
      /*0x38*/ _T("8"),
      /*0x39*/ _T("9"),
      /*0x3a*/ nullptr,
      /*0x3b*/ nullptr,
      /*0x3c*/ nullptr,
      /*0x3d*/ nullptr,
      /*0x3e*/ nullptr,
      /*0x3f*/ nullptr,
      /*0x40*/ nullptr,
      /*0x41*/ _T("A"),
      /*0x42*/ _T("B"),
      /*0x43*/ _T("C"),
      /*0x44*/ _T("D"),
      /*0x45*/ _T("E"),
      /*0x46*/ _T("F"),
      /*0x47*/ _T("G"),
      /*0x48*/ _T("H"),
      /*0x49*/ _T("I"),
      /*0x4a*/ _T("J"),
      /*0x4b*/ _T("K"),
      /*0x4c*/ _T("L"),
      /*0x4d*/ _T("M"),
      /*0x4e*/ _T("N"),
      /*0x4f*/ _T("O"),
      /*0x50*/ _T("P"),
      /*0x51*/ _T("Q"),
      /*0x52*/ _T("R"),
      /*0x53*/ _T("S"),
      /*0x54*/ _T("T"),
      /*0x55*/ _T("U"),
      /*0x56*/ _T("V"),
      /*0x57*/ _T("W"),
      /*0x58*/ _T("X"),
      /*0x59*/ _T("Y"),
      /*0x5a*/ _T("Z"),
      /*0x5b*/ nullptr,
      /*0x5c*/ nullptr,
      /*0x5d*/ nullptr,
      /*0x5e*/ nullptr,
      /*0x5f*/ nullptr,
      /*0x60*/ _T("Num0"),
      /*0x61*/ _T("Num1"),
      /*0x62*/ _T("Num2"),
      /*0x63*/ _T("Num3"),
      /*0x64*/ _T("Num4"),
      /*0x65*/ _T("Num5"),
      /*0x66*/ _T("Num6"),
      /*0x67*/ _T("Num7"),
      /*0x68*/ _T("Num8"),
      /*0x69*/ _T("Num9"),
      /*0x6a*/ _T("Num*"),
      /*0x6b*/ _T("Num+"),
      /*0x6c*/ nullptr,
      /*0x6d*/ _T("Num-"),
      /*0x6e*/ _T("Num."),
      /*0x6f*/ _T("Num/"),
      /*0x70*/ _T("F1"),
      /*0x71*/ _T("F2"),
      /*0x72*/ _T("F3"),
      /*0x73*/ _T("F4"),
      /*0x74*/ _T("F5"),
      /*0x75*/ _T("F6"),
      /*0x76*/ _T("F7"),
      /*0x77*/ _T("F8"),
      /*0x78*/ _T("F9"),
      /*0x79*/ _T("F10"),
      /*0x7a*/ _T("F11"),
      /*0x7b*/ _T("F12"),
      /*0x7c*/ nullptr,
      /*0x7d*/ nullptr,
      /*0x7e*/ nullptr,
      /*0x7f*/ nullptr,
      /*0x80*/ nullptr,
      /*0x81*/ nullptr,
      /*0x82*/ nullptr,
      /*0x83*/ nullptr,
      /*0x84*/ nullptr,
      /*0x85*/ nullptr,
      /*0x86*/ nullptr,
      /*0x87*/ nullptr,
      /*0x88*/ nullptr,
      /*0x89*/ nullptr,
      /*0x8a*/ nullptr,
      /*0x8b*/ nullptr,
      /*0x8c*/ nullptr,
      /*0x8d*/ nullptr,
      /*0x8e*/ nullptr,
      /*0x8f*/ nullptr,
      /*0x90*/ nullptr,
      /*0x91*/ _T("ScrollLock"),
      /*0x92*/ nullptr,
      /*0x93*/ nullptr,
      /*0x94*/ nullptr,
      /*0x95*/ nullptr,
      /*0x96*/ nullptr,
      /*0x97*/ nullptr,
      /*0x98*/ nullptr,
      /*0x99*/ nullptr,
      /*0x9a*/ nullptr,
      /*0x9b*/ nullptr,
      /*0x9c*/ nullptr,
      /*0x9d*/ nullptr,
      /*0x9e*/ nullptr,
      /*0x9f*/ nullptr,
      /*0xa0*/ nullptr,
      /*0xa1*/ nullptr,
      /*0xa2*/ nullptr,
      /*0xa3*/ nullptr,
      /*0xa4*/ nullptr,
      /*0xa5*/ nullptr,
      /*0xa6*/ nullptr,
      /*0xa7*/ nullptr,
      /*0xa8*/ nullptr,
      /*0xa9*/ nullptr,
      /*0xaa*/ nullptr,
      /*0xab*/ nullptr,
      /*0xac*/ nullptr,
      /*0xad*/ nullptr,
      /*0xae*/ nullptr,
      /*0xaf*/ nullptr,
      /*0xb0*/ nullptr,
      /*0xb1*/ nullptr,
      /*0xb2*/ nullptr,
      /*0xb3*/ nullptr,
      /*0xb4*/ nullptr,
      /*0xb5*/ nullptr,
      /*0xb6*/ nullptr,
      /*0xb7*/ nullptr,
      /*0xb8*/ nullptr,
      /*0xb9*/ nullptr,
      /*0xba*/ _T(":"),
      /*0xbb*/ _T(";"),
      /*0xbc*/ _T(","),
      /*0xbd*/ _T("-"),
      /*0xbe*/ _T("."),
      /*0xbf*/ _T("/"),
      /*0xc0*/ _T("@"),
      /*0xc1*/ nullptr,
      /*0xc2*/ nullptr,
      /*0xc3*/ nullptr,
      /*0xc4*/ nullptr,
      /*0xc5*/ nullptr,
      /*0xc6*/ nullptr,
      /*0xc7*/ nullptr,
      /*0xc8*/ nullptr,
      /*0xc9*/ nullptr,
      /*0xca*/ nullptr,
      /*0xcb*/ nullptr,
      /*0xcc*/ nullptr,
      /*0xcd*/ nullptr,
      /*0xce*/ nullptr,
      /*0xcf*/ nullptr,
      /*0xd0*/ nullptr,
      /*0xd1*/ nullptr,
      /*0xd2*/ nullptr,
      /*0xd3*/ nullptr,
      /*0xd4*/ nullptr,
      /*0xd5*/ nullptr,
      /*0xd6*/ nullptr,
      /*0xd7*/ nullptr,
      /*0xd8*/ nullptr,
      /*0xd9*/ nullptr,
      /*0xda*/ nullptr,
      /*0xdb*/ _T("["),
      /*0xdc*/ _T("\\"),
      /*0xdd*/ _T("]"),
      /*0xde*/ _T("^"),
      /*0xdf*/ nullptr,
      /*0xe0*/ nullptr,
      /*0xe1*/ nullptr,
      /*0xe2*/ _T("\\"),
      /*0xe3*/ nullptr,
      /*0xe4*/ nullptr,
      /*0xe5*/ nullptr,
      /*0xe6*/ nullptr,
      /*0xe7*/ nullptr,
      /*0xe8*/ nullptr,
      /*0xe9*/ nullptr,
      /*0xea*/ nullptr,
      /*0xeb*/ nullptr,
      /*0xec*/ nullptr,
      /*0xed*/ nullptr,
      /*0xee*/ nullptr,
      /*0xef*/ nullptr,
      /*0xf0*/ nullptr,
      /*0xf1*/ nullptr,
      /*0xf2*/ nullptr,
      /*0xf3*/ nullptr,
      /*0xf4*/ nullptr,
      /*0xf5*/ nullptr,
      /*0xf6*/ nullptr,
      /*0xf7*/ nullptr,
      /*0xf8*/ nullptr,
      /*0xf9*/ nullptr,
      /*0xfa*/ nullptr,
      /*0xfb*/ nullptr,
      /*0xfc*/ nullptr,
      /*0xfd*/ nullptr,
      /*0xfe*/ nullptr,
      /*0xff*/ nullptr,
  };

  /**
   * @brief �A�v���P�[�V�����̃C���X�^���X�n���h��
   */
  HINSTANCE const instance_handle_;

  /**
   * @brief �e�E�B���h�E�̃n���h��
   */
  HWND const parent_window_handle_;

  /**
   * @brief ���\�[�X�̕�����e�[�u��
   */
  const StringTable* const string_table_;

  /**
   * @brief �ݒ� 
   */
  Setting* const setting_;

  /**
   * @brief �E�B���h�E�n���h��
   */
  HWND window_handle_ = nullptr;

  /**
   * @brief �E�B���h�E�����ς݃t���O
   */
  bool created_ = false;

  /**
   * @brief ���b�Z�[�W�����t���O
   */
  bool valid_ = true;

  /**
   * @brief �m�[�g�\�L�ύX�̐����X�^�e�B�b�N�R���g���[���̃n���h��
   */
  HWND note_label_style_static_handle_ = nullptr;

  /**
   * @brief �m�[�g�\�L�ύX�p�̃��W�I�{�^���R���g���[��(���ێ�)�̃n���h��
   */
  HWND note_label_style_english_button_handle_ = nullptr;

  /**
   * @brief �m�[�g�\�L�ύX�p�̃��W�I�{�^���R���g���[��(���}�n��)�̃n���h��
   */
  HWND note_label_style_yamaha_button_handle_ = nullptr;

  /**
   * @brief �L�[�o�C���h�̑Ώۂ�\������e�L�X�g�R���g���[���̃n���h��
   */
  HWND label_text_handles_[Setting::KeyBindIndex::kLength];

  /**
   * @brief �L�[�o�C���h�̃L�[��\������e�L�X�g�R���g���[��
   */
  HWND key_text_handles_[Setting::KeyBindIndex::kLength];

  /**
   * @brief �L�[�o�C���h�ݒ�ύX�{�^���R���g���[���̃n���h��
   */
  HWND change_button_handles_[Setting::KeyBindIndex::kLength];

  /**
   * @brief �L�[�o�C���h�ݒ�ύX�m��{�^���R���g���[���̃n���h��
   */
  HWND commit_button_handles_[Setting::KeyBindIndex::kLength];

  /**
   * @brief �L�[�o�C���h�ݒ�ύX�L�����Z���{�^���R���g���[���̃n���h��
   */
  HWND cancel_button_handles_[Setting::KeyBindIndex::kLength];

  /**
   * @brief ���ݐݒ�ΏۂƂȂ��Ă���L�[�o�C���h�̃C���f�b�N�X
   * 
   * �ݒ�Ώۂ������Ƃ��̒l��Setting::KeyBindIndex::kLength
   */
  std::size_t target_index_ = Setting::KeyBindIndex::kLength;

  /**
   * @brief �L�[�I�����ɓ��͂���Ă���L�[���(�C���L�[�����܂�)
   */
  Setting::KeyBind current_key_bind_;

  /**
   * @brief �L�[�I�����ɓ��͂���Ă���C���L�[���(current_key_bind_�̂��߂̈ꎞ���)
   */
  Setting::KeyBind modifier_key_state_;

  /**
   * @brief �L�[�I�����ɃL�[���\�����邽�߂̃e�L�X�g�p�o�b�t�@
   */
  TCHAR key_bind_text_buffer_[kKeyTextBufferLength];

  /**
   * @brief �X�N���[���o�[����p�̍\����
   */
  SCROLLINFO scroll_info_;

  /**
   * @brief Window class�̓o�^�������1�x�����s�������p�̊֐�
   *
   * Window class�̓o�^���s��
   *
   * @param instanceHandle �A�v���P�[�V�����̃C���X�^���X�n���h��
   *
   * @return �Œ��0��Ԃ�
   */
  static int RegisterWindowClass(HINSTANCE instance_handle);

  /**
   * @brief �V�X�e������R�[���o�b�N�����E�B���h�E�v���V�[�W��
   */
  static LRESULT WINAPI CallbackProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

  /**
   * @brief �E�B���h�E�v���V�[�W���������e
   * 
   * Windows����Ăяo���ꂽCallbackProcedure���ŃC���X�^���X��������������ɏ������Ϗ�����
   * ����āA�����Ɩ߂�l�̓E�B���h�E�v���V�[�W���̎d�l�ɏ]��
   * 
   * @param window_handle �E�B���h�E�n���h��
   * @param message ���b�Z�[�W
   * @param wparam ���b�Z�[�W�p�����[�^1
   * @param lparam ���b�Z�[�W�p�����[�^2
   * 
   * @return ����
   */
  LRESULT Procedure(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);

  /**
   * @brief �o�C���h���ꂽ�L�[����\���p�̕�����ɕϊ�����
   *
   * @param key_bind �L�[���
   *
   * @return �e�L�X�g���������܂ꂽ�o�b�t�@��Ԃ�
   *         �o�b�t�@���̂̓N���X�����L���Ă���̂œǏo�����ł͎Q�Ƃ݂̂Ɏg�p����
   */
  const TCHAR* KeyBindToString(Setting::KeyBind key_bind);
};
}  // namespace keysequencer

#endif