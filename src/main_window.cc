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
  // 1度だけ実行する
  if (created_) {
    return false;
  }
  created_ = true;

  // 作成するウィンドウのスタイル
  DWORD window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

  // クライアント領域からウィンドウサイズを計算
  int width = kClientWidth;
  int height = kClientHeight;
  RECT client_rect = {0, 0, width, height};
  if (AdjustWindowRect(&client_rect, window_style, FALSE)) {
    width = client_rect.right - client_rect.left;
    height = client_rect.bottom - client_rect.top;
  }

  // 画面中央を初期位置とする
  int x = GetSystemMetrics(SM_CXSCREEN);
  int y = GetSystemMetrics(SM_CYSCREEN);
  x = x > width ? (x - width) / 2 : 0;
  y = y > height ? (y - height) / 2 : 0;

  // Window作成
  window_handle_ = CreateWindowEx(0, kWindowClassName, string_table_->get(IDS_MAINWINDOW_CAPTION), window_style, x, y, width, height, nullptr, nullptr, instance_handle_, nullptr);
  if (window_handle_ == nullptr) {
    MessageBox(nullptr, string_table_->get(IDS_ERROR_CREATE_WINDOW), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
    return false;
  }

  // Window procedure用にthisポインタを記憶
  SetWindowLongPtr(window_handle_, GWLP_USERDATA, 0);
  if (SetWindowLongPtr(window_handle_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)) == 0 && GetLastError() != 0) {
    goto DESTROY;
  }

  // IMEを無効化する
  // メインウィンドウでは無効化しなくてもよいが有効である必要もないので他ウィンドウと合わせて無効とする
  ImmAssociateContext(window_handle_, nullptr);

  // 再生するファイル名を表示
  file_text_handle_ = CreateWindow(_T("STATIC"), string_table_->get(IDS_MAINWINDOW_FILE_EMPTY), WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 8, 8, kClientWidth - 16, 30, window_handle_, nullptr, instance_handle_, nullptr);
  if (file_text_handle_ == nullptr) {
    goto DESTROY;
  }

  // 再生ステータスを表示
  status_text_handle_ = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 8, 46, kClientWidth - 16, 30, window_handle_, nullptr, instance_handle_, nullptr);
  if (status_text_handle_ == nullptr) {
    goto DESTROY;
  }

  // キーバインド設定ボタンを表示
  open_key_bind_button_hanndle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_MAINWINDOW_KEY_BIND_BUTTON), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, 8, 84, 128, 30, window_handle_, reinterpret_cast<HMENU>(kKeyBindButtonId), instance_handle_, nullptr);
  if (open_key_bind_button_hanndle_ == nullptr) {
    goto DESTROY;
  }

  // テスト再生/停止ボタンを表示
  test_play_button_handle_ = CreateWindow(_T("BUTTON"), string_table_->get(IDS_MAINWINDOW_TEST_PLAY_BUTTON), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | BS_CENTER, 8 + 128 + 8, 84, 192, 30, window_handle_, reinterpret_cast<HMENU>(kTestPlayButtonId), instance_handle_, nullptr);
  if (test_play_button_handle_ == nullptr) {
    goto DESTROY;
  }

  // Windowを表示
  ShowWindow(window_handle_, SW_SHOW);

  // Drag & Dropを有効化
  DragAcceptFiles(window_handle_, TRUE);

  // Hotkey有効化
  RegisterHotkeys();

  // 演奏スレッドを開始
  // 演奏処理からウィンドウにメッセージを送るケースが有るためウィンドウハンドルが無いと開始できない
  sequencer_manager_->BeginThread(window_handle_);

  // 起動時にファイルがドロップされていたら読み込む
  if (__argc > 1 && __targv[1] != nullptr) {
    Sequencer::LoadFileResult result = sequencer_manager_->LoadFile(__targv[1]);
    if (result == Sequencer::LoadFileResult::kSuccess) {
      // ファイル名の長さを取得
      WORD filename_length = GetFileTitle(__targv[1], nullptr, 0);

      // ファイル名用のメモリを確保
      TCHAR* filename = new TCHAR[filename_length];  // こっちは終端のNULLもlengthに入ってる

      // ファイル名を取得
      GetFileTitle(__targv[1], filename, filename_length);

      // ファイルパスの表示を切り替え
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
          // 演奏中なら止める
          sequencer_manager_->Stop();

          // MIDI入力中なら止める
          midi_input_to_keyboard_->Close();

          RefreshStatus();

          // キーバインド設定ダイアログを起動
          KeyBindWindow* child = new KeyBindWindow(instance_handle_, window_handle, string_table_, setting_);
          if (child->Create()) {
            // 設定中にHotkeyも押すが処理はしないので一旦Hotkeyを停止する
            UnregisterHotkeys();

            // キーバインドウィンドウ動作中はメインウィンドウは無効化する(生きてると閉じるボタンが押せたりするけどメッセージを処理出来なくてフリーズしていると判断されてしまう)
            EnableWindow(window_handle, FALSE);

            child->Run();

            // メインウィンドウを有効に戻す
            EnableWindow(window_handle, TRUE);

            // メインウィンドウをアクティブにする
            SetActiveWindow(window_handle);

            // Hotkeyを戻す(変更されていた場合の更新も兼ねる)
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
              // MIDIテスト再生中ならば停止する
              sequencer_manager_->Stop();
            } else {
              // 再生中ならば再生を停止してMIDIテスト再生を開始する
              sequencer_manager_->Play(Sequencer::Output::kMidiSynthesizer);
            }
          } else if (midi_input_to_keyboard_->IsOpened()) {
            // MIDI入力受付中ならばMIDI入力を停止してテスト再生を開始する
            midi_input_to_keyboard_->Close();
            sequencer_manager_->Play(Sequencer::Output::kMidiSynthesizer);
          } else {
            // 停止中ならば再生を開始する
            sequencer_manager_->Play(Sequencer::Output::kMidiSynthesizer);
          }

          // 表示を更新
          RefreshStatus();
          return 0;
      }
      break;

    case WM_HOTKEY:
      if (wparam == kPlayHotkeyId) {
        if (sequencer_manager_->IsPlaying()) {
          if (sequencer_manager_->GetOutput() == Sequencer::Output::kMidiSynthesizer) {
            // MIDIテスト再生中ならばMIDIテスト再生を停止して再生を開始する
            sequencer_manager_->Play(Sequencer::Output::kKeyboard);
          } else {
            // 再生中ならば再生を停止する
            sequencer_manager_->Stop();
          }
        } else if (midi_input_to_keyboard_->IsOpened()) {
          // MIDI入力受付中ならばMIDI入力を停止して再生を開始する
          midi_input_to_keyboard_->Close();
          sequencer_manager_->Play(Sequencer::Output::kKeyboard);
        } else {
          // 停止中ならば再生を開始する
          sequencer_manager_->Play(Sequencer::Output::kKeyboard);
        }

        // 表示を更新
        RefreshStatus();
      } else if (wparam == kMidiInputHotkeyId) {
        // 再生中 or MIDIテスト再生中ならば停止してMIDI入力を開始する
        if (sequencer_manager_->IsPlaying()) {
          sequencer_manager_->Stop();

          MidiDeviceInputToKeyboard::OpenResult result = midi_input_to_keyboard_->Open();
          if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureDeviceNotFound) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_DEVICE_NOT_FOUND), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          } else if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureUnknown) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_UNKNOWN), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          }
        } else if (midi_input_to_keyboard_->IsOpened()) {
          // MIDI入力受付中ならば停止する
          midi_input_to_keyboard_->Close();
        } else {
          // 停止中ならばMIDI入力開始
          MidiDeviceInputToKeyboard::OpenResult result = midi_input_to_keyboard_->Open();
          if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureDeviceNotFound) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_DEVICE_NOT_FOUND), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          } else if (result == MidiDeviceInputToKeyboard::OpenResult::kFailureUnknown) {
            MessageBox(window_handle, string_table_->get(IDS_ERROR_MIDI_INPUT_UNKNOWN), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
          }
        }

        // 表示を更新
        RefreshStatus();
      } else if (wparam == kAllStopHotkeyId) {
        // 再生中 or MIDIテスト再生中ならば停止する
        if (sequencer_manager_->IsPlaying()) {
          sequencer_manager_->Stop();

          // 表示を更新
          RefreshStatus();
        }
        // MIDI入力受付中ならば停止する
        else if (midi_input_to_keyboard_->IsOpened()) {
          midi_input_to_keyboard_->Close();

          // 表示を更新
          RefreshStatus();
        }
      }
      return 0;

    case WM_DROPFILES: {
      HDROP handle = reinterpret_cast<HDROP>(wparam);

      // ドロップされたファイル(1個目)のパスの長さを取得
      UINT filepath_length = DragQueryFile(handle, 0, nullptr, 0);

      // ファイルパス用のメモリを確保
      TCHAR* filepath = new TCHAR[static_cast<size_t>(filepath_length) + 1];  // 終端のNULLはlengthに入らない

      // ファイルパスを取得
      DragQueryFile(handle, 0, filepath, filepath_length + 1);
      DragFinish(handle);

      // 演奏中なら止める
      sequencer_manager_->Stop();

      // MIDI入力中なら止める
      midi_input_to_keyboard_->Close();

      RefreshStatus();

      // ファイルを読み込み
      Sequencer::LoadFileResult result = sequencer_manager_->LoadFile(filepath);
      if (result == Sequencer::LoadFileResult::kSuccess) {
        // ファイル名の長さを取得
        WORD filenameLength = GetFileTitle(filepath, nullptr, 0);

        // ファイル名用のメモリを確保
        TCHAR* filename = new TCHAR[filenameLength];  // こっちは終端のNULLもlengthに入ってる

        // ファイル名を取得
        GetFileTitle(filepath, filename, filenameLength);

        // ファイルパスの表示を切り替え
        SetWindowText(file_text_handle_, filename);

        delete[] filename;
      } else {
        SetWindowText(file_text_handle_, string_table_->get(IDS_MAINWINDOW_FILE_EMPTY));

        switch (result) {
          case Sequencer::LoadFileResult::kFailureFileOpen:
            // ファイルがオープン出来ない
            MessageBox(window_handle, string_table_->get(IDS_ERROR_OPEN_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureFileTooLarge:
            // 4GB以上のファイルは非対応
            MessageBox(window_handle, string_table_->get(IDS_ERROR_TOO_LARGE_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureFileRead:
            // ファイル読み込み中に不明なエラー
            MessageBox(window_handle, string_table_->get(IDS_ERROR_READ_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureIllegalSmf:
            // SMFとして不正
            MessageBox(window_handle, string_table_->get(IDS_ERROR_ILLEGAL_SMF_FILE), string_table_->get(IDS_ERRORDIALOG_CAPTION), ERROR_DIALOG_TYPE);
            break;
          case Sequencer::LoadFileResult::kFailureNotSupportSmf:
            // サポートしていないSMF形式
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
      // ウィンドウを参照するものを先に解放してからDestroyWindowする
      UnregisterHotkeys();

      // 演奏処理スレッド停止
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
  // 特に気にせずUnregisterHotkeyを連打しても大丈夫な気がするけど一応制御する

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
  // MIDIテスト再生ボタンの表示
  if (sequencer_manager_->IsPlaying() && sequencer_manager_->GetOutput() == Sequencer::Output::kMidiSynthesizer) {
    SetWindowText(test_play_button_handle_, string_table_->get(IDS_MAINWINDOW_STOP_BUTTON));
  } else {
    SetWindowText(test_play_button_handle_, string_table_->get(IDS_MAINWINDOW_TEST_PLAY_BUTTON));
  }

  if (sequencer_manager_->IsPlaying()) {
    // ステータス表示
    if (sequencer_manager_->GetOutput() == Sequencer::Output::kMidiSynthesizer) {
      // MIDIテスト再生中
      SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_MIDI_FILE_TEST_PLAYING));
    } else {
      // 再生中
      SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_PLAYING));
    }
  } else if (midi_input_to_keyboard_->IsOpened()) {
    // MIDI入力受付中
    SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_MIDI_INPUTING));
  } else {
    // 停止中
    SetWindowText(status_text_handle_, string_table_->get(IDS_MAINWINDOW_STATUS_STOPPED));
  }
}

}  // namespace keysequencer