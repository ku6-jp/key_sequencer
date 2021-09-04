#include "key_bind_window.h"

#include <cstdio>

#include "main_window.h"
#include "string_table.h"

namespace keysequencer {

KeyBindWindow::KeyBindWindow(HINSTANCE instance_handle, HWND parent_window_handle, const StringTable* string_table, Setting* setting) : instance_handle_(instance_handle), parent_window_handle_(parent_window_handle), string_table_(string_table), setting_(setting) {
  static int static_initialize = RegisterWindowClass(instance_handle);

  // 動作に必須というわけではないが一応初期化しておく
  for (std::size_t i = 0; i < Setting::KeyBindIndex::kLength; ++i) {
    label_text_handles_[i] = nullptr;
    key_text_handles_[i] = nullptr;
    change_button_handles_[i] = nullptr;
    commit_button_handles_[i] = nullptr;
    cancel_button_handles_[i] = nullptr;
  }

  // 構造体のサイズだけは固定なのでここで設定しておく
  scroll_info_.cbSize = sizeof(SCROLLINFO);
}

bool KeyBindWindow::Create() {
  if (created_) {
    return false;
  }
  created_ = true;

  // 作成するウィンドウのスタイル
  DWORD window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VSCROLL;

  // クライアント領域からウィンドウサイズを計算
  int width = kClientPadding + kNameTextWidth + kControlMarginRight + kKeyTextWidthH + kControlMarginRight + kCommitButtonWidth + kControlMarginRight + kCancelButtonWidth + kClientPadding;
  int height = kClientPadding + (kLineHeight * kLinesOnPage) + (kControlMarginBottom * (kLinesOnPage - 1)) + kClientPadding;
  RECT client_rect = {0, 0, width, height};
  if (AdjustWindowRect(&client_rect, window_style, FALSE)) {
    width = client_rect.right - client_rect.left + GetSystemMetrics(SM_CXVSCROLL);
    height = client_rect.bottom - client_rect.top;
  }

  // 画面中央を初期位置とする
  int x = GetSystemMetrics(SM_CXSCREEN);
  int y = GetSystemMetrics(SM_CYSCREEN);
  x = x > width ? (x - width) / 2 : 0;
  y = y > height ? (y - height) / 2 : 0;

  // Window作成
  window_handle_ = CreateWindowEx(0, kWindowClassName, string_table_->get(IDS_KEYBINDWINDOW_CAPTION), window_style, x, y, width, height, nullptr, nullptr, instance_handle_, nullptr);
  if (window_handle_ == nullptr) {
    MessageBox(parent_window_handle_, string_table_->get(IDS_ERROR_CREATE_WINDOW), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
    return false;
  }

  // Window procedure用にthisポインタを記憶
  SetWindowLongPtr(window_handle_, GWLP_USERDATA, 0);
  if (SetWindowLongPtr(window_handle_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)) == 0 && GetLastError() != 0) {
    goto DESTROY;
  }

  // IMEを無効化する
  //
  // 機能的にもIMEが動作すると邪魔であるし、無効化しないと特定環境でIMEメッセージを受け取ってハングアップする問題が有ったため
  // (無効化してもIMEのメッセージは飛んで来るがハングアップはしなくなった)
  //
  // ドキュメントが少ないので今一つ怪しいところではあるが
  // ウィンドウ作成時にはデフォルトのコンテキストが作成されセットされているため、戻り値はそれが返ってくるはず
  // おそらくウィンドウ削除と同時に削除されるはずなので特にフォローは必要ないと思われる
  ImmAssociateContext(window_handle_, nullptr);

  // スクロールの設定
  scroll_info_.fMask = SIF_PAGE | SIF_RANGE;
  scroll_info_.nMin = 0;
  scroll_info_.nMax = 1 + Setting::KeyBindIndex::kLength - 1;  // nMinとnMaxで全行数を指定する(全キーバインド数 + 表記変更の分の行)
  scroll_info_.nPage = kLinesOnPage;                           // 1画面に何行表示するかを指定する
  SetScrollInfo(window_handle_, SB_VERT, &scroll_info_, TRUE);

  // 以降もこの構造体を使用するが、nPosの変更にのみ使用する(ウィンドウをリサイズしないので)
  scroll_info_.fMask = SIF_POS;

  // ノート表示用のリソースを設定によって決定
  const int* const key_bind_name_resource_ids = setting_->GetNoteLabelStyle() == Setting::NoteLabelStyle::kEnglish ? kKeyBindNameEnglishStyleResourceIds : kKeyBindNameYamahaStyleResourceIds;

  // キーバインドのコントロールを配置
  for (size_t index = 0; index < Setting::KeyBindIndex::kLength; ++index) {
    // コントロールの配置位置を初期化
    x = kClientPadding;
    y = kClientPadding + (kLineHeight + kControlMarginBottom) * static_cast<int>(index);

    // キーバインド対象の表示テキスト
    label_text_handles_[index] = CreateWindow(_T("STATIC"), string_table_->get(key_bind_name_resource_ids[index]), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTERIMAGE | SS_LEFT, x, y, kNameTextWidth, kLineHeight, window_handle_, nullptr, instance_handle_, nullptr);
    if (label_text_handles_[index] == nullptr) {
      goto DESTROY;
    }
    x += kNameTextWidth + kControlMarginRight;

    // キーバインドの表示テキスト
    key_text_handles_[index] = CreateWindow(_T("STATIC"), KeyBindToString(setting_->GetKeyBind(index)), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTERIMAGE | SS_LEFT, x, y, kKeyTextWidthH, kLineHeight, window_handle_, nullptr, instance_handle_, nullptr);
    if (key_text_handles_[index] == nullptr) {
      goto DESTROY;
    }
    x += kKeyTextWidthH + kControlMarginRight;

    // キーバインド変更ボタン
    change_button_handles_[index] = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_CHANGE_BUTTON), WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, x, y, kChangeButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kChangeButtonControl | index), instance_handle_, nullptr);
    if (change_button_handles_[index] == nullptr) {
      goto DESTROY;
    }

    // キーバインド変更確定ボタン
    commit_button_handles_[index] = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_COMMIT_BUTTON), WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, x, y, kCommitButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kCommitButtonControl | index), instance_handle_, nullptr);
    if (commit_button_handles_[index] == nullptr) {
      goto DESTROY;
    }
    x += kCommitButtonWidth + kControlMarginRight;

    // キーバインド変更キャンセルボタン
    cancel_button_handles_[index] = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_CANCEL_BUTTON), WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, x, y, kCancelButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kCancelButtonControl | index), instance_handle_, nullptr);
    if (cancel_button_handles_[index] == nullptr) {
      goto DESTROY;
    }
  }

  // ノート表記方式設定の表示位置
  x = ((kClientPadding + kNameTextWidth + kControlMarginRight + kKeyTextWidthH + kControlMarginRight + kCommitButtonWidth + kControlMarginRight + kCancelButtonWidth + kClientPadding) - (kClientPadding + kNoteLabelStyleStaticWidth + kControlMarginRight + kNoteLabelStyleButtonWidth + kControlMarginRight + kNoteLabelStyleButtonWidth + kClientPadding)) / 2;
  y = kClientPadding + (kLineHeight + kControlMarginBottom) * static_cast<int>(Setting::KeyBindIndex::kLength);

  note_label_style_static_handle_ = CreateWindow(_T("STATIC"), string_table_->get(IDS_KEYBINDWINDOW_NOTE_LABEL_STYLE), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_LEFT, x, y, kNoteLabelStyleStaticWidth, kLineHeight, window_handle_, nullptr, instance_handle_, nullptr);
  x += kNoteLabelStyleStaticWidth + kControlMarginRight;

  // ラジオボタングループで生成(WS_GROUPを指定すると次にWS_GROUPを指定して作成するまでがグループになるというそびえ立つクソのような仕様)
  note_label_style_english_button_handle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_NOTE_LABEL_STYLE_ENGLISH), WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, x, y, kNoteLabelStyleButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kNoteLabelStyleRadioControl | static_cast<BYTE>(Setting::NoteLabelStyle::kEnglish)), instance_handle_, nullptr);
  x += kNoteLabelStyleButtonWidth + kControlMarginRight;

  note_label_style_yamaha_button_handle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_NOTE_LABEL_STYLE_YAMAHA), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, x, y, kNoteLabelStyleButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kNoteLabelStyleRadioControl | static_cast<BYTE>(Setting::NoteLabelStyle::kYamaha)), instance_handle_, nullptr);

  if (setting_->GetNoteLabelStyle() == Setting::NoteLabelStyle::kEnglish) {
    SendMessage(note_label_style_english_button_handle_, BM_SETCHECK, BST_CHECKED, 0);
  } else {
    SendMessage(note_label_style_yamaha_button_handle_, BM_SETCHECK, BST_CHECKED, 0);
  }

  // Windowを表示
  ShowWindow(window_handle_, SW_SHOW);

  return true;

DESTROY:

  MessageBox(window_handle_, string_table_->get(IDS_ERROR_CREATE_WINDOW), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
  PostMessage(window_handle_, WM_CLOSE, 0, 0);
  Run();
  return false;
}

void KeyBindWindow::Run() {
  MSG message;
  for (valid_ = true; valid_;) {
    BOOL result = GetMessage(&message, window_handle_, 0, 0);
    if (result == 0 || result == -1) {
      break;
    }
    DispatchMessage(&message);
  }
}

int KeyBindWindow::RegisterWindowClass(HINSTANCE instance_handle) {
  // 複数回やっても上書きされるだけで大丈夫な気がするけど
  // 一応最初に1回だけ登録すればいい物なのでそのように処理しておく
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.lpszClassName = kWindowClassName;
  wcex.hInstance = instance_handle;
  wcex.style = 0;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.lpszMenuName = nullptr;
  wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
  wcex.hIcon = reinterpret_cast<HICON>(LoadImage(instance_handle, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
  wcex.hIconSm = reinterpret_cast<HICON>(LoadImage(instance_handle, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.lpfnWndProc = CallbackProcedure;
  RegisterClassEx(&wcex);

  return 0;
}

LRESULT WINAPI KeyBindWindow::CallbackProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
  KeyBindWindow* window = reinterpret_cast<KeyBindWindow*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
  if (window != nullptr) {
    return window->Procedure(windowHandle, message, wParam, lParam);
  } else {
    return DefWindowProc(windowHandle, message, wParam, lParam);
  }
}

LRESULT KeyBindWindow::Procedure(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) {
  switch (message) {
    case WM_VSCROLL: {
      int old_scroll_position = scroll_info_.nPos;

      switch (LOWORD(wparam)) {
        case SB_TOP:
          scroll_info_.nPos = scroll_info_.nMin;
          break;
        case SB_BOTTOM:
          scroll_info_.nPos = scroll_info_.nMax - (static_cast<int>(scroll_info_.nPage) - 1);
          break;
        case SB_LINEUP:
          if (scroll_info_.nPos > scroll_info_.nMin) {
            --scroll_info_.nPos;
          }
          break;
        case SB_LINEDOWN:
          if (scroll_info_.nPos < scroll_info_.nMax - (static_cast<int>(scroll_info_.nPage) - 1)) {
            ++scroll_info_.nPos;
          }
          break;
        case SB_PAGEUP:
          scroll_info_.nPos -= scroll_info_.nPage;
          if (scroll_info_.nPos < scroll_info_.nMin) {
            scroll_info_.nPos = scroll_info_.nMin;
          }
          break;
        case SB_PAGEDOWN:
          scroll_info_.nPos += scroll_info_.nPage;
          if (scroll_info_.nPos > scroll_info_.nMax - (static_cast<int>(scroll_info_.nPage) - 1)) {
            scroll_info_.nPos = scroll_info_.nMax - (static_cast<int>(scroll_info_.nPage) - 1);
          }
          break;
        case SB_THUMBPOSITION:
          scroll_info_.nPos = HIWORD(wparam);
          break;
      }

      // スクロールバーの設定
      SetScrollInfo(window_handle, SB_VERT, &scroll_info_, TRUE);

      // クライアント領域をスクロールする
      ScrollWindowEx(window_handle, 0, (old_scroll_position - scroll_info_.nPos) * (kLineHeight + kControlMarginBottom), nullptr, nullptr, nullptr, nullptr, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
    }
      return 0;

    case WM_MOUSEWHEEL: {
      int z_delta = GET_WHEEL_DELTA_WPARAM(wparam);
      if (z_delta > 0) {
        if (scroll_info_.nPos > scroll_info_.nMin) {
          int old_scroll_position = scroll_info_.nPos;

          --scroll_info_.nPos;

          // スクロールバーの設定
          SetScrollInfo(window_handle, SB_VERT, &scroll_info_, TRUE);

          // クライアント領域をスクロールする
          ScrollWindowEx(window_handle, 0, (old_scroll_position - scroll_info_.nPos) * (kLineHeight + kControlMarginBottom), nullptr, nullptr, nullptr, nullptr, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
        }
        return 0;
      } else if (z_delta < 0) {
        if (scroll_info_.nPos < scroll_info_.nMax - (static_cast<int>(scroll_info_.nPage) - 1)) {
          int old_scroll_position = scroll_info_.nPos;

          ++scroll_info_.nPos;

          // スクロールバーの設定
          SetScrollInfo(window_handle, SB_VERT, &scroll_info_, TRUE);

          // クライアント領域をスクロールする
          ScrollWindowEx(window_handle, 0, (old_scroll_position - scroll_info_.nPos) * (kLineHeight + kControlMarginBottom), nullptr, nullptr, nullptr, nullptr, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
        }
        return 0;
      }
    } break;

    case WM_COMMAND: {
      WORD control_type = LOWORD(wparam) & kControlMask;
      WORD control_value = LOWORD(wparam) & (~kControlMask);

      if (control_type == kNoteLabelStyleRadioControl) {
        if (control_value == static_cast<WORD>(Setting::NoteLabelStyle::kEnglish)) {
          if (setting_->GetNoteLabelStyle() != Setting::NoteLabelStyle::kEnglish) {
            // 設定変更
            setting_->SetNoteLabelStyle(Setting::NoteLabelStyle::kEnglish);

            // 表記方式変更を反映
            for (size_t index = 0; index < Setting::KeyBindIndex::kLength; ++index) {
              SetWindowText(label_text_handles_[index], string_table_->get(kKeyBindNameEnglishStyleResourceIds[index]));
            }
          }

          return 0;
        } else if (control_value == static_cast<WORD>(Setting::NoteLabelStyle::kYamaha)) {
          if (setting_->GetNoteLabelStyle() != Setting::NoteLabelStyle::kYamaha) {
            // 設定変更
            setting_->SetNoteLabelStyle(Setting::NoteLabelStyle::kYamaha);

            // 表記方式変更を反映
            for (size_t index = 0; index < Setting::KeyBindIndex::kLength; ++index) {
              SetWindowText(label_text_handles_[index], string_table_->get(kKeyBindNameYamahaStyleResourceIds[index]));
            }
          }

          return 0;
        }
      } else if (control_type == kChangeButtonControl) {
        if (control_value < Setting::KeyBindIndex::kLength) {
          // 別のキーバインドを設定中の場合、そのキーバインドの状態を元に戻す
          if (target_index_ != Setting::KeyBindIndex::kLength) {
            ShowWindow(change_button_handles_[target_index_], SW_SHOW);
            ShowWindow(commit_button_handles_[target_index_], SW_HIDE);
            ShowWindow(cancel_button_handles_[target_index_], SW_HIDE);
            SetWindowText(key_text_handles_[target_index_], KeyBindToString(setting_->GetKeyBind(target_index_)));
          }

          // 選択されたキーバインド対象の番号を記録
          target_index_ = control_value;

          // キー入力をクリア
          current_key_bind_ = 0;

          // キー入力を促すメッセージを表示
          SetWindowText(key_text_handles_[target_index_], string_table_->get(IDS_KEYBINDWINDOW_KEY_INPUT_MESSAGE));

          // 変更ボタンを消して確定ボタン・キャンセルボタンを表示
          ShowWindow(change_button_handles_[target_index_], SW_HIDE);
          ShowWindow(commit_button_handles_[target_index_], SW_SHOW);
          ShowWindow(cancel_button_handles_[target_index_], SW_SHOW);

          // ボタンに行ったフォーカスをウィンドウに戻す(キー入力を取るため)
          SetFocus(window_handle);

          return 0;
        }
      } else if (control_type == kCommitButtonControl) {
        // 他のボタンは非表示にしているので来ないはずだが、一応一致しているか確認する
        if (control_value == target_index_) {
          // キーバインド更新(キーを入力していなければ未設定に更新する)
          setting_->SetKeyBind(target_index_, current_key_bind_);

          // キーバインドの表示を更新
          for (size_t i = 0; i < Setting::KeyBindIndex::kLength; ++i)
            SetWindowText(key_text_handles_[i], KeyBindToString(setting_->GetKeyBind(i)));

          // 確定ボタン・キャンセルボタンを消して変更ボタンを表示
          ShowWindow(change_button_handles_[target_index_], SW_SHOW);
          ShowWindow(commit_button_handles_[target_index_], SW_HIDE);
          ShowWindow(cancel_button_handles_[target_index_], SW_HIDE);

          // 選択中のキーバインド無し
          target_index_ = Setting::KeyBindIndex::kLength;

          return 0;
        }
      } else if (control_type == kCancelButtonControl) {
        // 他のボタンは非表示にしているので来ないはずだが、一応一致しているか確認する
        if (control_value == target_index_) {
          // キーバインドの表示を元に戻す
          SetWindowText(key_text_handles_[target_index_], KeyBindToString(setting_->GetKeyBind(target_index_)));

          // 確定ボタン・キャンセルボタンを消して変更ボタンを表示
          ShowWindow(change_button_handles_[target_index_], SW_SHOW);
          ShowWindow(commit_button_handles_[target_index_], SW_HIDE);
          ShowWindow(cancel_button_handles_[target_index_], SW_HIDE);

          // 選択中のキーバインド無し
          target_index_ = Setting::KeyBindIndex::kLength;
          return 0;
        }
      }
    } break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      wparam &= 0xff;
      if (wparam == VK_SHIFT) {
        modifier_key_state_ |= Setting::kShiftKeyBind;
      } else if (wparam == VK_CONTROL) {
        modifier_key_state_ |= Setting::kControlKeyBind;
      } else if (wparam == VK_MENU) {
        modifier_key_state_ |= Setting::kAlternateKeyBind;
      }
      break;

    case WM_SYSKEYUP:
    case WM_KEYUP:
      wparam &= 0xff;
      if (wparam == VK_SHIFT) {
        modifier_key_state_ &= ~Setting::kShiftKeyBind;
      } else if (wparam == VK_CONTROL) {
        modifier_key_state_ &= ~Setting::kControlKeyBind;
      } else if (wparam == VK_MENU) {
        modifier_key_state_ &= ~Setting::kAlternateKeyBind;
      } else if (kVirtualKeyNames[wparam] != nullptr) {
        // 設定変更中の場合にキー入力を保存し表示する
        if (target_index_ != Setting::KeyBindIndex::kLength) {
          current_key_bind_ = static_cast<WORD>(wparam) | modifier_key_state_;
          SetWindowText(key_text_handles_[target_index_], KeyBindToString(current_key_bind_));
        }
        return 0;
      }
      break;

    case WM_CTLCOLORSTATIC:
      SetBkMode(reinterpret_cast<HDC>(wparam), TRANSPARENT);
      return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_BTNFACE));

    case WM_CLOSE:
      DestroyWindow(window_handle);
      return 0;

    case WM_DESTROY:
      // 自力で制御しなくてもGetMessageが-1を返してエラーとして終了するんだけど一応制御する
      valid_ = false;
      return 0;
  }

  return DefWindowProc(window_handle, message, wparam, lparam);
}

const TCHAR* KeyBindWindow::KeyBindToString(Setting::KeyBind key_bind) {
  if (key_bind == 0) {
    key_bind_text_buffer_[0] = 0;
    return 0;
  }

  // 本来設定出来ない値を設定ファイルの直接編集により設定しているケースなのでそいつが悪いということで何も表示しない
  if (kVirtualKeyNames[key_bind & 0xff] == nullptr) {
    key_bind_text_buffer_[0] = 0;
    return 0;
  }

  int written = 1;
  key_bind_text_buffer_[0] = ' ';

  if ((key_bind & Setting::kShiftKeyBind) != 0) {
    written += _stprintf_s(&key_bind_text_buffer_[written], kKeyTextBufferLength - written, _T("Shift + "));
  }

  if ((key_bind & Setting::kControlKeyBind) != 0) {
    written += _stprintf_s(&key_bind_text_buffer_[written], kKeyTextBufferLength - written, _T("Ctrl + "));
  }

  if ((key_bind & Setting::kAlternateKeyBind) != 0) {
    written += _stprintf_s(&key_bind_text_buffer_[written], kKeyTextBufferLength - written, _T("Alt + "));
  }

  _stprintf_s(&key_bind_text_buffer_[written], kKeyTextBufferLength - written, kVirtualKeyNames[key_bind & 0xff]);

  return key_bind_text_buffer_;
}

}  // namespace keysequencer