#ifndef KEYSEQUENCER_SEQUENCER_H_
#define KEYSEQUENCER_SEQUENCER_H_

#include <cstddef>
#include <cstdint>

#include "midi_synthesizer.h"
#include "setting.h"
#include "standard_midi_format.h"
#include "standard_midi_format_event.h"
#include "standard_midi_format_reader.h"

namespace keysequencer {

class Keyboard;

/**
 * @brief ���t�����{��
 *
 * ���t�����ɂ͐�p�̃X���b�h���g�����A
 * ���t�ƃX���b�h���염���̏����ƃf�[�^�𓯈�N���X���ň����ƕ��G�Ȃ���
 * ���t�p�̏����ƃf�[�^�𕪗�����
 * �X���b�h�����X���b�h�Ԃ̔r�������SequencerManager���ōs��
 */
class Sequencer {
 public:
  /**
   * @brief �o�͐�
   */
  enum class Output {
    /**
     * @brief �L�[�{�[�h
     */
    kKeyboard,

    /**
     * @brief MIDI����
     */
    kMidiSynthesizer,
  };

  /**
   * @brief �t�@�C���ǂݍ��݌���
   */
  enum class LoadFileResult {
    /**
     * @brief ����I�� 
     */
    kSuccess,

    /**
     * @brief �t�@�C�����J���Ȃ�
     */
    kFailureFileOpen,

    /**
     * @brief 4GB�ȏ�̃t�@�C���͔�Ή�
     */
    kFailureFileTooLarge,

    /**
     * @brief �t�@�C���ǂݍ��ݒ��ɕs���ȃG���[
     */
    kFailureFileRead,

    /**
     * @brief SMF�Ƃ��ĕs��
     */
    kFailureIllegalSmf,

    /**
     * @brief SMF�Ƃ��Ă͐��������{�v���O�������T�|�[�g���Ă��Ȃ�SMF�`��
     */
    kFailureNotSupportSmf,
  };

  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  Sequencer() = delete;

  /**
   * @brief �R���X�g���N�^
   * 
   * @param setting �ݒ�
   * @param keyboard �L�[�{�[�h
   */
  Sequencer(const Setting* setting, Keyboard* keyboard);

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  Sequencer(const Sequencer&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  Sequencer(Sequencer&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  Sequencer& operator=(const Sequencer&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  Sequencer& operator=(Sequencer&&) = delete;

  /**
   * @brief �f�X�g���N�^ 
   */
  ~Sequencer() = default;

  /**
   * @brief �o�͐���擾
   * 
   * Close()���Ă��l���N���A������͂��Ȃ��̂ŁAOpen() �` Close()�ԈȊO�ł͎g�p���Ȃ�����
   * 
   * @return �o�͐�
   */
  inline Output GetOutput() const {
    return output_;
  }

  /**
   * @brief �t�@�C�����ǂݍ��ݍς݂��m�F
   * 
   * @return �t�@�C�����G���[�����ǂݍ��܂�A�X�L�������ă`�F�b�N���ςݎg�p�o�����Ԃł���ꍇ��true��Ԃ�
   */
  inline bool IsFileLoaded() const {
    return file_loaded_;
  }

  /**
   * @brief �t�@�C����ǂݍ���
   * 
   * @param filepath �t�@�C����
   * 
   * @return �ǂݍ��݌���
   */
  LoadFileResult LoadFile(const TCHAR* const filepath);

  /**
   * @brief ���t�̊J�n����������
   * 
   * ���t�J�n�ʒu��擪�ɐݒ肷��
   * 
   * @param output ���t�̏o�͐�
   * 
   * @return �J�n�ɐ��������ꍇ��true��Ԃ�
   */
  bool Open(Output output);

  /**
   * @brief ���t�̏I������������
   */
  void Close();

  /**
   * @brief ���t����
   *
   * @param sleep_millisecond ���̉��t�����܂őҋ@���鎞��(�~���b)
   * 
   * @return ���t�������p������ꍇ(���̉��t����������ꍇ)��true��Ԃ�
   *         (false��Ԃ����ꍇ�I�[�܂ŏ��������Ƃ�������)
   */
  bool Run(DWORD* sleep_millisecond);

 private:
  // �C���L�[�̃f�[�^��z��Ƃ��Ď����̂��߂̒萔
  enum {
    /**
     * @brief Shift
     */
    kShiftKey,

    /**
     * @brief Ctrl 
     */
    kControlKey,

    /**
     * @brief Alt
     */
    kAlternateKey,

    /**
     * @brief �C���L�[�̑���
     */
    kModifierKeyLength
  };

  /**
   * @brief �C���L�[�̃L�[�o�C���h��̉��z�L�[�R�[�h
   */
  constexpr static Setting::KeyBind kModifierKeys[kModifierKeyLength] = {Setting::kShiftKeyBind, Setting::kControlKeyBind, Setting::kAlternateKeyBind};

  /**
   * @brief �C���L�[��Windows���z�L�[�R�[�h 
   */
  constexpr static std::uint8_t kVirtualModifierKeyCodes[kModifierKeyLength] = {VK_SHIFT, VK_CONTROL, VK_MENU};

  /**
   * @brief �L�[����C�x���g�̓��쎞��
   */
  struct KeyEventTime {
    /**
     * @brief �\���̂���ł��邱�Ƃ�����index�̒l
     * 
     * 4GB�ȏ��MIDI�ɂ͑Ή����Ȃ�����index��32bit int max�܂œ��B���邱�Ƃ͖���
     */
    constexpr static std::uint32_t kNullIndex = 0xffffffff;

    /**
     * @brief �L�[����C�x���g�̌��ƂȂ�MIDI�C�x���g�̃C���f�b�N�X
     * 
     * ������KeyEventTime�����ꎞ��(millisecond)�������ꍇ�ɏ����������肷�邽�߂Ɏg�p����
     */
    uint32_t index = kNullIndex;

    /**
     * @brief �L�[����C�x���g�𓮍삳��鎞��
     *
     * ���t�J�n����̌o�ߎ���(�~���b)
     */
    uint32_t millisecond = 0;

    /**
     * @brief �\���̂�null(�g�p���Ă��Ȃ����)�ł��邩���肷��
     * 
     * @return �\���̂�null�̏ꍇ��true��Ԃ�
     */
    inline bool IsNull() const {
      return index == kNullIndex;
    }

    /**
     * @brief �l���ꊇ�ŃZ�b�g����
     *
     * @param idx index�ɐݒ肷��l
     * @param ms millisecond�ɐݒ肷��l
     */
    inline void Set(std::uint32_t idx, std::uint32_t ms) {
      index = idx;
      millisecond = ms;
    }

    /**
     * @brief �\���̂�null(�g�p���Ă��Ȃ����)�ɂ���
     */
    inline void SetNull() {
      index = kNullIndex;
    }

    /**
     * @brief ��������v���邩���肷��
     *
     * @param destination ��r����
     * 
     * @return index��millisecond��������v�������v�����Ƃ���true��Ԃ�
     *         �ǂ��炩�A�������͗�����null�̏ꍇ�͈�v���Ă��Ȃ��Ɣ��肷��
     */
    inline bool Equals(const KeyEventTime& destination) const {
      return !IsNull() && !destination.IsNull() && index == destination.index && millisecond == destination.millisecond;
    }

    /**
     * @brief ���g��������O�����肷��
     *
     * @param destination ��r����
     * 
     * @return millisecond, index�̏��Ŕ�r�����g�̕����O�Ȃ�true��Ԃ�
     *         null�͖����刵���Ƃ���
     *         ������null�̏ꍇ��false��Ԃ�
     */
    inline bool Before(const KeyEventTime& destination) const {
      // null�͖����ő刵���Ȃ̂Ŏ��g��null�Ȃ�Ύ��g���O�ƂȂ邱�Ƃ͂Ȃ�
      if (IsNull()) {
        return false;
      }

      // null�͖����ő刵���Ȃ̂Ŏ��g��not null�ő��肪null�Ȃ�Ύ��g���O
      if (destination.IsNull()) {
        return true;
      }

      // ����not null�̏ꍇ��millisecond -> index�̏��ŏ����������O
      return millisecond < destination.millisecond || (millisecond == destination.millisecond && index < destination.index);
    }
  };

  /**
   * @brief �ݒ�
   */
  const Setting* const setting_;

  /**
   * @brief �L�[�{�[�h 
   */
  Keyboard* const keyboard_;

  /**
   * @brief �y���f�[�^(SMF)�ǂݍ��ݍς݃t���O
   * 
   * �t�@�C�����G���[�����ǂݍ��܂�A�X�L�������ă`�F�b�N���ςݎg�p�o�����Ԃł���ꍇ��true�ƂȂ�
   */
  bool file_loaded_ = false;

  /**
   * @brief �o�͐�
   */
  Output output_ = Output::kKeyboard;

  /**
   * @brief MIDI�����o�͏���
   */
  MidiSynthesizer midi_synthesizer_;

  /**
   * �y���f�[�^(SMF)
   */
  StandardMidiFormat standard_midi_format_;

  /**
   * @brief MIDI�̃m�[�gON�C�x���g��ǂݎ�邽�߂�Reader
   * 
   * �m�[�gON���m�̕��ѕ��ɂ��ϓ����鏈�������݂��邽�ߐ�p��Reader���K�v
   */
  StandardMidiFormatReader note_on_reader_;

  /**
   * @brief MIDI�̃m�[�gOFF�C�x���g��ǂݎ�邽�߂�Reader
   *
   * �m�[�gON�C�x���g��Reader�������������ߎc�肪�m�[�gOFF�C�x���g�݂̂ƂȂ���
   */
  StandardMidiFormatReader note_off_reader_;

  /**
   * @brief ���ɏ�������SMF�C�x���g
   * 
   * MIDI�����o�͂̏ꍇ�ł̂ݎg�p(�L�[�{�[�h�o�͂̏ꍇ�͕ʃf�[�^�ŊǗ�����̂�)
   */
  StandardMidiFormatEvent event_;

  /**
   * @brief �m�[�gON�pReader���ǂݎ�����C�x���g�� 
   */
  std::uint32_t note_on_reader_count_ = 0;

  /**
   * @brief �m�[�gOFF�pReader���ǂݎ�����C�x���g�� 
   */
  std::uint32_t note_off_reader_count_ = 0;

  /**
   * @brief ���t�J�n����
   * 
   * Windows�N������̌o�ߎ���(timeGetTime()�Ŏ���l)
   */
  std::uint32_t play_start_millisecond_ = 0;

  /**
   * @brief ���̃m�[�gON����
   */
  KeyEventTime note_on_time_;

  /**
   * @brief ���̃m�[�gOFF����
   */
  KeyEventTime note_off_time_;

  /**
   * @brief ����ON����m�[�g
   * 
   * note_on_time_��not null�̏ꍇ�̂ݗL��
   */
  std::uint8_t on_note_number_ = 0;

  /**
   * @brief ����OFF����m�[�g
   * 
   * note_off_time_��not null�̏ꍇ�̂ݗL��
   */
  std::uint8_t off_note_number_ = 0;

  /**
   * @brief �C���L�[�_�E������ 
   */
  KeyEventTime modifier_key_down_times_[kModifierKeyLength];

  /**
   * @brief �C���L�[�A�b�v���� 
   */
  KeyEventTime modifier_key_up_times_[kModifierKeyLength];

  /**
   * @brief �m�[�g���̍Ō��ON�ɂ��ꂽMIDI�C�x���g����
   * 
   * �m�[�gOFF��MIDI�C�x���g������葁�߂��ꍇ��ON���O�ɂȂ�Ȃ��悤�������邽�߂ɋL�^���Ă���
   */
  std::uint32_t last_note_on_milliseconds_[Setting::kMaximumNoteNumber - Setting::kMinimumNoteNumber];

  /**
   * @brief SMF�̃C�x���g���m�[�gON�Ƃ��Ĉ����������肷��
   * 
   * �ȉ��̏�����S�Ė������ꍇ�Ƀm�[�gON�Ƃ��Ĉ���
   * �E��ʂ�MIDI�C�x���g
   * �E�`�����l��0
   * �EMIDI�C�x���g��ʂ��m�[�gON�܂��̓|���t�H�j�b�N�L�[�v���b�V���[(�|���t�H�j�b�N�L�[�v���b�V���[�͏��O���Ă���������)
   * �E�x���V�e�B��0���傫��(�x���V�e�B0�̓m�[�gOFF�Ƃ��Ĉ�������)
   * �E�Ή�����m�[�g�ԍ��͈̔�
   * 
   * @param event ����Ώۂ̃C�x���g
   *
   * @return ����Ώۂ̃C�x���g���m�[�gON�Ƃ��Ĉ����ꍇ��true��Ԃ�
   */
  constexpr static bool StandardMidiFormatEventIsValidNoteOn(const StandardMidiFormatEvent& event) {
    return event.type == StandardMidiFormatEvent::Type::kMidi &&
           event.channel == 0 &&
           (event.midi_type == StandardMidiFormatEvent::MidiType::kNoteOn || event.midi_type == StandardMidiFormatEvent::MidiType::kPolyphonicKeyPressure) &&
           event.velocity > 0 &&
           Setting::kMinimumNoteNumber <= event.note_number && event.note_number <= Setting::kMaximumNoteNumber;
  }

  /**
   * @brief SMF�̃C�x���g���m�[�gOFF�Ƃ��Ĉ����������肷��
   * 
   * �ȉ��̏�����S�Ė������ꍇ�Ƀm�[�gOFF�Ƃ��Ĉ���
   * �E��ʂ�MIDI�C�x���g
   * �E�`�����l��0
   * �EMIDI�C�x���g��ʂ��m�[�gOFF�܂��̓x���V�e�B��0�̃m�[�gON
   * �E�Ή�����m�[�g�ԍ��͈̔�
   * 
   * @param event ����Ώۂ̃C�x���g
   *
   * @return ����Ώۂ̃C�x���g���m�[�gOFF�Ƃ��Ĉ����ꍇ��true��Ԃ�
   */
  constexpr static bool StandardMidiFormatEventIsValidNoteOff(const StandardMidiFormatEvent& event) {
    return event.type == StandardMidiFormatEvent::Type::kMidi &&
           event.channel == 0 &&
           (event.midi_type == StandardMidiFormatEvent::MidiType::kNoteOff || (event.midi_type == StandardMidiFormatEvent::MidiType::kNoteOn && event.velocity == 0)) &&
           Setting::kMinimumNoteNumber <= event.note_number && event.note_number <= Setting::kMaximumNoteNumber;
  }

  /**
   * @brief MIDI���t�o�͏��������s
   *
   * @param sleep_millisecond ���̉��t�����܂őҋ@���鎞��(�~���b)
   * 
   * @return �������p������ꍇ(���̏���������ꍇ)��true��Ԃ�
   *         (false��Ԃ����ꍇ�I�[�܂ŏ��������Ƃ�������)
   */
  bool RunMidiSynthesizerOutput(DWORD* sleep_millisecond);

  /**
   * @brief �L�[�{�[�h�o�͏��������s
   *
   * @param sleep_millisecond ���̉��t�����܂őҋ@���鎞��(�~���b)
   * 
   * @return �������p������ꍇ(���̏���������ꍇ)��true��Ԃ�
   *         (false��Ԃ����ꍇ�I�[�܂ŏ��������Ƃ�������)
   */
  bool RunKeyboardOutput(DWORD* sleep_millisecond);

  /**
   * @brief �L�[�{�[�h�o�͂̏ꍇ�Ɏ��̃m�[�gON��ǂݏo���ė\�肷��
   */
  void ScheduleKeyboardNoteOn();

  /**
   * @brief �L�[�{�[�h�o�͂̏ꍇ�Ɏ��̃m�[�gOFF��ǂݏo���ė\�肷��
   */
  void ScheduleKeyboardNoteOff();
};

}  // namespace keysequencer

#endif