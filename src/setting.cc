#include "setting.h"

#include <Shlwapi.h>

#include <cstdio>

namespace keysequencer {

Setting::Setting() {
  for (std::uint32_t i = 0; i < MAX_PATH; ++i) {
    ini_file_path_[i] = 0;
  }

  for (std::uint32_t i = 0; i < KeyBindIndex::kLength; ++i) {
    key_binds_[i] = 0;
  }
}

bool Setting::LoadIniFile() {
  TCHAR buffer[8];

  // 読み込み完了判定に使えるようクリアする(一応全体を)
  for (std::uint32_t i = 0; i < MAX_PATH; ++i) {
    ini_file_path_[i] = 0;
  }

  // 実行ファイルのフルパス名を取得する
  if (GetModuleFileName(NULL, ini_file_path_, MAX_PATH) == 0) {
    return false;
  }

  // 同じフォルダに同じ名前で拡張子だけ.iniに変える
  if (!PathRenameExtension(ini_file_path_, _T(".ini"))) {
    return false;
  }

  if (PathFileExists(ini_file_path_)) {
    // .iniファイルが存在する場合はそれを読み出す
    for (std::size_t i = 0; i < KeyBindIndex::kLength; ++i) {
      // .iniファイルから0x0000形式の6文字を読み取る
      // 6文字 + NULL分の1文字に加えて6文字以上の文字列検出するためにさらに1文字余分に確保し8文字分とする
      if (GetPrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[i], _T(""), buffer, sizeof(buffer) / sizeof(buffer[0]), ini_file_path_) != 6) {
        key_binds_[i] = 0;
        continue;
      }

      // 文字列から整数に変換
      TCHAR* end;
      KeyBind result = static_cast<KeyBind>(_tcstoul(buffer, &end, 16));
      if (*end == 0) {
        // .iniファイルはユーザーが書き換え出来るので一応ロードしたデータもチェックする

        // 上位1バイトにゴミが混入しないよう一応クリアする
        result &= kShiftKeyBind | kControlKeyBind | kAlternateKeyBind | 0x00ff;

        // 同じキーが使用済みならばクリアしてしまう
        for (std::size_t j = 0; j < KeyBindIndex::kLength; ++j) {
          if (key_binds_[j] == result) {
            key_binds_[j] = 0;
          }
        }

        key_binds_[i] = result;
      } else {
        key_binds_[i] = 0;
      }
    }

    // ノートの表記方式を.iniファイルから取得
    if (GetPrivateProfileInt(kOptionalIniSection, kNoteLabelStyleIniKey, 0, ini_file_path_) == static_cast<int>(NoteLabelStyle::kYamaha)) {
      note_label_style_ = NoteLabelStyle::kYamaha;
    } else {
      note_label_style_ = NoteLabelStyle::kEnglish;

      // ノート表記方式はOptionalかつ設定画面で変更出来るので.iniファイルにキーが無くても気にしない
    }

    // キーを離すタイミングを.iniファイルから読み取り
    early_key_release_millisecond_ = GetPrivateProfileInt(kOptionalIniSection, kEarlyKeyReleaseMillisecondIniKey, 0, ini_file_path_);

    // .iniファイルにキーを出力するために書き込む(ユーザーが編集出来るように)
    // デフォルト値でなければ既に書き込んであるので何もしない
    if (early_key_release_millisecond_ == 0) {
      WritePrivateProfileString(kOptionalIniSection, kEarlyKeyReleaseMillisecondIniKey, _T("0"), ini_file_path_);
    }

    // キーを離すタイミングを.iniファイルから読み取り
    modifier_key_delay_millisecond_ = GetPrivateProfileInt(kOptionalIniSection, kModifierKeyDelayMillisecondIniKey, 50, ini_file_path_);

    // .iniファイルにキーを出力するために書き込む(ユーザーが編集出来るように)
    // デフォルト値でなければ既に書き込んであるので何もしない
    if (modifier_key_delay_millisecond_ == 50) {
      WritePrivateProfileString(kOptionalIniSection, kModifierKeyDelayMillisecondIniKey, _T("50"), ini_file_path_);
    }

    // 同一修飾キー押下を一つにまとめる時間を.iniファイルから読み取り
    modifier_key_down_join_millisecond_ = GetPrivateProfileInt(kOptionalIniSection, kModifierKeyDownJoinMillisecondIniKey, 1000, ini_file_path_);

    // .iniファイルにキーを出力するために書き込む(ユーザーが編集出来るように)
    // デフォルト値でなければ既に書き込んであるので何もしない
    if (modifier_key_down_join_millisecond_ == 1000) {
      WritePrivateProfileString(kOptionalIniSection, kModifierKeyDownJoinMillisecondIniKey, _T("1000"), ini_file_path_);
    }
  } else {
    // 存在しない場合はデフォルトの設定を使用する(同時にデフォルト設定で.iniファイルを作成する)

    // デフォルトのキーバインドをセット(と書き込み)
    for (std::size_t i = 0; i < KeyBindIndex::kLength; ++i) {
      key_binds_[i] = kDefaultKeyBinds[i];

      // .iniファイルに書き込み
      _stprintf_s(buffer, sizeof(buffer) / sizeof(buffer[0]), _T("0x%04x"), key_binds_[i]);
      WritePrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[i], buffer, ini_file_path_);
    }

    // ノート表記方式のデフォルト値
    // ノート表記方式はOptionalかつ設定画面で変更出来るので.iniファイルにキーが無くても気にしない
    note_label_style_ = NoteLabelStyle::kEnglish;

    // ノートOFFイベント時刻より何ミリ秒早くキーを離すかのデフォルト値を設定
    early_key_release_millisecond_ = 0;

    // .iniファイルにキーを出力するために書き込む(ユーザーが編集出来るように)
    // デフォルト値でなければ既に書き込んであるので何もしない
    WritePrivateProfileString(kOptionalIniSection, kEarlyKeyReleaseMillisecondIniKey, _T("0"), ini_file_path_);

    // 修飾キーをノートONから何ミリ秒離してアップダウンするかのデフォルト値を設定(50ms = 20fpsでも拾える値)
    modifier_key_delay_millisecond_ = 50;

    // .iniファイルにキーを出力するために書き込む(ユーザーが編集出来るように)
    // デフォルト値でなければ既に書き込んであるので何もしない
    WritePrivateProfileString(kOptionalIniSection, kModifierKeyDelayMillisecondIniKey, _T("50"), ini_file_path_);

    // 同一修飾キー押下を一つにまとめる時間のデフォルト値を設定
    modifier_key_down_join_millisecond_ = 1000;

    // .iniファイルにキーを出力するために書き込む(ユーザーが編集出来るように)
    // デフォルト値でなければ既に書き込んであるので何もしない
    WritePrivateProfileString(kOptionalIniSection, kModifierKeyDownJoinMillisecondIniKey, _T("1000"), ini_file_path_);
  }

  return true;
}

void Setting::SetKeyBind(std::size_t index, KeyBind key_bind) {
  if (ini_file_path_[0] == 0) {
    return;
  }

  if (key_bind != 0) {
    // 上位1バイトにゴミが混入しないよう一応クリアする
    key_bind &= kShiftKeyBind | kControlKeyBind | kAlternateKeyBind | 0x00ff;

    // 同じキーが使用済みならばクリアしてしまう
    for (std::size_t i = 0; i < KeyBindIndex::kLength; ++i) {
      if (key_binds_[i] == key_bind) {
        key_binds_[i] = 0;

        // .iniファイルに書き込み
        WritePrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[i], _T("0"), ini_file_path_);
      }
    }
  }

  // 新しい設定をセット
  key_binds_[index] = key_bind;

  // 整数から文字列に変換
  TCHAR buffer[7];
  _stprintf_s(buffer, sizeof(buffer) / sizeof(buffer[0]), _T("0x%04x"), key_bind);

  // .iniファイルに書き込み
  WritePrivateProfileString(kKeyBindsIniSection, kKeyBindIniKeys[index], buffer, ini_file_path_);
}

void Setting::SetNoteLabelStyle(NoteLabelStyle note_label_style) {
  if (ini_file_path_[0] == 0) {
    return;
  }

  note_label_style_ = note_label_style;

  if (note_label_style_ == NoteLabelStyle::kEnglish) {
    WritePrivateProfileString(kOptionalIniSection, kNoteLabelStyleIniKey, _T("0"), ini_file_path_);
  } else {
    WritePrivateProfileString(kOptionalIniSection, kNoteLabelStyleIniKey, _T("1"), ini_file_path_);
  }
}

}  // namespace keysequencer