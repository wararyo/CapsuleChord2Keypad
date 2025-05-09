# CapsuleChord 2 キーパッドファームウェア

ユーザーが押下する操作に反応する物理的な装置を「ボタン」
ファームウェア内で扱う論理的な入力の最小単位を「キー」と呼ぶことにします

[Picoboot3](https://github.com/IndoorCorgi/picoboot3)を書き込んだ後に書き込んでください

以下は自動で書いてもらったドキュメントなので間違い等あるかもです

## シリアルコンソールによる設定

各ボタンの感度（閾値）はシリアル通信経由で調整できます。調整した値はフラッシュメモリに保存され、電源を切っても保持されます。

### シリアル通信の設定

- ボーレート: 115200
- データビット: 8
- パリティ: なし
- ストップビット: 1
- フロー制御: なし

### 利用可能なコマンド

シリアルターミナル（Screen、CoolTerm、Terraterm等）を使用して以下のコマンドを送信できます：

- `threshold` - すべてのキーの閾値を表示
- `threshold <key>` - 特定のキーの閾値を表示（例: `threshold 0x01`）
- `threshold <key> <value>` - 特定のキーの閾値を設定（例: `threshold 0x01 3800`）
- `serial` - 現在のシリアルナンバーを表示
- `serial <value>` - 新しいシリアルナンバーを設定（例: `serial CC2P0001`）
- `save` - 現在の設定をフラッシュメモリに保存
- `load` - フラッシュメモリから設定を読み込み
- `default` - デフォルト設定に戻す（保存はされない）
- `help` - コマンド一覧を表示

### 使用例

1. すべてのキーの現在の閾値を確認：
   ```
   threshold
   ```

2. 特定のキーの閾値を確認（例：左キーパッドの1）：
   ```
   threshold 0x01
   ```

3. 特定のキーの閾値を変更（例：左キーパッドの1の閾値を3800に設定）：
   ```
   threshold 0x01 3800
   ```

4. 現在のシリアルナンバーを確認：
   ```
   serial
   ```

5. シリアルナンバーを変更（例：シリアルナンバーをCC2K00001に設定）：
   ```
   serial CC2K00001
   ```

6. 変更した設定をフラッシュメモリに保存：
   ```
   save
   ```

7. デフォルト設定に戻す：
   ```
   default
   ```
   
   その後、保存するには：
   ```
   save
   ```

## I2C通信による制御

CapsuleChord 2キーパッドはI2Cスレーブデバイスとして動作し、キーイベントの取得、ボタンのアナログ値の読み取り、LEDの明るさ制御、およびボタン閾値の設定などが可能です。

### I2C通信の設定

- スレーブアドレス: 0x09
- 通信速度: 100kHz
- 接続ピン:
  - SDA: ボード上のSDAPIN（I2C通信データライン）
  - SCL: ボード上のSCLPIN（I2C通信クロックライン）

### キーコード一覧

I2C操作で使用するキーコード：

```
# 左キーパッド
0x01: KEY_LEFT_1
0x02: KEY_LEFT_2
0x03: KEY_LEFT_3
0x04: KEY_LEFT_4
0x05: KEY_LEFT_5
0x06: KEY_LEFT_6
0x07: KEY_LEFT_7
0x08: KEY_LEFT_8
0x09: KEY_LEFT_9

# 右キーパッド
0x11: KEY_RIGHT_1
0x12: KEY_RIGHT_2
0x13: KEY_RIGHT_3
0x14: KEY_RIGHT_4
0x15: KEY_RIGHT_5
0x16: KEY_RIGHT_6
0x17: KEY_RIGHT_7
0x18: KEY_RIGHT_8
0x19: KEY_RIGHT_9

# その他のボタン
0x21: KEY_L
0x22: KEY_R
0x23: KEY_LT
0x24: KEY_RT
```

### 利用可能なコマンド

#### 1. キーイベント取得 (CMD_GET_KEY_EVENT: 0x01)

キーボードのイベント（押下/解放）を取得します。
このコマンドを送信後、マスターがリード操作を行うとキーイベント情報が1バイトで返されます。

**使用方法:**
1. マスターからコマンド`0x01`を送信
2. マスターがリード操作を行うと、キーイベントが1バイトで返される

#### 2. LED明るさ設定 (CMD_SET_LED: 0x02)

特定のキーに対応するLEDの明るさを設定します。

**コマンド形式:**
- 1バイト目: コマンド (0x02)
- 2バイト目: キーコード
- 3バイト目: 明るさの値 (0-3)
  - 0: 明るい (LED_BRIGHT)
  - 1: 普通 (LED_NORMAL)
  - 2: 暗い (LED_DIM)
  - 3: 消灯 (LED_OFF)

#### 3. すべてのキーのアナログ値取得 (CMD_GET_ALL_KEY_VALUE: 0x03)

すべてのボタンの現在のアナログ値を取得します。
アナログ値は各キーごとに2バイト（16ビット）で、値が小さいほどボタンが押されていることを意味します。

**使用方法:**
1. マスターからコマンド`0x03`を送信
2. 連続してリード操作を行うと、すべてのキーの値が順番に返される
   - 各キーの値は2バイト（下位バイト、上位バイトの順）

#### 4. ボタン閾値設定 (CMD_SET_BUTTON_THRESHOLD: 0x04)

特定のキーの閾値（感度）を設定します。
閾値はアナログ値がこの値より低い場合にボタンが押されたと判断される値です。

**コマンド形式:**
- 1バイト目: コマンド (0x04)
- 2バイト目: キーコード
- 3バイト目: 閾値の下位バイト
- 4バイト目: 閾値の上位バイト

#### 5. 設定の保存 (CMD_SAVE_CONFIG: 0x05)

現在の設定（ボタン閾値など）をフラッシュメモリに保存します。
これにより、設定は電源を切っても保持されます。

**使用方法:**
1. マスターからコマンド`0x05`を送信するのみ

#### 6. ファームウェア更新モード (CMD_ENTER_DFU: 0xF0)

デバイスをファームウェア更新モードに入れます。

**使用方法:**
1. マスターからコマンド`0xF0`を送信するのみ

### 使用例

#### Arduinoでの使用例

```cpp
#include <Wire.h>

#define KEYPAD_ADDRESS 0x09
#define CMD_GET_KEY_EVENT 0x01
#define CMD_SET_LED 0x02
#define CMD_GET_ALL_KEY_VALUE 0x03
#define CMD_SET_BUTTON_THRESHOLD 0x04
#define CMD_SAVE_CONFIG 0x05

void setup() {
  Serial.begin(115200);
  Wire.begin(); // マスターとして初期化
}

void loop() {
  // キーイベントの取得
  Wire.beginTransmission(KEYPAD_ADDRESS);
  Wire.write(CMD_GET_KEY_EVENT);
  Wire.endTransmission();
  
  Wire.requestFrom(KEYPAD_ADDRESS, 1);
  if (Wire.available()) {
    byte keyEvent = Wire.read();
    if (keyEvent != 0) {
      Serial.print("Key Event: 0x");
      Serial.println(keyEvent, HEX);
    }
  }
  
  // ボタンのアナログ値の取得（例として1キー分のみ）
  Wire.beginTransmission(KEYPAD_ADDRESS);
  Wire.write(CMD_GET_ALL_KEY_VALUE);
  Wire.endTransmission();
  
  // キー0x01のアナログ値を取得（2バイト分を読み取る）
  Wire.requestFrom(KEYPAD_ADDRESS, 2);
  if (Wire.available() >= 2) {
    byte lowByte = Wire.read();
    byte highByte = Wire.read();
    uint16_t value = lowByte | (highByte << 8);
    Serial.print("Key 0x01 Analog Value: ");
    Serial.println(value);
  }
  
  // ボタン閾値の設定（例：キー0x01の閾値を3800に設定）
  Wire.beginTransmission(KEYPAD_ADDRESS);
  Wire.write(CMD_SET_BUTTON_THRESHOLD);
  Wire.write(0x01);            // キーコード
  Wire.write(3800 & 0xFF);     // 閾値の下位バイト
  Wire.write((3800 >> 8) & 0xFF); // 閾値の上位バイト
  Wire.endTransmission();
  
  // 設定の保存
  Wire.beginTransmission(KEYPAD_ADDRESS);
  Wire.write(CMD_SAVE_CONFIG);
  Wire.endTransmission();
  
  delay(1000);
}
```

#### Raspberry Pi（Python）での使用例

```python
import smbus
import time

bus = smbus.SMBus(1)  # I2Cバス番号（Raspberry Piのモデルによって0か1）
KEYPAD_ADDRESS = 0x09
CMD_GET_KEY_EVENT = 0x01
CMD_SET_LED = 0x02
CMD_GET_ALL_KEY_VALUE = 0x03
CMD_SET_BUTTON_THRESHOLD = 0x04
CMD_SAVE_CONFIG = 0x05

# キーイベントの取得
bus.write_byte(KEYPAD_ADDRESS, CMD_GET_KEY_EVENT)
key_event = bus.read_byte(KEYPAD_ADDRESS)
if key_event != 0:
    print(f"Key Event: 0x{key_event:02x}")

# すべてのキーのアナログ値を取得
bus.write_byte(KEYPAD_ADDRESS, CMD_GET_ALL_KEY_VALUE)
time.sleep(0.01)  # 少し待機

# 最初の数キーの値を取得
key_values = {}
for i in range(5):  # 例として最初の5キーを読み取る
    low_byte = bus.read_byte(KEYPAD_ADDRESS)
    high_byte = bus.read_byte(KEYPAD_ADDRESS)
    value = low_byte | (high_byte << 8)
    key_values[i] = value
    
print("Key Analog Values:", key_values)

# キー0x01の閾値を3800に設定
threshold = 3800
bus.write_i2c_block_data(KEYPAD_ADDRESS, CMD_SET_BUTTON_THRESHOLD, 
                         [0x01, threshold & 0xFF, (threshold >> 8) & 0xFF])

# 設定を保存
bus.write_byte(KEYPAD_ADDRESS, CMD_SAVE_CONFIG)
```

## 注意事項

- アナログ値は、キーボードマトリクスのキーでは0〜4095の範囲で、値が小さいほどボタンが押されていることを示します
- キースイッチ（L、R、LT、RT）では、アナログ値は0（押されている）または4095（押されていない）の2値になります
- 閾値を変更した後、必要に応じてCMD_SAVE_CONFIGコマンドを使って設定を保存してください
- フラッシュメモリへの書き込みには寿命があるため、頻繁な保存操作は避けることをお勧めします
