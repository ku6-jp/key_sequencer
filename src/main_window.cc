#include "main_window.h"

#include "constants.h"
#include "key_bind_window.h"
#include "keyboard.h"
#include "midi_device_input_to_keyboard.h"
#include "midi_synthesizer.h"
#include "sequencer_manager.h"
#include "setting.h"
#include "standard_midi_format_reader.h"
#include "string_table.h"

namespace keysequencer {

MainWindow::MainWindow(HINSTANCE instance_handle, const StringTable* string_table, Setting* setting, MidiDeviceInputToKeyboard* midi_input_to_keyboard, SequencerManager* sequencer_manager)
    : instance_handle_(instance_handle), string_table_(string_table), setting_(setting), midi_input_to_keyboard_(midi_input_to_keyboard), sequencer_manager_(sequencer_manager) {
  static int static_initialize = RegisterWindowClass(instance_handle);
}

bool MainWindow::Create() {
  // 1�x�������s����
  if (created_) {
    return false;
  }
  created_ = true;

  // �쐬����E�B���h�E�̃X�^�C��
  DWORD window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

  // �N���C�A���g�̈悩��E�B���h�E�T�C�Y���v�Z
  int width = kClientWidth;
  int height = kClientHeight;
  RECT client_rect = {0, 0, width, height};
  if (AdjustWindowRect(&client_rect, window_style, FALSE)) {
    width = client_rect.right - client_rect.left;
    height = client_rect.bottom - client_rect.top;
  }

  // ��ʒ����������ʒu�Ƃ���
  int x = GetSystemMetrics(SM_CXSCREEN);
  int y = GetSystemMetrics(SM_CYSCREEN);
  x = x > width ? (x - width) / 2 : 0;
  y = y > height ? (y - height) / 2 : 0;

  // Window�쐬
  window_handle_ = CreateWindowEx(0, kWindowClassName, string_table_->get(IDS_MAINWINDOW_CAPTION), window_style, x, y, width, height, nullptr, nullptr, instance_handle_, nullptr);
  if (window_handle_ == nullptr) {
    MessageBox(nullptr, string_table_->get(IDS_ERROR_CREATE_WINDOW), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
    return false;
  }

  // Window procedure�p��this�|�C���^���L��
  SetWindowLongPtr(window_handle_, GWLP_USERDATA, 0);
  if (SetWindowLongPtr(window_handle_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)) == 0 && GetLastError() != 0) {
    goto DESTROY;
  }

  // IME�𖳌�������
  // ���C���E�B���h�E�ł͖��������Ȃ��Ă��悢���L���ł���K�v���Ȃ��̂ő��E�B���h�E�ƍ��킹�Ė����Ƃ���
  ImmAssociateContext(window_handle_, nullptr);

  // �Đ�����t�@�C������\��
  file_text_handle_ = CreateWindow(_T("STATIC"), string_table_->get(IDS_MAINWINDOW_FILE_EMPTY), WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 8, 8, kClientWidth - 16, 30, window_handle_, nullptr, instance_handle_, nullptr);
  if (file_text_handle_ == nullptr) {
    goto DESTROY;
  }

  // �Đ��X�e�[�^�X��\��
  status_text_handle_ = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 8, 46, kClientWidth - 16, 30, window_handle_, nullptr, instance_handle_, nullptr);
  if (status_text_handle_ == nullptr) {
    goto DESTROY;
  }

  // �L�[�o�C���h�ݒ�{�^����\��
  open_key_bind_button_hanndle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_MAINWINDOW_KEY_BIND_BUTTON), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, 8, 84, 128, 30, window_handle_, reinterpret_cast<HMENU>(kKeyBindButtonId), instance_handle_, nullptr);
  if (open_key_bind_button_hanndle_ == nullptr) {
    goto DESTROY;
  }

  // �e�X�g�Đ�/��~�{�^����\��
  test_play_button_handle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_MAINWINDOW_TEST_PLAY_BUTTON), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, 8 + 128 + 8, 84, 192, 30, window_handle_, reinterpret_cast<HMENU>(kTestPlayButtonId), instance_handle_, nullptr);
  if (test_play_button_handle_ == nullptr) {
    goto DESTROY;
  }

  // Window��\��
  ShowWindow(window_handle_, SW_SHOW);

  // Drag & Drop��L����
  DragAcceptFiles(window_handle_, TRUE);

  // Hotkey�L����
  RegisterHotkeys();

  // ���t�X���b�h���J�n
  // ���t��������E�B���h�E�Ƀ��b�Z�[�W�𑗂�P�[�X���L�邽�߃E�B���h�E�n���h���������ƊJ�n�ł��Ȃ�
  sequencer_manager_->BeginThread(window_handle_);

  // �N�����Ƀt�@�C�����h���b�v����Ă�����ǂݍ���
  if (__argc > 1 && __targv[1] != nullptr) {
    Sequencer::LoadFileResult result = sequencer_manager_->LoadFile(__targv[1]);
    if (result == Sequencer::LoadFileResult::kSuccess) {
      // �t�@�C�����̒������擾
      WORD filename_length = GetFileTitle(__targv[1], nullptr, 0);

      // �t�@�C�����p�̃��������m��
      TCHAR* filename = new TCHAR[filename_length];  // �������͏I�[��NULL��length�ɓ����Ă�

      // �t�@�C�������擾
      GetFileTitle(__targv[1], filename, filename_length);

      // �t�@�C���p�X�̕\����؂�ւ�
      SetWindowText(file_text_handle_, filename);

      delete[] filename;
    }
  }

  RefreshStatus();

  return true;

DESTROY:

  MessageBox(window_handle_, string_table_->get(IDS_ERROR_CREATE_WINDOW), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
  PostMessage(window_handle_, WM_CLOSE, 0, 0);
  Run();
  return false;
}

void MainWindow::Run() {
  MSG message;
  for (;;) {
    BOOL result = GetMessage(&message, nullptr, 0, 0);
    if (result == 0 || result == -1) {
      break;
    }
    DispatchMessage(&message);
  }
}

int MainWindow::RegisterWindowClass(HINSTANCE instance_handle) {
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

LRESULT WINAPI MainWindow::CallbackProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
  MainWindow* window = reinterpret_cast<MainWindow*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
  if (window != nullptr) {
    return window->Procedure(windowHandle, message, wParam, lParam);
  }

  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      return DefWindowProc(windowHandle, message, wParam, lParam);
  }
}

LRESULT MainWindow::Procedure(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) {
  switch (message) {
    case WM_APP_REFRESH_STATUS:
      RefreshStatus();
      break;

    case WM_COMMAND:
      switch (LOWORD(wparam)) {
        case kKeyBindButtonId: {
          // ���t���Ȃ�~�߂�
          sequencer_manager_->Stop();

          // MIDI���͒��Ȃ�~�߂�
          midi_input_to_keyboard_->Close();

          RefreshStatus();

          // �L�[�o�C���h�ݒ�_�C�A���O���N��
          KeyBindWindow* child = new KeyBindWindow(instance_handle_, window_handle, string_table_, setting_);
          if (child->Create()) {
            // �ݒ蒆��Hotkey�������������͂��Ȃ��̂ň�UHotkey���~����
            UnregisterHotkeys();

            // �L�[�o�C���h�E�B���h�E���쒆�̓��C���E�B���h�E�͖���������(�����Ă�ƕ���{�^�����������肷�邯�ǃ��b�Z�[�W�������o���Ȃ��ăt���[�Y���Ă���Ɣ��f����Ă��܂�)
            EnableWindow(window_handle, FALSE);

            child->Run();

            // ���C���E�B���h�E��L���ɖ߂�
            EnableWindow(window_handle, TRUE);

            // ���C���E�B���h�E���A�N�e�B�u�ɂ���
            SetActiveWindow(window_handle);

            // Hotkey��߂�(�ύX����Ă����ꍇ�̍X�V�����˂�)
            RegisterHotkeys();
          } else {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_OPEN_KEYBINDWINDOW), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          }
          delete child;
        }
          return 0;

        case kTestPlayButtonId:
          if (sequencer_manager_->IsPlaying()) {
            if (sequencer_manager_->GetOutput() == Sequencer::Output::kMidiSynthesizer) {
              // MIDI�e�X�g�Đ����Ȃ�Β�~����
              sequencer_manager_->Stop();
            } else {
              // �Đ����Ȃ�΍Đ����~����MIDI�e�X�g�Đ����J�n����
              sequencer_manager_->Play(Sequencer::Output::kMidiSynthesizer);
            }
          } else if (midi_input_to_keyboard_->IsOpened()) {
            // MIDI���͎�t���Ȃ��MIDI���͂��~���ăe�X�g�Đ����J�n����
            midi_input_to_keyboard_->Close();
            sequencer_manager_->Play(Sequencer::Output::kMidiSynthesizer);
          } else {
            // ��~���Ȃ�΍Đ����J�n����
            sequencer_manager_->Play(Sequencer::Output::kMidiSynthesizer);
          }

          // �\�����X�V
          RefreshStatus();
          return 0;
      }
      break;

    case WM_HOTKEY:
      if (wparam == kPlayHotkeyId) {
        if (sequencer_manager_->IsPlaying()) {
          if (sequencer_manager_->GetOutput() == Sequencer::Output::kMidiSynthesizer) {
            // MIDI�e�X�g�Đ����Ȃ��MIDI�e�X�g�Đ����~���čĐ����J�n����
            sequencer_manager_->Play(Sequencer::Output::kKeyboard);
          } else {
            // �Đ����Ȃ�΍Đ����~����
            sequencer_manager_->Stop();
          }
        } else if (midi_input_to_keyboard_->IsOpened()) {
          // MIDI���͎�t���Ȃ��MIDI���͂��~���čĐ����J�n����
          midi_input_to_keyboard_->Close();
          sequencer_manager_->Play(Sequencer::Output::kKeyboard);
        } else {
          // ��~���Ȃ�΍Đ����J�n����
          sequencer_manager_->Play(Sequencer::Output::kKeyboard);
        }

        // �\�����X�V
        RefreshStatus();
      } else if (wparam == kMidiInputHotkeyId) {
        // �Đ��� or MIDI�e�X�g�Đ����Ȃ�Β�~����MIDI���͂��J�n����
        if (sequencer_manager_->IsPlaying()) {
          sequencer_manager_->Stop();

          MidiDeviceInputToKeyboard::OpenResult result = midi_input_to_keyboard_->Open();
          if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureDeviceNotFound) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_DEVICE_NOT_FOUND), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          } else if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureUnknown) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_UNKNOWN), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          }
        } else if (midi_input_to_keyboard_->IsOpened()) {
          // MIDI���͎�t���Ȃ�Β�~����
          midi_input_to_keyboard_->Close();
        } else {
          // ��~���Ȃ��MIDI���͊J�n
          MidiDeviceInputToKeyboard::OpenResult result = midi_input_to_keyboard_->Open();
          if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureDeviceNotFound) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_DEVICE_NOT_FOUND), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          } else if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureUnknown) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_UNKNOWN), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          }
        }

        // �\�����X�V
        RefreshStatus();
      } else if (wparam == kAllStopHotkeyId) {
        // �Đ��� or MIDI�e�X�g�Đ����Ȃ�Β�~����
        if (sequencer_manager_->IsPlaying()) {
          sequencer_manager_->Stop();

          // �\�����X�V
          RefreshStatus();
        }
        // MIDI���͎�t���Ȃ�Β�~����
        else if (midi_input_to_keyboard_->IsOpened()) {
          midi_input_to_keyboard_->Close();

          // �\�����X�V
          RefreshStatus();
        }
      }
      return 0;

    case WM_DROPFILES: {
      HDROP handle = reinterpret_cast<HDROP>(wparam);

      // �h���b�v���ꂽ�t�@�C��(1��)�̃p�X�̒������擾
      UINT filepath_length = DragQueryFile(handle, 0, nullptr, 0);

      // �t�@�C���p�X�p�̃��������m��
      TCHAR* filepath = new TCHAR[static_cast<size_t>(filepath_length) + 1];  // �I�[��NULL��length�ɓ���Ȃ�

      // �t�@�C���p�X���擾
      DragQueryFile(handle, 0, filepath, filepath_length + 1);
      DragFinish(handle);

      // ���t���Ȃ�~�߂�
      sequencer_manager_->Stop();

      // MIDI���͒��Ȃ�~�߂�
      midi_input_to_keyboard_->Close();

      RefreshStatus();

      // �t�@�C����ǂݍ���
      Sequencer::LoadFileResult result = sequencer_manager_->LoadFile(filepath);
      if (result == Sequencer::LoadFileResult::kSuccess) {
        // �t�@�C�����̒������擾
        WORD filenameLength = GetFileTitle(filepath, nullptr, 0);

        // �t�@�C�����p�̃��������m��
        TCHAR* filename = new TCHAR[filenameLength];  // �������͏I�[��NULL��length�ɓ����Ă�

        // �t�@�C�������擾
        GetFileTitle(filepath, filename, filenameLength);

        // �t�@�C���p�X�̕\����؂�ւ�
        SetWindowText(file_text_handle_, filename);

        delete[] filename;
      } else {
        SetWindowText(file_text_handle_, string_table_->get(IDS_MAINWINDOW_FILE_EMPTY));

        switch (result) {
          case Sequencer::LoadFileResult::kFailureFileOpen:
            // �t�@�C�����I�[�v���o���Ȃ�
            MessageBox(window_handle, string_table_->get(IDS_ERROR_OPEN_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureFileTooLarge:
            // 4GB�ȏ�̃t�@�C���͔�Ή�
            MessageBox(window_handle, string_table_->get(IDS_ERROR_TOO_LARGE_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureFileRead:
            // �t�@�C���ǂݍ��ݒ��ɕs���ȃG���[
            MessageBox(window_handle, string_table_->get(IDS_ERROR_READ_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureIllegalSmf:
            // SMF�Ƃ��ĕs��
            MessageBox(window_handle, string_table_->get(IDS_ERROR_ILLEGAL_SMF_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureNotSupportSmf:
            // �T�|�[�g���Ă��Ȃ�SMF�`��
            MessageBox(window_handle, string_table_->get(IDS_ERROR_UNSUPPORT_SMF_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
        }
      }

      delete[] filepath;
    }
      return 0;

    case WM_CTLCOLORSTATIC:
      SetBkMode(reinterpret_cast<HDC>(wparam), TRANSPARENT);
      return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_BTNFACE));

    case WM_CLOSE:
      // �E�B���h�E���Q�Ƃ�����̂��ɉ�����Ă���DestroyWindow����
      UnregisterHotkeys();

      // ���t�����X���b�h��~
      sequencer_manager_->ShutdownThread();

      DestroyWindow(window_handle);
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProc(window_handle, message, wparam, lparam);
}

void MainWindow::RegisterHotkeys() {
  WORD hotkey;

  hotkey = setting_->GetPlayHotkeyBind();
  if (registered_play_hotkey_bind_ != hotkey) {
    if (registered_play_hotkey_bind_ != 0) {
      UnregisterHotKey(window_handle_, kPlayHotkeyId);
      registered_play_hotkey_bind_ = 0;
    }

    if (hotkey != 0) {
      UINT modifiers = 0;
      if ((hotkey & Setting::kShiftKeyBind) != 0) {
        modifiers |= MOD_SHIFT;
      }
      if ((hotkey & Setting::kControlKeyBind) != 0) {
        modifiers |= MOD_CONTROL;
      }
      if ((hotkey & Setting::kAlternateKeyBind) != 0) {
        modifiers |= MOD_ALT;
      }

      if (RegisterHotKey(window_handle_, kPlayHotkeyId, modifiers, hotkey & 0x00ff)) {
        registered_play_hotkey_bind_ = hotkey;
      } else {
        MessageBox(window_handle_, string_table_->get(IDS_ERROR_REGISTER_PLAY_HOTKEY), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
      }
    }
  }

  hotkey = setting_->GetMidiInputHotkeyBind();
  if (registered_midi_input_hotkey_bind_ != hotkey) {
    if (registered_midi_input_hotkey_bind_ != 0) {
      UnregisterHotKey(window_handle_, kMidiInputHotkeyId);
      registered_midi_input_hotkey_bind_ = 0;
    }

    if (hotkey != 0) {
      UINT modifiers = 0;
      if ((hotkey & Setting::kShiftKeyBind) != 0) {
        modifiers |= MOD_SHIFT;
      }
      if ((hotkey & Setting::kControlKeyBind) != 0) {
        modifiers |= MOD_CONTROL;
      }
      if ((hotkey & Setting::kAlternateKeyBind) != 0) {
        modifiers |= MOD_ALT;
      }

      if (RegisterHotKey(window_handle_, kMidiInputHotkeyId, modifiers, hotkey & 0x00ff)) {
        registered_midi_input_hotkey_bind_ = hotkey;
      } else {
        MessageBox(window_handle_, string_table_->get(IDS_ERROR_REGISTER_MIDI_INPUT_HOTKEY), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
      }
    }
  }

  hotkey = setting_->GetAllStopHotkeyBind();
  if (registered_all_stop_hotkey_bind_ != hotkey) {
    if (registered_all_stop_hotkey_bind_ != 0) {
      UnregisterHotKey(window_handle_, kAllStopHotkeyId);
      registered_all_stop_hotkey_bind_ = 0;
    }

    if (hotkey != 0) {
      UINT modifiers = 0;
      if ((hotkey & Setting::kShiftKeyBind) != 0) {
        modifiers |= MOD_SHIFT;
      }
      if ((hotkey & Setting::kControlKeyBind) != 0) {
        modifiers |= MOD_CONTROL;
      }
      if ((hotkey & Setting::kAlternateKeyBind) != 0) {
        modifiers |= MOD_ALT;
      }

      if (RegisterHotKey(window_handle_, kAllStopHotkeyId, modifiers, hotkey & 0x00ff)) {
        registered_all_stop_hotkey_bind_ = hotkey;
      } else {
        MessageBox(window_handle_, string_table_->get(IDS_ERROR_REGISTER_ALL_STOP_HOTKEY), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
      }
    }
  }
}

void MainWindow::UnregisterHotkeys() {
  // ���ɋC�ɂ���UnregisterHotkey��A�ł��Ă����v�ȋC�����邯�ǈꉞ���䂷��

  if (registered_all_stop_hotkey_bind_ != 0) {
    UnregisterHotKey(window_handle_, kAllStopHotkeyId);
    registered_all_stop_hotkey_bind_ = 0;
  }

  if (registered_midi_input_hotkey_bind_ != 0) {
    UnregisterHotKey(window_handle_, kMidiInputHotkeyId);
    registered_midi_input_hotkey_bind_ = 0;
  }

  if (registered_play_hotkey_bind_ != 0) {
    UnregisterHotKey(window_handle_, kPlayHotkeyId);
    registered_play_hotkey_bind_ = 0;
  }
}

void MainWindow::RefreshStatus() {
  // MIDI�e�X�g�Đ��{�^���̕\��
  if (sequencer_manager_->IsPlaying() && sequencer_manager_->GetOutput() == Sequencer::Output::kMidiSynthesizer) {
    SetWindowText(test_play_button_handle_, string_table_->get(IDS_MAINWINDOW_STOP_BUTTON));
  } else {
    SetWindowText(test_play_button_handle_, string_table_->get(IDS_MAINWINDOW_TEST_PLAY_BUTTON));
  }

  if (sequencer_manager_->IsPlaying()) {
    // �X�e�[�^�X�\��
    if (sequencer_manager_->GetOutput() == Sequencer::Output::kMidiSynthesizer) {
      // MIDI�e�X�g�Đ���
      SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_MIDI_FILE_TEST_PLAYING));
    } else {
      // �Đ���
      SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_PLAYING));
    }
  } else if (midi_input_to_keyboard_->IsOpened()) {
    // MIDI���͎�t��
    SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_MIDI_INPUTING));
  } else {
    // ��~��
    SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_STOPPED));
  }
}

}  // namespace keysequencer