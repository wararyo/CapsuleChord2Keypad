# CapsuleChord2 設定ツール

このツールは、CapsuleChord2キーパッドのボタン感度設定やシリアルナンバーをPC上から管理するためのユーティリティです。シリアル接続を通じて設定の読み取りと書き込みを行い、設定をJSONファイルとして保存・読み込みできます。

## セットアップ

1. Pythonが必要です（Python 3.6以上を推奨）
2. 仮想環境を使用しています：

```bash
# 仮想環境を有効化
cd /path/to/CapsuleChord2Keypad/tools
source venv/bin/activate  # Linuxまたは macOS
# Windowsの場合: venv\Scripts\activate
# パッケージをインストール
pip install pyserial
```

## 基本的な使い方

### 利用可能なシリアルポートを表示する

```bash
./config_tool.py --list-ports
# または
./config_tool.py -l
```

### デバイスから設定を読み取る

```bash
./config_tool.py --port /dev/ttyUSB0 --read
# または
./config_tool.py -p /dev/ttyUSB0 -r
```

※ `/dev/ttyUSB0` の部分は実際のシリアルポート名に置き換えてください。

### 設定をJSONファイルにエクスポートする

```bash
./config_tool.py -p /dev/ttyUSB0 -r -e my_settings.json
```

### JSONファイルから設定を読み込み、デバイスに書き込む

```bash
./config_tool.py -p /dev/ttyUSB0 -i my_settings.json -w
```

### 変更をデバイスのフラッシュメモリに保存する

```bash
./config_tool.py -p /dev/ttyUSB0 -i my_settings.json -w -s
```

## 完全なコマンドライン引数リスト

```
オプション:
  -h, --help                ヘルプを表示して終了
  
シリアルポート設定:
  -p, --port PORT           使用するシリアルポート
  -b, --baud BAUD           ボーレート（デフォルト: 115200）
  -l, --list-ports          利用可能なシリアルポートを表示
  
アクション:
  -r, --read                デバイスから設定を読み取る
  -w, --write               デバイスに設定を書き込む
  -s, --save                書き込み後に設定をフラッシュに保存する
  
ファイル:
  -i, --import-file FILE    JSONファイルから設定をインポート
  -e, --export-file FILE    設定をJSONファイルにエクスポート
```

## 設定JSONファイルの形式

JSONファイルは以下のような形式になっています：

```json
{
  "serial_number": "CC2P0001",
  "thresholds": {
    "KEY_LEFT_1": 3800,
    "KEY_LEFT_2": 3750,
    "KEY_LEFT_3": 3750,
    "KEY_LEFT_4": 3800,
    "KEY_LEFT_5": 3750,
    "KEY_LEFT_6": 3750,
    "KEY_LEFT_7": 3800,
    "KEY_LEFT_8": 3750,
    "KEY_LEFT_9": 3750,
    "KEY_RIGHT_1": 3800,
    "KEY_RIGHT_2": 3750,
    "KEY_RIGHT_3": 3750,
    "KEY_RIGHT_4": 3800,
    "KEY_RIGHT_5": 3750,
    "KEY_RIGHT_6": 3750,
    "KEY_RIGHT_7": 3800,
    "KEY_RIGHT_8": 3750,
    "KEY_RIGHT_9": 3750,
    "KEY_L": 3800,
    "KEY_R": 3800,
    "KEY_LT": 3800,
    "KEY_RT": 3800
  }
}
```

## 使用例

### 複数のデバイスを同じ設定で構成する

1. 1台目のデバイスから設定を読み取る：
   ```bash
   ./config_tool.py -p /dev/ttyUSB0 -r -e reference_settings.json
   ```

2. その設定を他のデバイスに適用する：
   ```bash
   ./config_tool.py -p /dev/ttyUSB1 -i reference_settings.json -w -s
   ```

### シリアルナンバーのみを変更する

1. 現在の設定を読み取る：
   ```bash
   ./config_tool.py -p /dev/ttyUSB0 -r -e temp_settings.json
   ```

2. JSONファイルを編集してシリアルナンバーを変更する

3. 変更した設定を書き込む：
   ```bash
   ./config_tool.py -p /dev/ttyUSB0 -i temp_settings.json -w -s
   ```

### 特定のキーの閾値のみを変更する

1. 現在の設定を読み取る：
   ```bash
   ./config_tool.py -p /dev/ttyUSB0 -r -e temp_settings.json
   ```

2. JSONファイルを編集して特定のキーの閾値を変更する

3. 変更した設定を書き込む：
   ```bash
   ./config_tool.py -p /dev/ttyUSB0 -i temp_settings.json -w -s
   ```

## 注意事項

- フラッシュメモリへの書き込みには寿命があるため、テスト中は `-s` オプションを頻繁に使用せず、最終的な設定が決まったときだけ使用することをお勧めします。
- 閾値の適切な範囲は通常 3700～3850 程度です。値が高すぎるとボタンが勝手に押されたと認識され、低すぎるとボタンが反応しなくなります。
- シリアルナンバーは個体識別に使用されます。各デバイスに一意のシリアルナンバーを設定することをお勧めします。