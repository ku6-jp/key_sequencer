#ifndef KEYSEQUENCER_CONSTANTS_H_
#define KEYSEQUENCER_CONSTANTS_H_

#include <Windows.h>

#include "resource.h"

namespace keysequencer {
//----------------------------------------------------------------
// 以下はWin32 APIの定数として扱うものなので書式はそちらに寄せる

/**
 * @brief メインウィンドウに対して再生ステータス表示の更新を要求するメッセージ
 */
constexpr inline WORD WM_APP_REFRESH_STATUS = WM_APP;

/**
 * エラーダイアログ表示時に使用するメッセージボックスのスタイル
 */
constexpr inline UINT ERROR_DIALOG_TYPE = MB_OK | MB_ICONERROR;

// バージョン別で切り替えるリソースID
// Visual Studioのリソースエディタがリソースのプリプロセッサに対応していないようなので
// リソース自体には両バージョンの文字列を入れておき、IDを切り替えて使用する
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