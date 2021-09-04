#include "sequencer_manager.h"

#include <process.h>

#include "constants.h"
#include "critical_section_lock.h"

namespace keysequencer {

SequencerManager::SequencerManager(const Setting* setting, Keyboard* keyboard) : sequencer_(setting, keyboard) {
  // Critical section初期化
  InitializeCriticalSection(&critical_section_);

  // スレッド動作用のイベントを作成
  thread_active_event_ = CreateEvent(nullptr, FALSE, FALSE, _T("Active"));
}

SequencerManager::~SequencerManager() {
  // デストラクタに来る前に手動で止めているはずだが最終手段として
  ShutdownThread();

  // スレッド動作用のイベントを終了
  CloseHandle(thread_active_event_);

  // Critical sectionを削除
  DeleteCriticalSection(&critical_section_);
}

void SequencerManager::BeginThread(HWND main_window_handle) {
  // 既に開始していたら何もしない
  if (thread_handle_ != nullptr) {
    return;
  }

  // クラスの初期化でセットしているが一応明示的にスレッド終了フラグをオフに
  thread_close_ = false;

  // 通知先となるメインウィンドウのハンドルを保存
  main_window_handle_ = main_window_handle;

  // スレッド開始
  thread_handle_ = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, Thread, this, 0, nullptr));
}

void SequencerManager::ShutdownThread() {
  if (thread_handle_ == nullptr) {
    return;
  }

  {
    CriticalSectionLock lock(&critical_section_);

    // スレッド終了をセット
    thread_close_ = true;

    // スレッドを動かす
    SetEvent(thread_active_event_);
  }

  // スレッド終了を待つ(終了処理なので無限に待つ)
  WaitForSingleObject(thread_handle_, INFINITE);

  // スレッドハンドルを閉じる
  CloseHandle(thread_handle_);

  thread_handle_ = nullptr;
}

Sequencer::LoadFileResult SequencerManager::LoadFile(const TCHAR* const filepath) {
  CriticalSectionLock lock(&critical_section_);

  return sequencer_.LoadFile(filepath);
}

bool SequencerManager::Play(Sequencer::Output const output) {
  CriticalSectionLock lock(&critical_section_);

  // ファイルが読み込まれていなければ再生もできない
  if (!sequencer_.IsFileLoaded()) {
    return false;
  }

  // 再生開始
  if (!sequencer_.Open(output)) {
    return false;
  }
  playing_ = true;
  SetEvent(thread_active_event_);

  return true;
}

void SequencerManager::Stop() {
  CriticalSectionLock lock(&critical_section_);

  // 再生フラグを停止に設定
  // 演奏スレッドが動作していたら(イベントがアクティブなら)これにより停止する
  // 演奏スレッドが動作していなかったら(イベントが非アクティブなら)元々止まっているので問題無く
  // 次にアクティブになる時も再生開始時のためそちらで処理されるので問題無い
  playing_ = false;
}

unsigned __stdcall SequencerManager::Thread(void* object) {
  // インスタンスの処理に委譲する
  reinterpret_cast<SequencerManager*>(object)->RunThread();
  return 0;
}

void SequencerManager::RunThread() {
  // スリープ時間(ミリ秒)
  DWORD sleep_millisecond = INFINITE;

  for (;;) {
    // スリープ時間が1ミリ秒以上設定されていたらスリープ処理を入れる
    if (sleep_millisecond != 0) {
      WaitForSingleObject(thread_active_event_, sleep_millisecond);
      sleep_millisecond = 0;
    }

    // スレッド終了が指定されていたら終了
    if (thread_close_) {
      return;
    }

    {
      CriticalSectionLock lock(&critical_section_);

      if (!playing_) {
        // 演奏処理を閉じる
        sequencer_.Close();

        // スレッド停止
        sleep_millisecond = INFINITE;

        // メインウィンドウに表示更新メッセージを送信
        PostMessage(main_window_handle_, WM_APP_REFRESH_STATUS, 0, 0);
        continue;
      }

      // 演奏処理を行い、終端まで達したら停止をセット
      if (!sequencer_.Run(&sleep_millisecond)) {
        playing_ = false;
        sleep_millisecond = 0;
      }
    }
  }
}

}  // namespace keysequencer