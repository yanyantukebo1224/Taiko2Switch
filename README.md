# Taiko2Switch

Nintendo Switch向けのTJAリズムゲームプロジェクトです。現在は **YataiDONを基盤にしたSwitch版への移行準備** を進めています。

## 方針

本プロジェクトは [YataiDON](https://github.com/Yonokid/YataiDON) の既存Nintendo Switch実装を上流として活用します。譜面エンジン・ゲーム画面・選曲・スコア・スキン機能を再利用し、Switch固有の描画・入力・音声・ファイルアクセスは上流の `platform/switch/` 実装を起点に検証・改善します。

優先事項:

- 日本語UIと日本語譜面・ファイル名の扱いやすさ
- Switch実機でのフレーム安定性、入力遅延、音声同期
- Joy-Con / Pro Controller の操作性
- SDカード上の曲・スキン管理
- USB太鼓コントローラーは機種ごとに互換性を検証

## 現在の状態

このブランチは移行準備段階です。旧Taiko2Switchの簡易NROスケルトンとTJA下書き生成ツールは残っていますが、YataiDON本体のソースをまだ統合していません。したがって、現時点でYataiDONベースの完成版NROがビルドできるとは限りません。

上流のSwitch版READMEでは、起動・コントローラー入力・設定・ネイティブキーボード・音声・動画の実機動作が報告されています。一方、60 FPSの持続や大規模な曲/スキン構成でのメモリ使用量などは、こちらの対象環境で再検証が必要です。

## 上流プロジェクト

- Repository: https://github.com/Yonokid/YataiDON
- Upstream branch: `master`
- Upstream license: GNU GPL-3.0
- Switch port notes: [platform/switch/README.md](https://github.com/Yonokid/YataiDON/blob/master/platform/switch/README.md)

上流コードを取り込む場合は、GPL-3.0の条件、著作権表示、ライセンス文書を維持します。依存ライブラリはそれぞれのライセンスにも従います。

## ビルド

YataiDONのSwitch版はdevkitPro/devkitA64、libnx、Switch向けSDL2、Mesa-switch/NVK、FFmpeg、bgfx用host shadercなど複数の依存環境を必要とします。上流の手順を確認し、依存バージョンを固定したGitHub Actions環境を整備する予定です。

上流のビルド手順: [YataiDON Switch build guide](https://github.com/Yonokid/YataiDON/blob/master/platform/switch/README.md)

## 旧ツール

`tools/tja_autogen.py` は音声からTJAの下書きを作る補助ツールです。生成結果は自動採譜の完成品ではないため、BPM・オフセット・譜面内容を確認してください。

## License

GNU General Public License v3.0. See [LICENSE](LICENSE).
