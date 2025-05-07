# CapsuleChord 2 キーパッドファームウェア

ユーザーが押下する操作に反応する物理的な装置を「ボタン」
ファームウェア内で扱う論理的な入力の最小単位を「キー」と呼ぶことにします

以下は自動で書いてもらったドキュメントなので間違い等あるかもです

## ボタン感度調整方法

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

### キーコード一覧

閾値を調整する際のキーコード：

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

### 注意事項

- 閾値が低すぎると、ボタンを押していないときにも押されていると誤認識する可能性があります
- 閾値が高すぎると、ボタンを押しても反応しにくくなる可能性があります
- フラッシュメモリへの書き込みには寿命があるため、テストの間は頻繁に`save`コマンドを使用せず、最終的な設定が決まったときだけ使用することをお勧めします
- シリアルナンバーは個体識別に使用されます。製品管理のため、各デバイスに一意のシリアルナンバーを設定することをお勧めします

## I2C通信によるLED制御

CapsuleChord 2キーパッドはI2Cスレーブデバイスとして動作し、キーイベントの取得やLEDの明るさ制御が可能です。

### I2C通信の設定

- スレーブアドレス: 0x09
- 通信速度: 100kHz
- 接続ピン:
  - SDA: ボード上のSDAPIN（I2C通信データライン）
  - SCL: ボード上のSCLPIN（I2C通信クロックライン）

### 利用可能なコマンド

#### 1. キーイベント取得 (CMD_GET_KEY_EVENT: 0x01)

マスターデバイスがスレーブからデータをリクエストすると、押されたキーのイベント情報が1バイトで返されます。

#### 2. LED明るさ設定 (CMD_SET_LED: 0x02)

特定のキーに対応するLEDの明るさを設定します。

**コマンド形式:**
- 1バイト目: コマンド (0x02)
- 2バイト目: キーコード（上記の「キーコード一覧」と同じコードを使用）
- 3バイト目: 明るさの値 (0-3)
  - 0: 明るい (LED_BRIGHT)
  - 1: 普通 (LED_NORMAL)
  - 2: 暗い (LED_DIM)
  - 3: 消灯 (LED_OFF)

### 使用例

以下はArduinoを使用してLEDの明るさを制御する例です：

```cpp
#include <Wire.h>

#define KEYPAD_ADDRESS 0x09
#define CMD_SET_LED 0x02

void setup() {
  Wire.begin(); // マスターとして初期化
}

void loop() {
  // 左キーパッドの1番（キーコード0x01）のLEDを明るく設定
  setLedBrightness(0x01, 0); // 0 = LED_BRIGHT
  
  delay(1000);
  
  // 左キーパッドの1番のLEDを暗く設定
  setLedBrightness(0x01, 2); // 2 = LED_DIM
  
  delay(1000);
  
  // 左キーパッドの1番のLEDを消灯
  setLedBrightness(0x01, 3); // 3 = LED_OFF
  
  delay(1000);
}

void setLedBrightness(uint8_t key, uint8_t brightness) {
  Wire.beginTransmission(KEYPAD_ADDRESS);
  Wire.write(CMD_SET_LED);     // LEDコマンド
  Wire.write(key);             // キーコード
  Wire.write(brightness);      // 明るさ (0-3)
  Wire.endTransmission();
}
```

Raspberry Piを使用した例（Python）：

```python
import smbus
import time

bus = smbus.SMBus(1)  # I2Cバス番号（Raspberry Piのモデルによって0か1）
KEYPAD_ADDRESS = 0x09
CMD_SET_LED = 0x02

def set_led_brightness(key, brightness):
    """
    特定のキーのLED明るさを設定
    
    引数:
        key (int): キーコード (0x01-0x24)
        brightness (int): 明るさの値 (0=明るい, 1=普通, 2=暗い, 3=消灯)
    """
    bus.write_i2c_block_data(KEYPAD_ADDRESS, CMD_SET_LED, [key, brightness])

# 左キーパッドの1番のLEDを明るく設定
set_led_brightness(0x01, 0)  # 0 = LED_BRIGHT
time.sleep(1)

# 左キーパッドの1番のLEDを普通の明るさに設定
set_led_brightness(0x01, 1)  # 1 = LED_NORMAL
time.sleep(1)

# 左キーパッドの1番のLEDを暗く設定
set_led_brightness(0x01, 2)  # 2 = LED_DIM
time.sleep(1)

# 左キーパッドの1番のLEDを消灯
set_led_brightness(0x01, 3)  # 3 = LED_OFF
```

### 注意事項

- LED明るさの設定はすぐに反映されますが、保存されません。再起動すると初期設定に戻ります。
- 無効なキーコードに対してLED明るさを設定しようとしても、操作は無視されます。
- I2C通信はISR（割り込みサービスルーチン）内で処理されるため、確実な通信のためにはタイミングに注意してください。
