#include "setting.h"

#include <Shlwapi.h>

#include <cstdio>

namespace keysequencer {

Setting::Setting() {
  for (std::uint32_t i = 0; i < MAX_PATH; ++i) {
    ini_file_path_[i] = 0;
  }

  for (std::uint32_t i = 0; i < KeyBindIndex::kLength; ++i) {
    key_binds_[i] = 0;
  }
}

bool Setting::LoadIniFile() {
  TCHAR buffer[8];

  // �ǂݍ��݊�������Ɏg����悤�N���A����(�ꉞ�S�̂�)
  for (std::uint32_t i = 0; i < MAX_PATH; ++i) {
    ini_file_path_[i] = 0;
  }

  // ���s�t�@�C���̃t���p�X�����擾����
  if (GetModuleFileName(NULL, ini_file_path_, MAX_PATH) == 0) {
    return false;
  }

  // �����t�H���_�ɓ������O�Ŋg���q����.ini�ɕς���
  if (!PathRenameExtension(ini_file_path_, _T(".ini"))) {
    return false;
  }

  if (PathFileExists(ini_file_path_)) {
    // .ini�t�@�C�������݂���ꍇ�͂����ǂݏo��
    for (std::size_t i = 0; i < KeyBindIndex::kLength; ++i) {
      // .ini�t�@�C������0x0000�`����6������ǂݎ��
      // 6���� + NULL����1�����ɉ�����6�����ȏ�̕����񌟏o���邽�߂ɂ����1�����]���Ɋm�ۂ�8�������Ƃ���
      if (GetPrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[i], _T(""), buffer, sizeof(buffer) / sizeof(buffer[0]), ini_file_path_) != 6) {
        key_binds_[i] = 0;
        continue;
      }

      // �����񂩂琮���ɕϊ�
      TCHAR* end;
      KeyBind result = static_cast<KeyBind>(_tcstoul(buffer, &end, 16));
      if (*end == 0) {
        // .ini�t�@�C���̓��[�U�[�����������o����̂ňꉞ���[�h�����f�[�^���`�F�b�N����

        // ���1�o�C�g�ɃS�~���������Ȃ��悤�ꉞ�N���A����
        result &= kShiftKeyBind | kControlKeyBind | kAlternateKeyBind | 0x00ff;

        // �����L�[���g�p�ς݂Ȃ�΃N���A���Ă��܂�
        for (std::size_t j = 0; j < KeyBindIndex::kLength; ++j) {
          if (key_binds_[j] == result) {
            key_binds_[j] = 0;
          }
        }

        key_binds_[i] = result;
      } else {
        key_binds_[i] = 0;
      }
    }

    // �m�[�g�̕\�L������.ini�t�@�C������擾
    if (GetPrivateProfileInt(kOptionalIniSection, kNoteLabelStyleIniKey, 0, ini_file_path_) == static_cast<int>(NoteLabelStyle::kYamaha)) {
      note_label_style_ = NoteLabelStyle::kYamaha;
    } else {
      note_label_style_ = NoteLabelStyle::kEnglish;

      // �m�[�g�\�L������Optional���ݒ��ʂŕύX�o����̂�.ini�t�@�C���ɃL�[�������Ă��C�ɂ��Ȃ�
    }

    // �L�[�𗣂��^�C�~���O��.ini�t�@�C������ǂݎ��
    early_key_release_millisecond_ = GetPrivateProfileInt(kOptionalIniSection, kEarlyKeyReleaseMillisecondIniKey, 0, ini_file_path_);

    // .ini�t�@�C���ɃL�[���o�͂��邽�߂ɏ�������(���[�U�[���ҏW�o����悤��)
    // �f�t�H���g�l�łȂ���Ί��ɏ�������ł���̂ŉ������Ȃ�
    if (early_key_release_millisecond_ == 0) {
      WritePrivateProfileString(kOptionalIniSection, kEarlyKeyReleaseMillisecondIniKey, _T("0"), ini_file_path_);
    }

    // �L�[�𗣂��^�C�~���O��.ini�t�@�C������ǂݎ��
    modifier_key_delay_millisecond_ = GetPrivateProfileInt(kOptionalIniSection, kModifierKeyDelayMillisecondIniKey, 50, ini_file_path_);

    // .ini�t�@�C���ɃL�[���o�͂��邽�߂ɏ�������(���[�U�[���ҏW�o����悤��)
    // �f�t�H���g�l�łȂ���Ί��ɏ�������ł���̂ŉ������Ȃ�
    if (modifier_key_delay_millisecond_ == 50) {
      WritePrivateProfileString(kOptionalIniSection, kModifierKeyDelayMillisecondIniKey, _T("50"), ini_file_path_);
    }

    // ����C���L�[��������ɂ܂Ƃ߂鎞�Ԃ�.ini�t�@�C������ǂݎ��
    modifier_key_down_join_millisecond_ = GetPrivateProfileInt(kOptionalIniSection, kModifierKeyDownJoinMillisecondIniKey, 1000, ini_file_path_);

    // .ini�t�@�C���ɃL�[���o�͂��邽�߂ɏ�������(���[�U�[���ҏW�o����悤��)
    // �f�t�H���g�l�łȂ���Ί��ɏ�������ł���̂ŉ������Ȃ�
    if (modifier_key_down_join_millisecond_ == 1000) {
      WritePrivateProfileString(kOptionalIniSection, kModifierKeyDownJoinMillisecondIniKey, _T("1000"), ini_file_path_);
    }
  } else {
    // ���݂��Ȃ��ꍇ�̓f�t�H���g�̐ݒ���g�p����(�����Ƀf�t�H���g�ݒ��.ini�t�@�C�����쐬����)

    // �f�t�H���g�̃L�[�o�C���h���Z�b�g(�Ə�������)
    for (std::size_t i = 0; i < KeyBindIndex::kLength; ++i) {
      key_binds_[i] = kDefaultKeyBinds[i];

      // .ini�t�@�C���ɏ�������
      _stprintf_s(buffer, sizeof(buffer) / sizeof(buffer[0]), _T("0x%04x"), key_binds_[i]);
      WritePrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[i], buffer, ini_file_path_);
    }

    // �m�[�g�\�L�����̃f�t�H���g�l
    // �m�[�g�\�L������Optional���ݒ��ʂŕύX�o����̂�.ini�t�@�C���ɃL�[�������Ă��C�ɂ��Ȃ�
    note_label_style_ = NoteLabelStyle::kEnglish;

    // �m�[�gOFF�C�x���g������艽�~���b�����L�[�𗣂����̃f�t�H���g�l��ݒ�
    early_key_release_millisecond_ = 0;

    // .ini�t�@�C���ɃL�[���o�͂��邽�߂ɏ�������(���[�U�[���ҏW�o����悤��)
    // �f�t�H���g�l�łȂ���Ί��ɏ�������ł���̂ŉ������Ȃ�
    WritePrivateProfileString(kOptionalIniSection, kEarlyKeyReleaseMillisecondIniKey, _T("0"), ini_file_path_);

    // �C���L�[���m�[�gON���牽�~���b�����ăA�b�v�_�E�����邩�̃f�t�H���g�l��ݒ�(50ms = 20fps�ł��E����l)
    modifier_key_delay_millisecond_ = 50;

    // .ini�t�@�C���ɃL�[���o�͂��邽�߂ɏ�������(���[�U�[���ҏW�o����悤��)
    // �f�t�H���g�l�łȂ���Ί��ɏ�������ł���̂ŉ������Ȃ�
    WritePrivateProfileString(kOptionalIniSection, kModifierKeyDelayMillisecondIniKey, _T("50"), ini_file_path_);

    // ����C���L�[��������ɂ܂Ƃ߂鎞�Ԃ̃f�t�H���g�l��ݒ�
    modifier_key_down_join_millisecond_ = 1000;

    // .ini�t�@�C���ɃL�[���o�͂��邽�߂ɏ�������(���[�U�[���ҏW�o����悤��)
    // �f�t�H���g�l�łȂ���Ί��ɏ�������ł���̂ŉ������Ȃ�
    WritePrivateProfileString(kOptionalIniSection, kModifierKeyDownJoinMillisecondIniKey, _T("1000"), ini_file_path_);
  }

  return true;
}

void Setting::SetKeyBind(std::size_t index, KeyBind key_bind) {
  if (ini_file_path_[0] == 0) {
    return;
  }

  if (key_bind != 0) {
    // ���1�o�C�g�ɃS�~���������Ȃ��悤�ꉞ�N���A����
    key_bind &= kShiftKeyBind | kControlKeyBind | kAlternateKeyBind | 0x00ff;

    // �����L�[���g�p�ς݂Ȃ�΃N���A���Ă��܂�
    for (std::size_t i = 0; i < KeyBindIndex::kLength; ++i) {
      if (key_binds_[i] == key_bind) {
        key_binds_[i] = 0;

        // .ini�t�@�C���ɏ�������
        WritePrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[i], _T("0"), ini_file_path_);
      }
    }
  }

  // �V�����ݒ���Z�b�g
  key_binds_[index] = key_bind;

  // �������當����ɕϊ�
  TCHAR buffer[7];
  _stprintf_s(buffer, sizeof(buffer) / sizeof(buffer[0]), _T("0x%04x"), key_bind);

  // .ini�t�@�C���ɏ�������
  WritePrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[index], buffer, ini_file_path_);
}

void Setting::SetNoteLabelStyle(NoteLabelStyle note_label_style) {
  if (ini_file_path_[0] == 0) {
    return;
  }

  note_label_style_ = note_label_style;

  if (note_label_style_ == NoteLabelStyle::kEnglish) {
    WritePrivateProfileString(kOptionalIniSection, kNoteLabelStyleIniKey, _T("0"), ini_file_path_);
  } else {
    WritePrivateProfileString(kOptionalIniSection, kNoteLabelStyleIniKey, _T("1"), ini_file_path_);
  }
}

}  // namespace keysequencer