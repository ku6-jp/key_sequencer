#ifndef KEYSEQUENCER_STANDARDMIDIFORMATREADER_H_
#define KEYSEQUENCER_STANDARDMIDIFORMATREADER_H_

#include <cstddef>
#include <cstdint>

namespace keysequencer {

struct StandardMidiFormatEvent;

/**
 * @brief SMF�ǂݎ�菈�� 
 * 
 * SMF�o�C�i���f�[�^����SMF�C�x���g�Ƃ��ēǂݎ��
 */
class StandardMidiFormatReader {
 public:
  /**
   * @brief ���쌋��
   */
  enum class Result {
    /**
     * @brief ����I�� 
     */
    kSuccess,

    /**
     * @brief ��������������Ă��Ȃ����ߑ���ł��Ȃ� 
     */
    kFailureUninitialized,

    /**
     * @brief �I�[�ɒB�������ߓǂݏo���Ȃ�
     */
    kFailureEndOfFile,

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
  StandardMidiFormatReader() = default;

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  StandardMidiFormatReader(const StandardMidiFormatReader&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  StandardMidiFormatReader(StandardMidiFormatReader&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  StandardMidiFormatReader& operator=(const StandardMidiFormatReader&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  StandardMidiFormatReader& operator=(StandardMidiFormatReader&&) = delete;

  /**
   * @brief �f�X�g���N�^ 
   */
  ~StandardMidiFormatReader();

  /**
   * @brief SMF�o�C�i���f�[�^��n���ēǂݎ�菈��������������
   * 
   * @param start SMF�o�C�i���f�[�^
   * @param length SMF�o�C�i���f�[�^�̒���
   * 
   * @return �ǂݎ�茋��
   */
  Result Initialize(const std::uint8_t* const start, std::size_t const length);

  /**
   * @brief ����SMF�C�x���g��1�ǂݎ��
   *
   * �����ɓǂݎ��J�[�\�����ړ�����
   * 
   * @param event �ǂݎ�茋�ʂ��������ލ\���̂̃|�C���^
   *              �ǂݎ�茋�ʂ�����I���̏ꍇ�ȊO��event�ɔ��[�ȓ��e���������ނ��ߎQ�Ƃ��Ȃ�����
   * 
   * @return �ǂݎ�茋��
   */
  Result Next(StandardMidiFormatEvent* event);

  /**
   * @brief �ǂݎ��ʒu��擪�ɖ߂�
   */
  Result Rewind();

 private:
  /**
   * @brief �t�H�[�}�b�g�^�C�v
   *
   * ������t�H�[�}�b�g2�Ƃ����̂��K�i�㑶�݂���̂���
   * �g�p���邱�Ƃ��H�ł���\�����C�}�C�`�悭�킩���̂Ō����_�ł͔�Ή��Ƃ���
   */
  enum class Format : std::uint16_t {
    kType0 = 0,  // �t�H�[�}�b�g0: 1�g���b�N�ɑS�ẴC�x���g���܂Ƃ߂��`��
    kType1 = 1,  // �t�H�[�}�b�g1: �������t����镡���̃g���b�N�ō\�������
  };

  /**
   * @brief �g���b�N�Ǐo���p�f�[�^
   */
  struct TrackReader {
   public:
    /**
     * @brief �g���b�N�J�n�ʒu
     *
     * �g���b�N�ŏ��̃��b�Z�[�W�̐擪�o�C�g�̃|�C���^
     * MTrk��g���b�N�f�[�^���̈ʒu�ł͂Ȃ�
     */
    const std::uint8_t* start;

    /**
     * @brief �g���b�N�I���ʒu
     *
     * �g���b�N�G���h�̍ŏI�o�C�g�̎��̃|�C���^
     * start + �g���b�N�f�[�^��
     */
    const std::uint8_t* end;

    /**
     * @brief �Ǐo���J�[�\��
     *
     * �f���^�^�C���͗\�ߓǂݎ���Ă����̂ŁA�f���^�^�C���̎��ɂ���C�x���g�f�[�^�̊J�n�ʒu���w��
     * (�I�[�ɒB�����ꍇ�͏I�[���w��)
     */
    const std::uint8_t* cursor;

    /**
     * @brief
     * 
     * �����j���O�X�e�[�^�X�̂��߂�1�O��MIDI�C�x���g�̃X�e�[�^�X�o�C�g���L�^���Ă���
     */
    std::uint8_t last_midi_event_status;

    /**
     * @brief �Ǐo���J�[�\�����w���C�x���g�̃f���^�^�C��
     *
     * ���߂̃��b�Z�[�W���������邽�߂ɕ�����Q�Ƃ���̂Ńf�R�[�h�̏�ێ����Ă���
     */
    std::uint32_t delta_time;

    /**
     * @brief �Ǐo���J�[�\�����O�̃C�x���g�̍Đ��J�n����̌o�ߎ���
     *
     * �C�x���g�̃\�[�g�̂��߂Ɍo�ߎ��Ԃ�ێ����Ă���(�P��: �}�C�N���b)
     */
    std::uint64_t last_event_microsecond;
  };

  /**
   * @brief �t�H�[�}�b�g
   */
  Format format_ = Format::kType0;

  /**
   * @brief �l�������̕���\
   *
   * �^�C���x�[�X���e���|�x�[�X�̏ꍇ�Ɏl�������̕���\������(�P��: Tick)
   * �e�C�x���g�̃f���^�^�C���͂��̐����ɑ΂��銄��������
   *
   * ��:
   * �^�C���x�[�X(�l�������̕���\)��480Tick�̏ꍇ�A�f���^�^�C�����S�����Ȃ�1960Tick�A���������Ȃ�240Tick�ƂȂ�)
   * ����������Ԃɂ��邽�߂ɂ̓e���|���K�v�ƂȂ�A�ȉ��̌v�Z���ŋ��߂���
   * �f���^�^�C��(�}�C�N���b) =
   * (�f���^�^�C��(Tick) / �^�C���x�[�X(Tick)) *  �e���|�ݒ�(�}�C�N���b/Tick)
   */
  std::uint16_t quarter_note_tick_ = 0;

  /**
   * @brief �g���b�N��
   *
   * �t�H�[�}�b�g0�̏ꍇ��1�Œ� ����ȊO�̏ꍇ�̓g���b�N��
   */
  std::uint16_t track_ = 0;

  /**
   * @brief �g���b�N�Ǐo���f�[�^(1�g���b�N�ɂ�1��)
   */
  TrackReader* track_readers_ = nullptr;

  /**
   * @brief �ǂݎ��J�[�\���ʒu�ł�OMNI���[�h��ON/OFF
   *
   * Next�֐����̌Ăяo�����ʂɂ������I�ɃX�e�[�^�X���ϓ�����
   */
  bool omni_ = false;

  /**
   * @brief �ǂݎ��J�[�\���ʒu�ł�MONO���[�h��ON/OFF
   *
   * Next�֐����̌Ăяo�����ʂɂ������I�ɃX�e�[�^�X���ϓ�����
   */
  bool mono_ = false;

  /**
   * @brief �ǂݎ��J�[�\���ʒu�ł̃e���|
   *
   * Next�֐����̌Ăяo�����ʂɂ������I�ɃX�e�[�^�X���ϓ�����
   */
  std::uint32_t tempo_ = 0;

  /**
   * @brief 2Byte�����l��ǂݎ��
   *
   * @param p  �ǂݎ��ʒu�̃|�C���^(�ǂݎ���̈ʒu�Ɉړ�����)
   *
   * @return �ǂݎ���������l
   */
  constexpr static std::uint16_t ReadUint16(const std::uint8_t*& p) {
    std::uint16_t result = static_cast<std::uint16_t>(*p++) << 8;
    return result | static_cast<std::uint16_t>(*p++);
  }

  /**
   * @brief 4Byte�����l��ǂݎ��
   *
   * @param p �ǂݎ��ʒu�̃|�C���^(�ǂݎ���̈ʒu�Ɉړ�����)
   *
   * @return �ǂݎ���������l
   */
  constexpr static std::uint32_t ReadUint32(const std::uint8_t*& p) {
    std::uint32_t result = static_cast<std::uint32_t>(*p++) << 8;
    result = (result | static_cast<std::uint32_t>(*p++)) << 8;
    result = (result | static_cast<std::uint32_t>(*p++)) << 8;
    return result | static_cast<std::uint32_t>(*p++);
  }

  /**
   * @brief �ϒ������l��ǂݎ��
   * 
   * ���܂���Byte����ǂݎ��ꍇ�͌Ăяo�����œǂݎ��\�ȃf�[�^���Ɏ��܂邩�����O�ɔ��f�o���邪
   * �ϒ������̏ꍇ�͓ǂ�ł݂Ȃ��ƕ�����Ȃ����߁A�ǂݎ��\���̃`�F�b�N���s��
   *
   * @param cursor �ǂݎ��ʒu�̃|�C���^(�ǂݎ���̈ʒu�Ɉړ�����)
   * @param end �ǂݎ��\�ȃf�[�^�̏I�[
   * @param destination �ǂݎ�����f�[�^�������Ɋi�[�����
   *
   * @return �ǂݎ�茋��
   */
  constexpr static bool ReadVariableUint(const std::uint8_t*& cursor, const std::uint8_t* const end, std::uint32_t& destination) {
    std::uint32_t value = 0;
    for (std::size_t i = 0; i < 4; ++i) {
      // �ǂݎ��I�[�ɒB������f�[�^����������
      if (cursor + i >= end) {
        return false;
      }

      // ����7bit�̃f�[�^���擾
      value |= static_cast<std::uint32_t>(cursor[i]) & 0x7f;

      if ((cursor[i] & 0x80) == 0) {
        // �ŏ��bit�������Ă��Ȃ���Γǂݎ��͂����܂�

        // �ǂݎ�������|�C���^���ړ�����
        cursor += i + 1;

        // �ǂݎ�����l��ݒ�
        destination = value;

        return true;
      } else {
        // �ŏ��bit�������Ă���Ύ�������͂��Ȃ̂ł��̕��󂯂Ă���
        value <<= 7;
      }
    }

    // �Œ�4Byte�Ȃ̂ł���ȏ゠������f�[�^����������
    return false;
  }
};

}  // namespace keysequencer

#endif