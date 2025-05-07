#pragma once

#include <stdio.h>

// コンソールの初期化
void console_init();

// コンソールのアップデート（main loopで呼び出す）
void console_update();

// コンソールコマンドを処理する関数
void process_command(char* cmd);

// コマンド処理時の最大コマンド長
#define MAX_CMD_LENGTH 32