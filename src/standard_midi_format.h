#ifndef KEYSEQUENCER_STANDARDMIDIFORMAT_H_
#define KEYSEQUENCER_STANDARDMIDIFORMAT_H_

#include <tchar.h>

#include <cstddef>
#include <cstdint>

namespace keysequencer {

/**
 * @brief SMFファイルのデータを保持する
 * 
 * 解析処理は別クラスになっているのでこのクラスはファイルのバイナリデータを保持するのみ
 */
class StandardMidiFormat {
 public:
  /**
   * @brief 操作結果
   */
  enum class Result {
    /**
     * @brief 正常終了 
     */
    kSuccess,

    /**
     * @brief ファイルが開けない
     */
    kFailureFileOpen,

    /**
     * @brief 4GB以上のファイルは非対応
     */
    kFailureFileTooLarge,

    /**
     * @brief ファイル読み込み中に不明なエラー
     */
    kFailureFileRead,
  };

  /**
   * @brief デフォルトコンストラクタ
   */
  StandardMidiFormat() = default;

  /**
   * @brief コピーコンストラクタ
   */
  StandardMidiFormat(const StandardMidiFormat&) = delete;

  /**
   * @brief ムーブコンストラクタ
   */
  StandardMidiFormat(StandardMidiFormat&&) = delete;

  /**
   * @brief コピー代入演算子
   */
  StandardMidiFormat& operator=(const StandardMidiFormat&) = delete;

  /**
   * @brief ムーブ代入演算子
   */
  StandardMidiFormat& operator=(StandardMidiFormat&&) = delete;

  /**
   * @brief デストラクタ
   */
  ~StandardMidiFormat();

  /**
   * @brief ファイルを読み込む
   * 
   * 読み込みの成功・失敗に係わらず保持中のデータは破棄する
   *
   * @param filepath ファイルパス
   *
   * @return ファイル読み込み結果
   */
  Result LoadFile(const TCHAR* filepath);

  /**
   * @brief 読み込んだバイナリデータを取得
   * 
   * @return バイナリデータの先頭ポインタ
   *         LoadFileを呼び出すと無効となる
   */
  inline const std::uint8_t* GetData() const {
    return data_;
  }

  /**
   * @brief 読み込んだバイナリデータのサイズを取得 
   * 
   * @return バイナリデータのサイズ(Byte)
   */
  inline std::size_t GetLength() const {
    return length_;
  }

 private:
  /**
   * @brief バイナリデータ
   */
  std::uint8_t* data_ = nullptr;

  /**
   * @brief バイナリデータの長さ
   */
  std::size_t length_ = 0;
};

}  // namespace keysequencer
#endif