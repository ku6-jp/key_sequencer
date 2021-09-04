#ifndef KEYSEQUENCER_STANDARDMIDIFORMATEVENT_H_
#define KEYSEQUENCER_STANDARDMIDIFORMATEVENT_H_

#include <Windows.h>
#include <tchar.h>

#include <cstddef>
#include <cstdint>

namespace keysequencer {

/**
 * @brief SMF�Ɋ܂܂��C�x���g1���̏��
 */
struct StandardMidiFormatEvent {
 public:
  /**
   * @brief �C�x���g�̎��
   */
  enum class Type : std::uint8_t {
    /**
     * @brief MIDI�C�x���g 
     */
    kMidi,

    /**
     * @brief SysEx�C�x���g 
     */
    kSysEx,

    /**
     * @brief ���^�C�x���g 
     */
    kMeta,
  };

  /**
   * @brief MIDI�C�x���g�̎��
   *
   * SMF���̎��ۂ̃X�e�[�^�X�o�C�g����`�����l���ԍ��ł��鉺��4bit���N���A�����l���g�p����
   */
  enum class MidiType : std::uint8_t {
    /**
     * @brief �m�[�g�I�t
     */
    kNoteOff = 0x80,

    /**
     * @brief �m�[�g�I��
     */
    kNoteOn = 0x90,

    /**
     * @brief �|���t�H�j�b�N�L�[�v���b�V���[
     */
    kPolyphonicKeyPressure = 0xa0,

    /**
     * @brief �R���g���[���`�F���W
     */
    kControlChange = 0xb0,

    /**
     * @brief �v���O�����`�F���W
     */
    kProgramChange = 0xc0,

    /**
     * @brief �`�����l���v���b�V���[
     */
    kChannelPressure = 0xd0,

    /**
     * @brief �s�b�`�x���h
     */
    kPitchBend = 0xe0,
  };

  /**
   * @brief SysEx�C�x���g�̎��
   *
   * SMF���̎��ۂ̃X�e�[�^�X�o�C�g�̒l���g�p����
   */
  enum class SysExType : std::uint8_t {
    /**
     * @brief F0
     */
    kF0 = 0xf0,

    /**
     * @brief F7
     */
    kF7 = 0xf7,
  };

  /**
   * @brief ���^�C�x���g�̎��
   *
   * SMF���̎��ۂ̃��^�C�x���g�̒l���g�p����
   */
  enum class MetaType : std::uint8_t {
    /**
     * @brief �V�[�P���X�ԍ�
     */
    kSequenceNumber = 0x00,

    /**
     * @brief �e�L�X�g
     */
    kText = 0x01,

    /**
     * @brief ���쌠�\��
     */
    kCopyright = 0x02,

    /**
     * @brief �V�[�P���X��(�ȃ^�C�g��)�E�g���b�N��
     */
    kSequence = 0x03,

    /**
     * @brief �y�햼
     */
    kInstrumentName = 0x04,

    /**
     * @brief �̎�
     */
    kLyric = 0x05,

    /**
     * @brief �}�[�J�[
     */
    kMarker = 0x06,

    /**
     * @brief �L���[�|�C���g
     */
    kCuePoint = 0x07,

    /**
     * @brief �v���O������ (���F��) 
     */
    kProgramName = 0x08,

    /**
     * @brief �f�o�C�X�� (������) 
     */
    kDeviceName = 0x09,

    /**
     * @brief MIDI�`�����l���v���t�B�b�N�X
     */
    kMidiChannelPrefix = 0x20,

    /**
     * @brief �|�[�g�w��
     */
    kMidiPort = 0x21,

    /**
     * @brief �g���b�N�I�[
     */
    kEndOfTrack = 0x2f,

    /**
     * @brief �e���|�ݒ�
     */
    kTempo = 0x51,

    /**
     * @brief SMPTE�I�t�Z�b�g
     */
    kSmpteOffset = 0x54,

    /**
     * @brief ���q�̐ݒ�
     */
    kTimeSignature = 0x58,

    /**
     * @brief ���̐ݒ�
     */
    kKeySignature = 0x59,

    /**
     * @brief �V�[�P���T���胁�^�C�x���g
     */
    kSequencerSpecificEvent = 0x7f,
  };

  /**
   * @brief �C�x���g�����s����Đ��J�n����̎���(�}�C�N���b)
   * 
   * ���ڃC�x���g�Ɋ܂܂��f�[�^�ł͂Ȃ����A�ǂݎ�莞�Ɍv�Z�������ʂ��Z�b�g����
   */
  std::uint64_t microsecond;

  /**
   * @brief �g���b�N�ԍ�
   */
  std::uint16_t track_number;

  /**
   * @brief �C�x���g�̎��
   */
  Type type;

  // �C�x���g��ނɂ���Ďg�p����f�[�^���r���I�ɕς��̂�union���g�p
  union {
    //  MIDI�C�x���g�Ŏg�p����l
    struct {
      /**
       * @brief MIDI�C�x���g�̎��
       */
      MidiType midi_type;

      /**
       * @brief �Ώۂ̃`�����l��
       */
      std::uint8_t channel;

      // MIDI�C�x���g�̎�ނɂ���Ă��g�p����f�[�^���r���I�ɕς��̂ł����union���g�p
      union {
        /**
         * @brief �s�b�`�x���h
         */
        std::uint16_t pitch_bend;

        struct {
          /**
           * @brief �m�[�g�ԍ�
           *
           * �����鉹�K
           */
          std::uint8_t note_number;

          /**
           * @brief �x���V�e�B
           *
           * ���Ղ��������藣�����肷�鎞�̑��x���
           * ��{�I�ɂ͉��ʓI�ȈӖ�
           */
          std::uint8_t velocity;
        };

        struct {
          /**
           * @brief �R���g���[���ԍ�
           *
           * �R���g���[���`�F���W�C�x���g�̏ꍇ�ɑΏۂ̃R���g���[���ԍ����Z�b�g�����
           * �`�����l�����[�h���b�Z�[�W�̏ꍇ���b�Z�[�W�ԍ�(0x78-0x7f)������
           */
          std::uint8_t control_number;

          /**
           * @brief �R���g���[���`�F���W�̒l
           */
          std::uint8_t control_value;
        };

        /**
         * @brief �v���O�����`�F���W�̒l
         *
         * �M�^�[�Ƃ��s�A�m�Ƃ��݂����ȉ��F�̎w��
         */
        std::uint8_t program;
      };
    };

    /**
     * @brief SysEx�C�x���g
     */
    struct {
      /**
       * @brief SysEx�C�x���g�̎��
       */
      SysExType sys_ex_type;

      /**
       * @brief �]���f�[�^��
       *
       * data�Ɋi�[����Ă���f�[�^�T�C�Y
       */
      std::uint32_t data_length;

      /**
       * @brief �f�[�^�̒��g
       *
       * �f�[�^�T�C�Y��dataLength���Q��
       * �f�[�^�T�C�Y��0�̏ꍇ��nullptr�ƂȂ�
       */
      const std::uint8_t* data;
    };

    /**
     * @brief ���^�C�x���g
     */
    struct {
      /**
       * @brief ���^�C�x���g�̎��
       */
      MetaType meta_type;

      /**
       * @brief �f�[�^��
       *
       * data�Ɋi�[����Ă���f�[�^�T�C�Y
       */
      std::uint32_t data_length;

      /**
       * @brief �f�[�^�̒��g
       *
       * �f�[�^�T�C�Y��dataLength���Q��
       * �f�[�^�T�C�Y��0�̏ꍇ��nullptr�ƂȂ�
       */
      const std::uint8_t* data;
    };
  };

  /**
   * @brief �R���X�g���N�^
   *
   * ��������̏�Ԃ��̂܂܂Ŏg�����Ƃ͖����z��ł��邪�ꉞ�m�[�gOFF�ŏ���������
   */
  StandardMidiFormatEvent() : microsecond(0), track_number(0), type(Type::kMidi), midi_type(MidiType::kNoteOff), channel(0), pitch_bend(0), data_length(0), data(nullptr) {
  }
};
}  // namespace keysequencer
#endif