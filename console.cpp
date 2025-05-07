#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "console.h"
#include "config.h"

// シリアルコマンドのバッファとポジション
static char cmd_buffer[MAX_CMD_LENGTH];
static int cmd_pos = 0;

void console_init() {
    printf("CapsuleChord2Keypad started.\n");
    printf("Type 'help' for available commands.\n");
}

void console_update() {
    // シリアルコマンドを処理
    int c = getchar_timeout_us(0);
    if (c != PICO_ERROR_TIMEOUT) {
        if (c == '\r' || c == '\n') {
            // コマンド終了、処理を実行
            if (cmd_pos > 0) {
                cmd_buffer[cmd_pos] = '\0';
                process_command(cmd_buffer);
                cmd_pos = 0;
            }
        } else if (cmd_pos < MAX_CMD_LENGTH - 1) {
            // コマンドを蓄積
            cmd_buffer[cmd_pos++] = c;
        }
    }
}

// シリアルコマンドを処理する関数
void process_command(char* cmd) {
    char* token = strtok(cmd, " ");
    if (token == NULL) return;

    if (strcmp(token, "threshold") == 0) {
        // threshold <key> [value]
        token = strtok(NULL, " ");
        if (token == NULL) {
            // 全てのキーの閾値を表示
            printf("Current threshold values:\n");
            for (int i = 0x01; i <= 0x24; i++) {
                if ((i >= 0x01 && i <= 0x09) || 
                    (i >= 0x11 && i <= 0x19) || 
                    (i >= 0x21 && i <= 0x24)) {
                    printf("Key 0x%02x: %d\n", i, get_button_threshold(i));
                }
            }
            return;
        }

        // 特定のキーの閾値を取得または設定
        int key = strtol(token, NULL, 0);
        token = strtok(NULL, " ");
        
        if (token == NULL) {
            // 値が指定されていない場合は現在の値を表示
            printf("Key 0x%02x threshold: %d\n", key, get_button_threshold(key));
        } else {
            // 値が指定されている場合は設定を更新
            int value = atoi(token);
            set_button_threshold(key, value);
            printf("Set key 0x%02x threshold to %d\n", key, value);
        }
    } else if (strcmp(token, "serial") == 0) {
        // serial [new-serial-number]
        token = strtok(NULL, " ");
        if (token == NULL) {
            // 現在のシリアルナンバーを表示
            printf("Serial Number: %s\n", get_serial_number());
        } else {
            // 新しいシリアルナンバーを設定
            if (set_serial_number(token)) {
                printf("Serial Number set to: %s\n", get_serial_number());
            } else {
                printf("Error: Invalid serial number format or too long\n");
            }
        }
    } else if (strcmp(token, "save") == 0) {
        // 設定を保存
        if (save_config()) {
            printf("Settings saved to flash\n");
        } else {
            printf("Failed to save settings\n");
        }
    } else if (strcmp(token, "load") == 0) {
        // 設定を読み込み
        if (load_config()) {
            printf("Settings loaded from flash\n");
        } else {
            printf("Failed to load settings\n");
        }
    } else if (strcmp(token, "default") == 0) {
        // デフォルト設定に戻す
        use_default_config();
        printf("Restored default settings (not saved to flash)\n");
    } else if (strcmp(token, "help") == 0) {
        // ヘルプを表示
        printf("Available commands:\n");
        printf("  threshold              - Show all key thresholds\n");
        printf("  threshold <key>        - Show threshold for specific key\n");
        printf("  threshold <key> <val>  - Set threshold for specific key\n");
        printf("  serial                 - Show current serial number\n");
        printf("  serial <value>         - Set new serial number\n");
        printf("  save                   - Save settings to flash\n");
        printf("  load                   - Load settings from flash\n");
        printf("  default                - Restore default settings\n");
        printf("  help                   - Show this help\n");
    } else {
        printf("Unknown command. Type 'help' for available commands.\n");
    }
}