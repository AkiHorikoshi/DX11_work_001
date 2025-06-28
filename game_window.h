/*================================================
		
		ウィンドウ表示[game_window.h]
					
					Author: aki hoeikoshi
					  data: 2025.6.6

================================================*/

static const constexpr int SCREEN_WIDTH = 1120;
static const constexpr int SCREEN_HEIGHT = 630;


#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <sdkddkver.h>
#define WIN32_LEANAND_MEAN
#include <Windows.h>

HWND GameWindow_Create(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


#endif //GAME_WINDOW_H