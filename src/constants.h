#ifndef KEYSEQUENCER_CONSTANTS_H_
#define KEYSEQUENCER_CONSTANTS_H_

#include <Windows.h>

#include "resource.h"

namespace keysequencer {
//----------------------------------------------------------------
// �ȉ���Win32 API�̒萔�Ƃ��Ĉ������̂Ȃ̂ŏ����͂�����Ɋ񂹂�

/**
 * @brief ���C���E�B���h�E�ɑ΂��čĐ��X�e�[�^�X�\���̍X�V��v�����郁�b�Z�[�W
 */
constexpr inline WORD WM_APP_REFRESH_STATUS = WM_APP;

/**
 * �G���[�_�C�A���O�\�����Ɏg�p���郁�b�Z�[�W�{�b�N�X�̃X�^�C��
 */
constexpr inline UINT ERROR_DIALOG_TYPE = MB_OK | MB_ICONERROR;

// �o�[�W�����ʂŐ؂�ւ��郊�\�[�XID
// Visual Studio�̃��\�[�X�G�f�B�^�����\�[�X�̃v���v���Z�b�T�ɑΉ����Ă��Ȃ��悤�Ȃ̂�
// ���\�[�X���̂ɂ͗��o�[�W�����̕���������Ă����AID��؂�ւ��Ďg�p����
#ifdef KEY_SEQUENCER_FULL_NOTE
constexpr inline int IDS_ERRORDIALOG_CAPTION = IDS_ERRORDIALOG_CAPTION_FULL_NOTE;
constexpr inline int IDS_MAINWINDOW_CAPTION = IDS_MAINWINDOW_CAPTION_FULL_NOTE;
constexpr inline int IDS_KEYBINDWINDOW_CAPTION = IDS_KEYBINDWINDOW_CAPTION_FULL_NOTE;
#else
constexpr inline int IDS_ERRORDIALOG_CAPTION = IDS_ERRORDIALOG_CAPTION_NORMAL;
constexpr inline int IDS_MAINWINDOW_CAPTION = IDS_MAINWINDOW_CAPTION_NORMAL;
constexpr inline int IDS_KEYBINDWINDOW_CAPTION = IDS_KEYBINDWINDOW_CAPTION_NORMAL;
#endif

}  // namespace keysequencer

#endif