#ifndef KEYSEQUENCER_STANDARDMIDIFORMAT_H_
#define KEYSEQUENCER_STANDARDMIDIFORMAT_H_

#include <tchar.h>

#include <cstddef>
#include <cstdint>

namespace keysequencer {

/**
 * @brief SMF�t�@�C���̃f�[�^��ێ�����
 * 
 * ��͏����͕ʃN���X�ɂȂ��Ă���̂ł��̃N���X�̓t�@�C���̃o�C�i���f�[�^��ێ�����̂�
 */
class StandardMidiFormat {
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
  };

  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  StandardMidiFormat() = default;

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  StandardMidiFormat(const StandardMidiFormat&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  StandardMidiFormat(StandardMidiFormat&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  StandardMidiFormat& operator=(const StandardMidiFormat&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  StandardMidiFormat& operator=(StandardMidiFormat&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~StandardMidiFormat();

  /**
   * @brief �t�@�C����ǂݍ���
   * 
   * �ǂݍ��݂̐����E���s�ɌW��炸�ێ����̃f�[�^�͔j������
   *
   * @param filepath �t�@�C���p�X
   *
   * @return �t�@�C���ǂݍ��݌���
   */
  Result LoadFile(const TCHAR* filepath);

  /**
   * @brief �ǂݍ��񂾃o�C�i���f�[�^���擾
   * 
   * @return �o�C�i���f�[�^�̐擪�|�C���^
   *         LoadFile���Ăяo���Ɩ����ƂȂ�
   */
  inline const std::uint8_t* GetData() const {
    return data_;
  }

  /**
   * @brief �ǂݍ��񂾃o�C�i���f�[�^�̃T�C�Y���擾 
   * 
   * @return �o�C�i���f�[�^�̃T�C�Y(Byte)
   */
  inline std::size_t GetLength() const {
    return length_;
  }

 private:
  /**
   * @brief �o�C�i���f�[�^
   */
  std::uint8_t* data_ = nullptr;

  /**
   * @brief �o�C�i���f�[�^�̒���
   */
  std::size_t length_ = 0;
};

}  // namespace keysequencer
#endif