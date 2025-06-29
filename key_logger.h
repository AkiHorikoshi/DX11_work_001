/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃															┃
┃		キーボード入力の記録[key_logger.h]						┃
┃															┃
┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃							Author: aki hoeikoshi			┃
┃							  data: 2025.6.27				┃
┃															┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

#ifndef KEY_LOGGER_H
#define KEY_LOGGER_H


#include "keyboard.h"


void KeyLoggerInitialize();

void KeyLoggerUpdate();

// 押している間true
bool KeyLoggerIsPressed(Keyboard_Keys key);
// 押した瞬間だけtrue
bool KeyLoggerIsTrigger(Keyboard_Keys key);
// 離した瞬間だけtrue
bool KeyLoggerIsRelease(Keyboard_Keys key);

#endif //KEY_LOGGER_H