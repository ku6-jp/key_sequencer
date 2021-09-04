#ifndef KEYSEQUENCER_SEQUENCERMANAGER_H_
#define KEYSEQUENCER_SEQUENCERMANAGER_H_

#include <Windows.h>
#include <tchar.h>

#include "sequencer.h"

namespace keysequencer {

class Setting;
class Keyboard;

/**
 * @brief 演奏処理の管理クラス
 * 
 * 演奏処理はこのクラスを通して行う
 */
class SequencerManager {
 public:
  /**
   * @brief デフォルトコンストラクタ
   */
  SequencerManager() = delete;

  /**
   * @brief コンストラクタ
   *
   * @param setting 設定
   * @param keyboard キーボード
   */
  SequencerManager(const Setting* setting, Keyboard* keyboard);

  /**
   * @brief コピーコンストラクタ
   */
  SequencerManager(const SequencerManager&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  SequencerManager(SequencerManager&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  SequencerManager& operator=(const SequencerManager&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  SequencerManager& operator=(SequencerManager&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~SequencerManager();
  
  /**
   * @brief 演奏しているか確認する
   * 
   * @return 演奏中の場合trueを返す
   */
  inline bool IsPlaying() const {
    return playing_;
  }

  /**
   * @brief 出力先を取得
   * 
   * Stop()しても値をクリアしたりはしないので、Play() ～ Stop()間以外では使用しないこと(IsPlaying()で確認する)
   * 
   * @return 出力先
   */
  inline Sequencer::Output GetOutput() const {
    return sequencer_.GetOutput();
  }

  /**
   * @brief 演奏用のスレッドを開始する
   * 
   * 演奏情報の通知をメインウィンドウに対して行うためにウィンドウハンドルが必要
   * ウィンドウハンドルが取れるようになる(メインウィンドウが生成される)タイミングが少し遅いのでコンストラクタとは別に開始処理を設ける
   * もし開始後に再度呼び出した場合は何もしない
   *
   * @param main_window_handle メインウィンドウハンドル
   */
  void BeginThread(HWND main_window_handle);

  /**
   * @brief 演奏用のスレッドを終了する
   * 
   * オブジェクトの破棄タイミングの前に終了を行いたいためデストラクタとは別に終了処理を設ける
   * スレッドに対して終了要求を行った上で終了を無限に待つ
   */
  void ShutdownThread();

  /**
   * @brief ファイル(SMF)を読み込む
   *
   * @param filepath ファイルパス
   *
   * @return 読み込み結果
   */
  Sequencer::LoadFileResult LoadFile(const TCHAR* filepath);

  /**
   * 演奏を開始する(初めから)
   *
   * @param output 出力先
   *
   * @return 再生開始処理が正常に行えた場合にtrueを返す
   */
  bool Play(Sequencer::Output const output);

  /**
   * 演奏を停止する
   */
  void Stop();

 private:
  /**
   * @brief スレッド終了フラグ
   *
   * スレッドを終了する場合にtrueにする
   */
  bool thread_close_ = false;

  /**
   * @brief 演奏中フラグ
   */
  bool playing_ = false;

  /**
   * @brief メインウィンドウのハンドル
   *
   * メインウィンドウにメッセージを送る際に使用する
   */
  HWND main_window_handle_ = nullptr;

  /**
   * @brief 演奏スレッドのハンドル
   */
  HANDLE thread_handle_ = nullptr;

  /**
   * @brief 演奏スレッド制御用のシグナルハンドル
   *
   * 再生時にシグナル状態にしてスレッドを動かし
   * 停止時またはウェイト時に非シグナル状態にしてスレッドをスリープさせる
   * 
   * 演奏の待機処理は本来マルチメディアタイマーを使いたいのだが
   * マルチメディアタイマーの動作がスレッド制御と合わせにくいので諦めて素直にこちらで処理する
   * 
   * 一応こちらのタイマーもマルチメディアタイマーの精度設定の影響にあるらしい？
   */
  HANDLE thread_active_event_ = nullptr;

  /**
   * @brief スレッド間排他制御用のクリティカルセクション
   */
  CRITICAL_SECTION critical_section_;

  /**
   * @brief 演奏処理本体 
   */
  Sequencer sequencer_;

  /**
   * @brief スレッド起動時に呼び出される関数
   *
   * _beginthreadexに指定するための関数
   * 実際の処理はthreadRunに移譲する
   * 
   * @param object _beginthreadexで渡したユーザーオブジェクト(このクラスのインスタンス)
   */
  static unsigned __stdcall Thread(void* object);

  /**
   * @brief 演奏スレッド処理本体
   *
   * 演奏スレッドはメインウィンドウ作成時に起動され、演奏していない時には待機している
   */
  void RunThread();
};
}  // namespace keysequencer

#endif