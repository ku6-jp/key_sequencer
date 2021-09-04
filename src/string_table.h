#ifndef KEYSEQUENCER_STRINGTABLE_H_
#define KEYSEQUENCER_STRINGTABLE_H_

#include <Windows.h>
#include <tchar.h>

namespace keysequencer {

/**
 * @brief ���\�[�X�̕�����e�[�u���擾����
 */
class StringTable {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  StringTable() = delete;

  /**
   * @brief �R���X�g���N�^
   */
  StringTable(HINSTANCE instance_handle);

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  StringTable(const StringTable&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  StringTable(StringTable&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  StringTable& operator=(const StringTable&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  StringTable& operator=(StringTable&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~StringTable();

  /**
   * @brief �w��ID�̕�������擾
   *
   * @param resource_id ���\�[�XID
   *
   * @return ������̃o�b�t�@
   */
  inline const TCHAR* get(int resource_id) const {
    if (resource_id < RESOURCE_ID_START || RESOURCE_ID_START + RESOURCE_LENGTH <= resource_id) {
      return nullptr;
    }

    if (strings_[resource_id - RESOURCE_ID_START] == nullptr) {
      return nullptr;
    } else {
      return strings_[resource_id - RESOURCE_ID_START];
    }
  }

 private:
  /**
   * @brief StringTable���̃��\�[�X�̊J�nID
   *
   * StringTable���̃��\�[�X�͂���ID����A�ԂƂȂ��Ă���O��ŏ�������
   */
  constexpr static int RESOURCE_ID_START = 1001;

  /**
   * @brief StringTable���̃��\�[�X�̌�
   *
   * ����Ȃ��ƃ��\�[�X���ǂ߂Ȃ��Ȃ邪�]�镪�ɂ͍\��Ȃ�
   */
  constexpr static int RESOURCE_LENGTH = 192;

  /**
   * @brief ���\�[�XID���̕�����̐擪�|�C���^
   * 
   * �Z�b�g����镶����̎��̂�buffer_�Ɋi�[����̂ŌʂɃ��������������K�v�͖���
   */
  TCHAR** const strings_ = new TCHAR*[RESOURCE_LENGTH];

  /**
   * @brief ������Ǐo���p�̃o�b�t�@
   */
  TCHAR* buffer_ = nullptr;
};

}  // namespace keysequencer

#endif