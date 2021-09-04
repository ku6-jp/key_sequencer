# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [2.0.0] - 2021-09-04

### Added

- MIDIの全ての音階に対応するビルドバージョンを追加
    - 別実行ファイル(key_sequencer_full_note.exe)
- 全ての音階に対応するとキーが足りなくなってきたため、修飾キーの利用を復活
- 音階の表記を国際式とヤマハ式で切り替える機能を追加
    - 以前までのバージョンは国際式表記
    - 業界的にはヤマハ式の方が多そう

### Changed

- ウィンドウ背景色がWindowsのボタン色の設定に従うよう変更
    - レジストリ「\HKEY_CURRENT_USER\Control Panel\Colors\ButtonFace」の設定値
        - Windows10だとレジストリ直接編集以外の設定方法は無い模様
        - 古いWindowsはコントロールパネルから変更出来た気がする

### Fixed

- SMFファイルの読み込みで複数トラック間のイベントの処理順の誤りを修正
- SMFファイルの読み込みでSysExイベントのデータ長読み取りの誤りを修正
- 特定環境でキーバインド時にハングアップする問題を修正(IMEを無効化)


## [1.3.1] - 2018-05-25

このバージョンはGitHub移行前のバージョンであり、Gitには含まれない。<br>
現在以下のURLで公開しているが、将来的に公開停止される予定。

- https://ku6.jp/archive/key_sequencer_1_3_1.zip

### Added

- MIDI(SMF)再生時にキーを離す(ノートOFF)動作をMIDI(SMF)で指定された時間より早く行うオプションを追加
    - 何ミリ秒早く離すかを.iniファイルのOptionalセクションEarlyKeyReleaseMillisecondsに設定する


## [1.3.0] - 2018-05-22

このバージョンはGitHub移行前のバージョンであり、Gitには含まれない。<br>
現在以下のURLで公開しているが、将来的に公開停止される予定。

- https://ku6.jp/archive/key_sequencer_1_3_0.zip

### Added

- キーを押す長さに対応(MIDIのONからOFFの間キーボードが押されたままの状態となる)

### Removed

- 音階のキーバインドで修飾キーを使用不可に変更

### Fixed

- キーコンフィグ中もホットキーに登録済みのキーが有効となっていたため無効になるよう修正


## [1.2.0] - 2018-02-18

このバージョンはGitHub移行前のバージョンであり、Gitには含まれない。<br>
現在以下のURLで公開しているが、将来的に公開停止される予定。

- https://ku6.jp/archive/key_sequencer_1_2_0.zip

### Changed

- MIDI入力の開始と終了をHotkeyに変更
- MIDI入力は再生やMIDIテスト再生と排他処理になるよう変更
- Hotkeyとボタンの挙動を変更
    - 同じボタンを押したら停止、違うボタンを押したらその機能に切り替えて実行
- デフォルトのキーバインドを変更

### Removed

- MBCS版のバイナリを削除
    - 作るのが簡単なので付けていたが使い道も無いし混乱するのでむしろ邪魔なため
    - 万一必要になった場合はソースからビルドで入手可能


## [1.1.0] - 2017-11-24

このバージョンはGitHub移行前のバージョンであり、Gitには含まれない。<br>
現在以下のURLで公開しているが、将来的に公開停止される予定。

- https://ku6.jp/archive/key_sequencer_1_1_0.zip

### Added

- MIDI入力機器(MIDIキーボードなど)に対応
    - MIDI入力機器からの入力をキーボード入力に変換する
- ファイルをドロップして起動に対応

### Changed

- デフォルトのキーバインドを修正


## [1.0.0] - 2017-11-21

このバージョンはGitHub移行前のバージョンであり、Gitには含まれない。<br>
現在以下のURLで公開しているが、将来的に公開停止される予定。

- https://ku6.jp/archive/key_sequencer_1_0_0.zip

### Added

- 公開


[Unreleased]: https://github.com/ku6-jp/key_sequencer/compare/v2.0.0...HEAD
[2.0.0]: https://github.com/ku6-jp/key_sequencer/releases/tag/v2.0.0
