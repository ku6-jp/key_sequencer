#ifndef KEYSEQUENCER_MAINWINDOW_H_
#define KEYSEQUENCER_MAINWINDOW_H_

#include <Windows.h>
#include <tchar.h>

#include "setting.h"

namespace keysequencer {

class StringTable;
class MidiDeviceInputToKeyboard;
class SequencerManager;

/**
 * @brief メインウィンドウ
 */
class MainWindow {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  MainWindow() = delete;

  /**
   * @brief コンストラクタ
   *
   * @param instance_handle アプリケーションのインスタンスハンドル
   * @param string_table リソースの文字列テーブル
   * @param setting 設定
   * @param midi_input_to_keyboard MIDI入力をキーボード入力に変換する処理
   * @param sequencer_manager 自動演奏処理
   */
  MainWindow(HINSTANCE instance_handle, const StringTable* string_table, Setting* setting, MidiDeviceInputToKeyboard* midi_input_to_keyboard, SequencerManager* sequencer_manager);

  /**
   * @brief コピーコンストラクタ
   */
  MainWindow(const MainWindow&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  MainWindow(MainWindow&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  MainWindow& operator=(const MainWindow&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  MainWindow& operator=(MainWindow&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~MainWindow() = default;

  /**
   * @brief ウィンドウの生成処理
   * 
   * ウィンドウ生成の成否を判定するためにコンストラクタとは別にしたウィンドウ生成処理
   * 1度だけ実行する
   * もし2回目以降の呼び出しをした場合は何もしない
   *
   * @return 正常に終了した場合にtrueを返す
   */
  bool Create();

  /**
   * @brief ウィンドウのメッセージ処理
   *
   * ウィンドウが存在する限り処理を継続する
   * ウィンドウが閉じられた時に処理が返る
   */
  void Run();

 private:
  /**
   * @brief Window classを登録する時の名称
   */
  constexpr static const TCHAR* kWindowClassName = _T("MainWindow");

  /**
   * @brief クライアント領域の幅
   */
  constexpr static int kClientWidth = 480;

  /**
   * @brief クライアント領域の高さ
   */
  constexpr static int kClientHeight = 8 + 30 + 8 + 30 + 8 + 30 + 8;

  /**
   * @brief キーバインド設定ボタンのID
   */
  constexpr static WORD kKeyBindButtonId = 100;

  /**
   * @brief テスト再生/停止ボタンのID
   */
  constexpr static WORD kTestPlayButtonId = 101;

  /**
   * @brief Hotkey: 再生/停止登録用のID
   */
  constexpr static int kPlayHotkeyId = 100;

  /**
   * @brief Hotkey: MIDI入力登録用のID
   */
  constexpr static int kMidiInputHotkeyId = 101;

  /**
   * @brief Hotkey: 全停止
   */
  constexpr static int kAllStopHotkeyId = 102;

  /**
   * @brief リソースの文字列テーブル
   */
  const StringTable* const string_table_;

  /**
   * @brief アプリケーションのインスタンスハンドル
   */
  HINSTANCE const instance_handle_;

  /**
   * @brief 設定
   */
  Setting* const setting_;

  /**
   * @brief MIDI入力処理
   */
  MidiDeviceInputToKeyboard* const midi_input_to_keyboard_;

  /**
   * @brief 演奏処理
   */
  SequencerManager* const sequencer_manager_;

  /**
   * @brief ウィンドウ生成済みフラグ
   */
  bool created_ = false;

  /**
   * @brief ウィンドウハンドル
   */
  HWND window_handle_ = nullptr;

  /**
   * @brief ファイル情報表示のstatic textハンドル
   */
  HWND file_text_handle_ = nullptr;

  /**
   * @brief 再生ステータス表示のstatic textハンドル
   */
  HWND status_text_handle_ = nullptr;

  /**
   * @brief キーバインドを開くbuttonのハンドル
   */
  HWND open_key_bind_button_hanndle_ = nullptr;

  /**
   * @brief テスト再生/停止ボタンハンドル
   */
  HWND test_play_button_handle_ = nullptr;

  /**
   * @brief 登録された再生/停止ホットキーバインド
   */
  Setting::KeyBind registered_play_hotkey_bind_ = 0;

  /**
   * @brief 登録されたMIDI入力ホットキーバインド
   */
  Setting::KeyBind registered_midi_input_hotkey_bind_ = 0;

  /**
   * @brief 登録された全停止ホットキーバインド
   */
  Setting::KeyBind registered_all_stop_hotkey_bind_ = 0;

  /**
   * @brief Window classの登録を初回に1度だけ行う処理用の関数
   *
   * Window classの登録を行う
   *
   * @param instanceHandle アプリケーションのインスタンスハンドル
   *
   * @return 固定で0を返す
   */
  static int RegisterWindowClass(HINSTANCE instance_handle);

  /**
   * @brief システムからコールバックされるウィンドウプロシージャ
   */
  static LRESULT WINAPI CallbackProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

  /**
   * @brief ウィンドウプロシージャ処理内容
   * 
   * Windowsから呼び出されたCallbackProcedure内でインスタンスを解決しこちらに処理を委譲する
   * よって、引数と戻り値はウィンドウプロシージャの仕様に従う
   * 
   * @param window_handle ウィンドウハンドル
   * @param message メッセージ
   * @param wparam メッセージパラメータ1
   * @param lparam メッセージパラメータ2
   * 
   * @return 結果
   */
  LRESULT Procedure(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);

  /**
   * @brief ホットキーを登録する
   */
  void RegisterHotkeys();

  /**
   * @brief ホットキーを解除する
   */
  void UnregisterHotkeys();

  /**
   * @brief 演奏状態を元に表示を切り替える
   */
  void RefreshStatus();
};

}  // namespace keysequencer

#endif