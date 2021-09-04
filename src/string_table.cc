#include "string_table.h"

#include <cstddef>

namespace keysequencer {

StringTable::StringTable(HINSTANCE instance_handle) {
  // 1����������邽�߂̃o�b�t�@
  int string_buffer_length = 128;
  TCHAR* string_buffer = new TCHAR[string_buffer_length];

  // �S����������邽�߂ɕK�v�ȃT�C�Y���擾
  int buffer_length = 0;
  for (UINT i = RESOURCE_ID_START; i < RESOURCE_ID_START + RESOURCE_LENGTH; ++i) {
    for (;;) {
      int result = LoadString(instance_handle, i, string_buffer, string_buffer_length);

      // �ǂݍ��߂Ȃ�ID�̓X�L�b�v
      if (result == 0) {
        strings_[i - RESOURCE_ID_START] = nullptr;  // ���ł�nullptr�����������Ⴄ
        break;
      }

      // �o�b�t�@���ǂݍ��݃T�C�Y����������΂��ꂪ������̒���
      if (result < string_buffer_length - 1) {
        strings_[i - RESOURCE_ID_START] = string_buffer;  // ��ʂ��t���悤��nullptr�łȂ��K���Ȓl�����Ƃ�
        buffer_length += result + 1;
        break;
      }

      // �o�b�t�@��t�܂œǂݍ��܂�Ă�����o�b�t�@������Ȃ������\��������̂Ŋg�����ă��g���C
      delete[] string_buffer;
      string_buffer_length *= 2;
      string_buffer = new TCHAR[string_buffer_length];
    }
  }
  delete[] string_buffer;

  buffer_ = new TCHAR[buffer_length];

  int written = 0;
  for (UINT i = RESOURCE_ID_START; i < RESOURCE_ID_START + RESOURCE_LENGTH; ++i) {
    // �������ǂ߂����\�[�XID������ΏۂƂ���
    if (strings_[i - RESOURCE_ID_START] != nullptr) {
      int result = LoadString(instance_handle, i, &buffer_[written], buffer_length - written);
      if (result == 0) {
        strings_[i - RESOURCE_ID_START] = nullptr;
      } else {
        strings_[i - RESOURCE_ID_START] = &buffer_[written];
        written += result + 1;
      }
    }
  }
}

StringTable::~StringTable() {
  delete[] buffer_;
  delete[] strings_;
}

}  // namespace keysequencer