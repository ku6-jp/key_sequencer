#include "key_bind_window.h"

#include <cstdio>

#include "main_window.h"
#include "string_table.h"

namespace keysequencer {

KeyBindWindow::KeyBindWindow(HINSTANCE instance_handle, HWND parent_window_handle, const StringTable* string_table, Setting* setting) : instance_handle_(instance_handle), parent_window_handle_(parent_window_handle), string_table_(string_table), setting_(setting) {
  static int static_initialize = RegisterWindowClass(instance_handle);

  // ����ɕK�{�Ƃ����킯�ł͂Ȃ����ꉞ���������Ă���
  for (std::size_t i = 0; i < Setting::KeyBindIndex::kLength; ++i) {
    label_text_handles_[i] = nullptr;
    key_text_handles_[i] = nullptr;
    change_button_handles_[i] = nullptr;
    commit_button_handles_[i] = nullptr;
    cancel_button_handles_[i] = nullptr;
  }

  // �\���̂̃T�C�Y�����͌Œ�Ȃ̂ł����Őݒ肵�Ă���
  scroll_info_.cbSize = sizeof(SCROLLINFO);
}

bool KeyBindWindow::Create() {
  if (created_) {
    return false;
  }
  created_ = true;

  // �쐬����E�B���h�E�̃X�^�C��
  DWORD window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VSCROLL;

  // �N���C�A���g�̈悩��E�B���h�E�T�C�Y���v�Z
  int width = kClientPadding + kNameTextWidth + kControlMarginRight + kKeyTextWidthH + kControlMarginRight + kCommitButtonWidth + kControlMarginRight + kCancelButtonWidth + kClientPadding;
  int height = kClientPadding + (kLineHeight * kLinesOnPage) + (kControlMarginBottom * (kLinesOnPage - 1)) + kClientPadding;
  RECT client_rect = {0, 0, width, height};
  if (AdjustWindowRect(&client_rect, window_style, FALSE)) {
    width = client_rect.right - client_rect.left + GetSystemMetrics(SM_CXVSCROLL);
    height = client_rect.bottom - client_rect.top;
  }

  // ��ʒ����������ʒu�Ƃ���
  int x = GetSystemMetrics(SM_CXSCREEN);
  int y = GetSystemMetrics(SM_CYSCREEN);
  x = x > width ? (x - width) / 2 : 0;
  y = y > height ? (y - height) / 2 : 0;

  // Window�쐬
  window_handle_ = CreateWindowEx(0, kWindowClassName, string_table_->get(IDS_KEYBINDWINDOW_CAPTION), window_style, x, y, width, height, nullptr, nullptr, instance_handle_, nullptr);
  if (window_handle_ == nullptr) {
    MessageBox(parent_window_handle_, string_table_->get(IDS_ERROR_CREATE_WINDOW), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
    return false;
  }

  // Window procedure�p��this�|�C���^���L��
  SetWindowLongPtr(window_handle_, GWLP_USERDATA, 0);
  if (SetWindowLongPtr(window_handle_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)) == 0 && GetLastError() != 0) {
    goto DESTROY;
  }

  // IME�𖳌�������
  //
  // �@�\�I�ɂ�IME�����삷��Ǝז��ł��邵�A���������Ȃ��Ɠ������IME���b�Z�[�W���󂯎���ăn���O�A�b�v�����肪�L��������
  // (���������Ă�IME�̃��b�Z�[�W�͔��ŗ��邪�n���O�A�b�v�͂��Ȃ��Ȃ���)
  //
  // �h�L�������g�����Ȃ��̂ō���������Ƃ���ł͂��邪
  // �E�B���h�E�쐬���ɂ̓f�t�H���g�̃R���e�L�X�g���쐬����Z�b�g����Ă��邽�߁A�߂�l�͂��ꂪ�Ԃ��Ă���͂�
  // �����炭�E�B���h�E�폜�Ɠ����ɍ폜�����͂��Ȃ̂œ��Ƀt�H���[�͕K�v�Ȃ��Ǝv����
  ImmAssociateContext(window_handle_, nullptr);

  // �X�N���[���̐ݒ�
  scroll_info_.fMask = SIF_PAGE | SIF_RANGE;
  scroll_info_.nMin = 0;
  scroll_info_.nMax = 1 + Setting::KeyBindIndex::kLength - 1;  // nMin��nMax�őS�s�����w�肷��(�S�L�[�o�C���h�� + �\�L�ύX�̕��̍s)
  scroll_info_.nPage = kLinesOnPage;                           // 1��ʂɉ��s�\�����邩���w�肷��
  SetScrollInfo(window_handle_, SB_VERT, &scroll_info_, TRUE);

  // �ȍ~�����̍\���̂��g�p���邪�AnPos�̕ύX�ɂ̂ݎg�p����(�E�B���h�E�����T�C�Y���Ȃ��̂�)
  scroll_info_.fMask = SIF_POS;

  // �m�[�g�\���p�̃��\�[�X��ݒ�ɂ���Č���
  const int* const key_bind_name_resource_ids = setting_->GetNoteLabelStyle() == Setting::NoteLabelStyle::kEnglish ? kKeyBindNameEnglishStyleResourceIds : kKeyBindNameYamahaStyleResourceIds;

  // �L�[�o�C���h�̃R���g���[����z�u
  for (size_t index = 0; index < Setting::KeyBindIndex::kLength; ++index) {
    // �R���g���[���̔z�u�ʒu��������
    x = kClientPadding;
    y = kClientPadding + (kLineHeight + kControlMarginBottom) * static_cast<int>(index);

    // �L�[�o�C���h�Ώۂ̕\���e�L�X�g
    label_text_handles_[index] = CreateWindow(_T("STATIC"), string_table_->get(key_bind_name_resource_ids[index]), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTERIMAGE | SS_LEFT, x, y, kNameTextWidth, kLineHeight, window_handle_, nullptr, instance_handle_, nullptr);
    if (label_text_handles_[index] == nullptr) {
      goto DESTROY;
    }
    x += kNameTextWidth + kControlMarginRight;

    // �L�[�o�C���h�̕\���e�L�X�g
    key_text_handles_[index] = CreateWindow(_T("STATIC"), KeyBindToString(setting_->GetKeyBind(index)), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTERIMAGE | SS_LEFT, x, y, kKeyTextWidthH, kLineHeight, window_handle_, nullptr, instance_handle_, nullptr);
    if (key_text_handles_[index] == nullptr) {
      goto DESTROY;
    }
    x += kKeyTextWidthH + kControlMarginRight;

    // �L�[�o�C���h�ύX�{�^��
    change_button_handles_[index] = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_CHANGE_BUTTON), WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, x, y, kChangeButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kChangeButtonControl | index), instance_handle_, nullptr);
    if (change_button_handles_[index] == nullptr) {
      goto DESTROY;
    }

    // �L�[�o�C���h�ύX�m��{�^��
    commit_button_handles_[index] = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_COMMIT_BUTTON), WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, x, y, kCommitButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kCommitButtonControl | index), instance_handle_, nullptr);
    if (commit_button_handles_[index] == nullptr) {
      goto DESTROY;
    }
    x += kCommitButtonWidth + kControlMarginRight;

    // �L�[�o�C���h�ύX�L�����Z���{�^��
    cancel_button_handles_[index] = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_CANCEL_BUTTON), WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, x, y, kCancelButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kCancelButtonControl | index), instance_handle_, nullptr);
    if (cancel_button_handles_[index] == nullptr) {
      goto DESTROY;
    }
  }

  // �m�[�g�\�L�����ݒ�̕\���ʒu
  x = ((kClientPadding + kNameTextWidth + kControlMarginRight + kKeyTextWidthH + kControlMarginRight + kCommitButtonWidth + kControlMarginRight + kCancelButtonWidth + kClientPadding) - (kClientPadding + kNoteLabelStyleStaticWidth + kControlMarginRight + kNoteLabelStyleButtonWidth + kControlMarginRight + kNoteLabelStyleButtonWidth + kClientPadding)) / 2;
  y = kClientPadding + (kLineHeight + kControlMarginBottom) * static_cast<int>(Setting::KeyBindIndex::kLength);

  note_label_style_static_handle_ = CreateWindow(_T("STATIC"), string_table_->get(IDS_KEYBINDWINDOW_NOTE_LABEL_STYLE), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_LEFT, x, y, kNoteLabelStyleStaticWidth, kLineHeight, window_handle_, nullptr, instance_handle_, nullptr);
  x += kNoteLabelStyleStaticWidth + kControlMarginRight;

  // ���W�I�{�^���O���[�v�Ő���(WS_GROUP���w�肷��Ǝ���WS_GROUP���w�肵�č쐬����܂ł��O���[�v�ɂȂ�Ƃ������т����N�\�̂悤�Ȏd�l)
  note_label_style_english_button_handle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_NOTE_LABEL_STYLE_ENGLISH), WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, x, y, kNoteLabelStyleButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kNoteLabelStyleRadioControl | static_cast<BYTE>(Setting::NoteLabelStyle::kEnglish)), instance_handle_, nullptr);
  x += kNoteLabelStyleButtonWidth + kControlMarginRight;

  note_label_style_yamaha_button_handle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_KEYBINDWINDOW_NOTE_LABEL_STYLE_YAMAHA), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, x, y, kNoteLabelStyleButtonWidth, kLineHeight, window_handle_, reinterpret_cast<HMENU>(kNoteLabelStyleRadioControl | static_cast<BYTE>(Setting::NoteLabelStyle::kYamaha)), instance_handle_, nullptr);

  if (setting_->GetNoteLabelStyle() == Setting::NoteLabelStyle::kEnglish) {
    SendMessage(note_label_style_english_button_handle_, BM_SETCHECK, BST_CHECKED, 0);
  } else {
    SendMessage(note_label_style_yamaha_button_handle_, BM_SETCHECK, BST_CHECKED, 0);
  }

  // Window��\��
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
  // ���������Ă��㏑������邾���ő��v�ȋC�����邯��
  // �ꉞ�ŏ���1�񂾂��o�^����΂������Ȃ̂ł��̂悤�ɏ������Ă���
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

      // �X�N���[���o�[�̐ݒ�
      SetScrollInfo(window_handle, SB_VERT, &scroll_info_, TRUE);

      // �N���C�A���g�̈���X�N���[������
      ScrollWindowEx(window_handle, 0, (old_scroll_position - scroll_info_.nPos) * (kLineHeight + kControlMarginBottom), nullptr, nullptr, nullptr, nullptr, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
    }
      return 0;

    case WM_MOUSEWHEEL: {
      int z_delta = GET_WHEEL_DELTA_WPARAM(wparam);
      if (z_delta > 0) {
        if (scroll_info_.nPos > scroll_info_.nMin) {
          int old_scroll_position = scroll_info_.nPos;

          --scroll_info_.nPos;

          // �X�N���[���o�[�̐ݒ�
          SetScrollInfo(window_handle, SB_VERT, &scroll_info_, TRUE);

          // �N���C�A���g�̈���X�N���[������
          ScrollWindowEx(window_handle, 0, (old_scroll_position - scroll_info_.nPos) * (kLineHeight + kControlMarginBottom), nullptr, nullptr, nullptr, nullptr, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
        }
        return 0;
      } else if (z_delta < 0) {
        if (scroll_info_.nPos < scroll_info_.nMax - (static_cast<int>(scroll_info_.nPage) - 1)) {
          int old_scroll_position = scroll_info_.nPos;

          ++scroll_info_.nPos;

          // �X�N���[���o�[�̐ݒ�
          SetScrollInfo(window_handle, SB_VERT, &scroll_info_, TRUE);

          // �N���C�A���g�̈���X�N���[������
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
            // �ݒ�ύX
            setting_->SetNoteLabelStyle(Setting::NoteLabelStyle::kEnglish);

            // �\�L�����ύX�𔽉f
            for (size_t index = 0; index < Setting::KeyBindIndex::kLength; ++index) {
              SetWindowText(label_text_handles_[index], string_table_->get(kKeyBindNameEnglishStyleResourceIds[index]));
            }
          }

          return 0;
        } else if (control_value == static_cast<WORD>(Setting::NoteLabelStyle::kYamaha)) {
          if (setting_->GetNoteLabelStyle() != Setting::NoteLabelStyle::kYamaha) {
            // �ݒ�ύX
            setting_->SetNoteLabelStyle(Setting::NoteLabelStyle::kYamaha);

            // �\�L�����ύX�𔽉f
            for (size_t index = 0; index < Setting::KeyBindIndex::kLength; ++index) {
              SetWindowText(label_text_handles_[index], string_table_->get(kKeyBindNameYamahaStyleResourceIds[index]));
            }
          }

          return 0;
        }
      } else if (control_type == kChangeButtonControl) {
        if (control_value < Setting::KeyBindIndex::kLength) {
          // �ʂ̃L�[�o�C���h��ݒ蒆�̏ꍇ�A���̃L�[�o�C���h�̏�Ԃ����ɖ߂�
          if (target_index_ != Setting::KeyBindIndex::kLength) {
            ShowWindow(change_button_handles_[target_index_], SW_SHOW);
            ShowWindow(commit_button_handles_[target_index_], SW_HIDE);
            ShowWindow(cancel_button_handles_[target_index_], SW_HIDE);
            SetWindowText(key_text_handles_[target_index_], KeyBindToString(setting_->GetKeyBind(target_index_)));
          }

          // �I�����ꂽ�L�[�o�C���h�Ώۂ̔ԍ����L�^
          target_index_ = control_value;

          // �L�[���͂��N���A
          current_key_bind_ = 0;

          // �L�[���͂𑣂����b�Z�[�W��\��
          SetWindowText(key_text_handles_[target_index_], string_table_->get(IDS_KEYBINDWINDOW_KEY_INPUT_MESSAGE));

          // �ύX�{�^���������Ċm��{�^���E�L�����Z���{�^����\��
          ShowWindow(change_button_handles_[target_index_], SW_HIDE);
          ShowWindow(commit_button_handles_[target_index_], SW_SHOW);
          ShowWindow(cancel_button_handles_[target_index_], SW_SHOW);

          // �{�^���ɍs�����t�H�[�J�X���E�B���h�E�ɖ߂�(�L�[���͂���邽��)
          SetFocus(window_handle);

          return 0;
        }
      } else if (control_type == kCommitButtonControl) {
        // ���̃{�^���͔�\���ɂ��Ă���̂ŗ��Ȃ��͂������A�ꉞ��v���Ă��邩�m�F����
        if (control_value == target_index_) {
          // �L�[�o�C���h�X�V(�L�[����͂��Ă��Ȃ���Ζ��ݒ�ɍX�V����)
          setting_->SetKeyBind(target_index_, current_key_bind_);

          // �L�[�o�C���h�̕\�����X�V
          for (size_t i = 0; i < Setting::KeyBindIndex::kLength; ++i)
            SetWindowText(key_text_handles_[i], KeyBindToString(setting_->GetKeyBind(i)));

          // �m��{�^���E�L�����Z���{�^���������ĕύX�{�^����\��
          ShowWindow(change_button_handles_[target_index_], SW_SHOW);
          ShowWindow(commit_button_handles_[target_index_], SW_HIDE);
          ShowWindow(cancel_button_handles_[target_index_], SW_HIDE);

          // �I�𒆂̃L�[�o�C���h����
          target_index_ = Setting::KeyBindIndex::kLength;

          return 0;
        }
      } else if (control_type == kCancelButtonControl) {
        // ���̃{�^���͔�\���ɂ��Ă���̂ŗ��Ȃ��͂������A�ꉞ��v���Ă��邩�m�F����
        if (control_value == target_index_) {
          // �L�[�o�C���h�̕\�������ɖ߂�
          SetWindowText(key_text_handles_[target_index_], KeyBindToString(setting_->GetKeyBind(target_index_)));

          // �m��{�^���E�L�����Z���{�^���������ĕύX�{�^����\��
          ShowWindow(change_button_handles_[target_index_], SW_SHOW);
          ShowWindow(commit_button_handles_[target_index_], SW_HIDE);
          ShowWindow(cancel_button_handles_[target_index_], SW_HIDE);

          // �I�𒆂̃L�[�o�C���h����
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
        // �ݒ�ύX���̏ꍇ�ɃL�[���͂�ۑ����\������
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
      // ���͂Ő��䂵�Ȃ��Ă�GetMessage��-1��Ԃ��ăG���[�Ƃ��ďI������񂾂��ǈꉞ���䂷��
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

  // �{���ݒ�o���Ȃ��l��ݒ�t�@�C���̒��ڕҏW�ɂ��ݒ肵�Ă���P�[�X�Ȃ̂ł����������Ƃ������Ƃŉ����\�����Ȃ�
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