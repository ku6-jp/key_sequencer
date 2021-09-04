# 開発情報

アプリケーションをビルドしたり改変したりするための情報。

## ビルド環境

Visual Studio 2019を使用。

## C++ version

C++17に準拠する。

## 文字コード 改行コード

Visual C++がマルチバイト(SJIS)を使うことを強要して来るのでおとなしく従っておく。<br>
UTF-8にすることも可能ではあるが各所で発生する問題を解決する労力に見合うだけの理由は今のところ無い。

同様に改行コードもCRLFとする。

## コーディング規約

Google C++ スタイルガイドに従う: https://github.com/google/styleguide/

ただし、幅80文字の制限だけは気が狂いそうになるので適用しない。

ClangFormatにより整形を行う。(.clang-formatファイルを同梱)

## ビルド構成

通常版

| 構成 | プラットフォーム | 文字セット |
| ---- | ---- | ---- |
| Debug | Win32 | マルチバイト |
| Debug | x64 | Unicode |
| Release(MBCS) | Win32 | マルチバイト |
| Release(MBCS) | x64 | マルチバイト |
| Release(Unicode) | Win32 | Unicode |
| Release(Unicode) | x64 | Unicode |

全ノート対応版
| 構成 | プラットフォーム | 文字セット |
| ---- | ---- | ---- |
| Debug full note | Win32 | マルチバイト |
| Debug full note | x64 | Unicode |
| Release full note(MBCS) | Win32 | マルチバイト |
| Release full note(MBCS) | x64 | マルチバイト |
| Release full note(Unicode) | Win32 | Unicode |
| Release full note(Unicode) | x64 | Unicode |

