#ifndef KEYSEQUENCER_SETTING_H_
#define KEYSEQUENCER_SETTING_H_

#include <Windows.h>
#include <tchar.h>

#include <cstddef>
#include <cstdint>

namespace keysequencer {

/**
 * @brief �A�v���P�[�V�����̐ݒ�l�Ǘ�
 */
class Setting {
 public:
  /**
   * @brief �L�[�o�C���h���
   * 
   * 2�o�C�g�̃f�[�^�ňȉ��̈Ӗ�������
   * ���1�o�C�g: �C���L�[(ModifierKey��OR)
   * ����1�o�C�g: ���z�L�[�R�[�h(VK_A�Ȃ�)
   */
  using KeyBind = std::uint16_t;

  /**
   * @brief �L�[�o�C���h��Shift�L�[��\���r�b�g
   */
  constexpr static KeyBind kShiftKeyBind = 0x0100;

  /**
   * @brief �L�[�o�C���h��Ctrl�L�[��\���r�b�g
   */
  constexpr static KeyBind kControlKeyBind = 0x0200;

  /**
   * @brief �L�[�o�C���h��Alt�L�[��\���r�b�g
   */
  constexpr static KeyBind kAlternateKeyBind = 0x0400;

  /**
   * @brief �A�v���P�[�V�����������m�[�g�ԍ��̍ŏ�
   */
#ifdef KEY_SEQUENCER_FULL_NOTE
  constexpr static std::uint8_t kMinimumNoteNumber = 0x00;
#else
  constexpr static std::uint8_t kMinimumNoteNumber = 0x30;
#endif

  /**
   * @brief �A�v���P�[�V�����������m�[�g�ԍ��̍ő�
   */
#ifdef KEY_SEQUENCER_FULL_NOTE
  constexpr static std::uint8_t kMaximumNoteNumber = 0x7F;
#else
  constexpr static std::uint8_t kMaximumNoteNumber = 0x54;
#endif

  /**
   * @brief �L�[�o�C���h�f�[�^���̂ǂ��ɉ��������Ă��邩�̏��
   */
  enum KeyBindIndex : std::size_t {
    /**
     * @brief �����m�[�g�ԍ��̍ŏ��l(�������爵���m�[�g������)
     */
    kNoteNumber = 0,

    /**
     * @brief �Đ�/��~�̃z�b�g�L�[
     */
    kPlayHotkey = kNoteNumber + kMaximumNoteNumber - kMinimumNoteNumber + 1,

    /**
     * @brief MIDI���͊J�n�̃z�b�g�L�[
     */
    kMidiInputHotkey,

    /**
     * @brief �S��~�̃z�b�g�L�[ 
     */
    kAllStopHotkey,

    /**
     * @brief �L�[�o�C���h����
     */
    kLength,
  };

  /**
   * @brief �m�[�g�̕\�L����
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  enum class NoteLabelStyle : int {
    /**
     * @brief ���ێ�(�p��ł̌ď̂��s���B�p��\�L�ƈꏏ�炵���̂łƂ肠����English) 
     */
    kEnglish = 0,

    /**
     * @brief ���}�n��
     */
    kYamaha = 1,
  };

  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  Setting();

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  Setting(const Setting&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  Setting(Setting&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  Setting& operator=(const Setting&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  Setting& operator=(Setting&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~Setting() = default;

  /**
   * @brief .ini�t�@�C������f�[�^��ǂݏo��
   * 
   * ���s�t�@�C���p�X�̊g���q��.ini�ɕύX�����t�@�C���p�X��ΏۂƂ���
   * .ini�t�@�C�������݂��Ȃ��ꍇ�͐V���ɍ쐬����
   * �܂��A�ȍ~�̐ݒ�ǂݏ�������ł͂���.ini�t�@�C���𑀍삷��
   *
   * @return ���������ꍇtrue��Ԃ�
   */
  bool LoadIniFile();

  /**
   * @brief �L�[�o�C���h��ݒ�
   *
   * @param index �Ώۂ̃L�[�o�C���h�̈ʒu
   * @param key_bind �L�[�o�C���h
   */
  void SetKeyBind(std::size_t index, KeyBind key_bind);

  /**
   * @brief �L�[�o�C���h���擾
   * 
   * @param index �Ώۂ̃L�[�o�C���h�̈ʒu
   * 
   * @return �L�[�o�C���h
   */
  inline KeyBind GetKeyBind(std::size_t index) const {
    return key_binds_[index];
  }

  /**
   * @brief �m�[�g�\�L������ݒ�
   * 
   * @param note_label_style �m�[�g�\�L����
   */
  void SetNoteLabelStyle(NoteLabelStyle note_label_style);

  /**
   * @brief �m�[�g�\�L�������擾
   * 
   * @return �m�[�g�\�L����
   */
  inline NoteLabelStyle GetNoteLabelStyle() const {
    return note_label_style_;
  }

  /**
   * @brief �m�[�g�ԍ��ɑ΂���L�[���蓖�Ă��擾
   *
   * @param note_number �m�[�g�ԍ�
   *
   * @return �L�[���蓖�ď��
   */
  inline KeyBind GetKeyBindByNoteNumber(std::uint8_t note_number) const {
    if (note_number < kMinimumNoteNumber || kMaximumNoteNumber < note_number)
      return 0;

    return key_binds_[KeyBindIndex::kNoteNumber + (note_number - kMinimumNoteNumber)];
  }

  /**
   * @brief �Đ�/��~�̃z�b�g�L�[���蓖�Ă��擾
   *
   * @return �L�[���蓖�ď��
   */
  inline KeyBind GetPlayHotkeyBind() const {
    return key_binds_[KeyBindIndex::kPlayHotkey];
  }

  /**
   * @brief MIDI���͎�t�̃z�b�g�L�[���蓖�Ă��擾
   *
   * @return �L�[���蓖�ď��
   */
  inline KeyBind GetMidiInputHotkeyBind() const {
    return key_binds_[KeyBindIndex::kMidiInputHotkey];
  }

  /**
   * @brief �S��~�̃z�b�g�L�[���蓖�Ă��擾
   *
   * @return �L�[���蓖�ď��
   */
  inline KeyBind GetAllStopHotkeyBind() const {
    return key_binds_[KeyBindIndex::kAllStopHotkey];
  }

  /**
   * @brief �m�[�gOFF�C�x���g������艽�~���b�����L�[�𗣂����̐ݒ���擾����
   * 
   * ����Set�͑��݂��Ȃ�
   * 
   * @return �~���b
   */
  inline std::uint32_t GetEarlyKeyReleaseMillisecond() const {
    return early_key_release_millisecond_;
  }

  /**
   * @brief �C���L�[���m�[�gON���牽�~���b�����ăA�b�v�_�E�����邩�̐ݒ���擾����
   * 
   * ����Set�͑��݂��Ȃ�
   * 
   * @return �~���b
   */
  inline std::uint32_t GetModifierKeyDelayMillisecond() const {
    return modifier_key_delay_millisecond_;
  }

  /**
   * @brief ����C���L�[���������̎��Ԉȓ��Ȃ��ɂ܂Ƃ߂�
   * 
   * ����Set�͑��݂��Ȃ�
   * 
   * @return �~���b
   */
  inline std::uint32_t GetModifierKeyDownJoinMillisecond() const {
    return modifier_key_down_join_millisecond_;
  }

 private:
  /**
   * @brief .ini�t�@�C�� �Z�N�V������: �L�[�o�C���h
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  constexpr static const TCHAR* kKeyBindsIniSection = _T("KeyBinds");

  /**
   * @brief .ini�t�@�C�� �Z�N�V������: �I�v�V����(�ݒ肪�����Ă��悢����)
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  constexpr static const TCHAR* kOptionalIniSection = _T("Optional");

  /**
   * @brief .ini�t�@�C�� �L�[������: �L�[�o�C���h
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  constexpr static const TCHAR* kKeyBindIniKeys[KeyBindIndex::kLength] = {
#ifdef KEY_SEQUENCER_FULL_NOTE
      _T("C-1"),
      _T("Cs-1"),
      _T("D-1"),
      _T("Ds-1"),
      _T("E-1"),
      _T("F-1"),
      _T("Fs-1"),
      _T("G-1"),
      _T("Gs-1"),
      _T("A-1"),
      _T("As-1"),
      _T("B-1"),
      _T("C0"),
      _T("Cs0"),
      _T("D0"),
      _T("Ds0"),
      _T("E0"),
      _T("F0"),
      _T("Fs0"),
      _T("G0"),
      _T("Gs0"),
      _T("A0"),
      _T("As0"),
      _T("B0"),
      _T("C1"),
      _T("Cs1"),
      _T("D1"),
      _T("Ds1"),
      _T("E1"),
      _T("F1"),
      _T("Fs1"),
      _T("G1"),
      _T("Gs1"),
      _T("A1"),
      _T("As1"),
      _T("B1"),
      _T("C2"),
      _T("Cs2"),
      _T("D2"),
      _T("Ds2"),
      _T("E2"),
      _T("F2"),
      _T("Fs2"),
      _T("G2"),
      _T("Gs2"),
      _T("A2"),
      _T("As2"),
      _T("B2"),
#endif
      _T("C3"),
      _T("Cs3"),
      _T("D3"),
      _T("Ds3"),
      _T("E3"),
      _T("F3"),
      _T("Fs3"),
      _T("G3"),
      _T("Gs3"),
      _T("A3"),
      _T("As3"),
      _T("B3"),
      _T("C4"),
      _T("Cs4"),
      _T("D4"),
      _T("Ds4"),
      _T("E4"),
      _T("F4"),
      _T("Fs4"),
      _T("G4"),
      _T("Gs4"),
      _T("A4"),
      _T("As4"),
      _T("B4"),
      _T("C5"),
      _T("Cs5"),
      _T("D5"),
      _T("Ds5"),
      _T("E5"),
      _T("F5"),
      _T("Fs5"),
      _T("G5"),
      _T("Gs5"),
      _T("A5"),
      _T("As5"),
      _T("B5"),
      _T("C6"),
#ifdef KEY_SEQUENCER_FULL_NOTE
      _T("Cs6"),
      _T("D6"),
      _T("Ds6"),
      _T("E6"),
      _T("F6"),
      _T("Fs6"),
      _T("G6"),
      _T("Gs6"),
      _T("A6"),
      _T("As6"),
      _T("B6"),
      _T("C7"),
      _T("Cs7"),
      _T("D7"),
      _T("Ds7"),
      _T("E7"),
      _T("F7"),
      _T("Fs7"),
      _T("G7"),
      _T("Gs7"),
      _T("A7"),
      _T("As7"),
      _T("B7"),
      _T("C8"),
      _T("Cs8"),
      _T("D8"),
      _T("Ds8"),
      _T("E8"),
      _T("F8"),
      _T("Fs8"),
      _T("G8"),
      _T("Gs8"),
      _T("A8"),
      _T("As8"),
      _T("B8"),
      _T("C9"),
      _T("Cs9"),
      _T("D9"),
      _T("Ds9"),
      _T("E9"),
      _T("F9"),
      _T("Fs9"),
      _T("G9"),
#endif
      _T("HotkeyPlay"),
      _T("HotkeyMidiInput"),
      _T("HotkeyAllStop"),
  };

  /**
   * @brief .ini�t�@�C�� �L�[������: �m�[�g�\�L����
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  constexpr static const TCHAR* kNoteLabelStyleIniKey = _T("NoteLabelStyle");

  /**
   * @brief .ini�t�@�C�� �L�[������: �m�[�gOFF�C�x���g������艽�~���b�����L�[�𗣂���
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  constexpr static const TCHAR* kEarlyKeyReleaseMillisecondIniKey = _T("EarlyKeyReleaseMilliseconds");

  /**
   * @brief .ini�t�@�C�� �L�[������: �C���L�[���m�[�gON���牽�~���b�����ăA�b�v�_�E�����邩
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  constexpr static const TCHAR* kModifierKeyDelayMillisecondIniKey = _T("ModifierKeyDelayMilliseconds");

  /**
   * @brief .ini�t�@�C�� �L�[������: ����C���L�[���������̎��Ԉȓ��Ȃ��ɂ܂Ƃ߂�
   * 
   * �ύX����ƃo�[�W�����ԂŌ݊����������Ȃ��Ă��܂��̂ŕύX���Ă͂Ȃ�Ȃ�
   */
  constexpr static const TCHAR* kModifierKeyDownJoinMillisecondIniKey = _T("ModifierKeyDownJoinMilliseconds");

  /**
   * @brief �f�t�H���g�̃L�[�o�C���h
   *
   * �N������.ini�t�@�C�����ǂݍ��߂Ȃ��ꍇ�ɂ�����g�p����
   */
  constexpr static KeyBind kDefaultKeyBinds[KeyBindIndex::kLength] = {
#ifdef KEY_SEQUENCER_FULL_NOTE
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
#endif
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      '0',
      'Z',
      'X',
      'Q',
      'W',
      'E',
      'R',
      'T',
      'Y',
      'U',
      'I',
      'O',
      'P',
      'C',
      'V',
      'A',
      'S',
      'D',
      'F',
      'G',
      'H',
      'J',
      'K',
      'L',
      'B',
      'N',
      'M',
      VK_OEM_COMMA,  // ,
#ifdef KEY_SEQUENCER_FULL_NOTE
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
#endif
      VK_F11,
      VK_F10,
      VK_F12,
  };

  /**
   * @brief .ini�t�@�C���p�X
   */
  TCHAR ini_file_path_[MAX_PATH];

  /**
   * @brief �L�[�o�C���h�f�[�^
   */
  KeyBind key_binds_[KeyBindIndex::kLength];

  /**
   * @brief �m�[�g�\�L����
   */
  NoteLabelStyle note_label_style_ = NoteLabelStyle::kEnglish;

  /**
   * @brief �m�[�gOFF�C�x���g������艽�~���b�����L�[�𗣂����̎w��
   *
   * �L�[�𗣂�������ɃL�[����������������ꍇ�ɁA�L�[����������̎󂯕t�������������錻�ۂ�����̂�
   * �L�[�𗣂������MIDI�C�x���g������肱�̐ݒ�l�������s��
   */
  std::uint32_t early_key_release_millisecond_ = 0;

  /**
   * @brief �C���L�[���m�[�gON���牽�~���b�����ăA�b�v�_�E�����邩
   * 
   * �C���L�[�̃A�b�v�_�E�����L�[�_�E���̑O��Ɋm���ɏE����悤�ɂ�����x�P�\������
   * �ΏۃA�v������FPS����������͏��ɒ����ɏ�������قǏ������l�ł悢
   * ���܂�傫�Ȓl�ɂ���ƑO��̃m�[�gON�ɉe�����Ă���
   */
  std::uint32_t modifier_key_delay_millisecond_ = 50;

  /**
   * @brief ����C���L�[���������̎��Ԉȓ��Ȃ��ɂ܂Ƃ߂�
   */
  std::uint32_t modifier_key_down_join_millisecond_ = 1000;
};

}  // namespace keysequencer

#endif