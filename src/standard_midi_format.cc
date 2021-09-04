#include "standard_midi_format.h"

#include <Windows.h>

namespace keysequencer {

StandardMidiFormat::~StandardMidiFormat() {
  if (data_ != nullptr) {
    delete[] data_;
  }
}

StandardMidiFormat::Result StandardMidiFormat::LoadFile(const TCHAR* const filepath) {
  // ���ݓǂݍ���ł���f�[�^��j��
  if (data_ != nullptr) {
    delete[] data_;
  }
  data_ = nullptr;
  length_ = 0;

  // �t�@�C�����J��
  HANDLE handle = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    return Result::kFailureFileOpen;
  }

  // �t�@�C���T�C�Y���擾
  LARGE_INTEGER filesize;
  if (!GetFileSizeEx(handle, &filesize)) {
    CloseHandle(handle);
    return Result::kFailureFileOpen;  // �T�C�Y�����Ȃ��Ƃ��I�[�v�����s�����ł���
  }

  // 4GB�ȏ�̃t�@�C���ɂ͔�Ή�
  // 32bit�ł��Ƃ����Ə������T�C�Y�ł��������m�ۃG���[�ɂȂ邯��
  if (filesize.QuadPart > 0x00000000ffffffff) {
    CloseHandle(handle);
    return Result::kFailureFileTooLarge;
  }

  // �T�C�Y��ۑ�
  length_ = static_cast<std::size_t>(filesize.LowPart);

  // ���������m��
  data_ = new std::uint8_t[length_];

  // �t�@�C����ǂݍ���
  DWORD read;
  BOOL result = ReadFile(handle, data_, filesize.LowPart, &read, nullptr);
  CloseHandle(handle);
  if (!result || filesize.LowPart != read) {
    return Result::kFailureFileRead;
  }

  return Result::kSuccess;
}

}  // namespace keysequencer