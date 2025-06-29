/*================================================

		ウィンドウ表示[game_window.cpp]

					Author: aki hoeikoshi
					  data: 2025.6.6

================================================*/


#include "game_window.h"
#include <algorithm>
#include "keyboard.h"


/*
ウィンドウ情報
*/
static constexpr char WINDOW_CLASS[] = "GameWindow"; //メインウィンドウクラス名
static constexpr char TITLE[] = "Game";	 //タイトルバーのテキスト


/*
ウィンドウプロシージャ　プロトタイプ宣言
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


HWND GameWindow_Create(HINSTANCE hInstance)
{
	/*　ウィンドウクラスの登録　*/
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName = nullptr;	//メニュー無し
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hCursor = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassEx(&wcex);


	/* メインウィンドウ作成 */
	RECT WindowRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	DWORD WinStyle = WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME);

	AdjustWindowRect(&WindowRect, WinStyle, FALSE);

	const int WINDOW_WIDTH = WindowRect.right - WindowRect.left;
	const int WINDOW_HEIGHT = WindowRect.bottom - WindowRect.top;

	//======================================================================================
	//6.6追加分
	//デスクトップのサイズ取得
	//プライマリモニターの画面解像度取得
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	//画面サイズの最大から表示したいもののサイズを引いて÷２する
	//そうすれば表示したいものを中心に持ってくるための左上座標が求まる
	const int WINDOW_X = std::max((desktop_width - WINDOW_WIDTH) / 2, 0);
	const int WINDOW_Y = std::max((desktop_height - WINDOW_HEIGHT) / 2, 0);
	//======================================================================================

	HWND hWnd = CreateWindow(WINDOW_CLASS,
		TITLE,
		WinStyle,
		WINDOW_X,
		WINDOW_Y,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		nullptr, nullptr, hInstance, nullptr);


	return hWnd;
}

/*
ウィンドウプロシージャ
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//======================================================================================
		//6.6追加分
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
         Keyboard_ProcessMessage(message, wParam, lParam);
         break;

		break;
		//======================================================================================
	case WM_CLOSE:
		if (MessageBox(hWnd, "終了しますか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
			DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}