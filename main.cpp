#include <sdkddkver.h>
#define WIN32_LEANAND_MEAN
#include <Windows.h>
#include <sstream>
#include "game_window.h"
#include "direct3d.h"
#include "sprite.h"
#include "shader.h"
#include "texture.h"
#include "sprite_anim.h"
#include "debug_text.h"
#include "system_timer.h"
#include "game.h"
#include <DirectXmath.h>
using namespace DirectX;


/* ���C�� */
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);


	HWND hWnd = GameWindow_Create(hInstance);

	/*===============================�@�@�������@�@==================================*/
	SystemTimer_Initialize();
	Direct3D_Initialize(hWnd);
	Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	GameInitialize();


	// �f�o�b�O�e�L�X�g�p�摜�ǂݍ���
	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(), L"resource/texture/consolab_ascii_512.png", Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(), 0.0f, 0.0f, 0, 0, 0.0f, 0.0f);
	

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	/* fps�E���s�t���[���v���p */
	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0;
	ULONG flame_count = 0;
	double fps = 0.0;


	/*====================�@�@�Q�[�����[�v & ���b�Z�\�W���[�v�@�@=======================*/
	MSG msg;

	do
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{ // �E�B���h�E���b�Z�[�W�����Ă�����
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{ // �Q�[���̏���

			/* fps�v�� */
			current_time = SystemTimer_GetTime();
			double elapsed_time = current_time - fps_last_time;

			if (elapsed_time >= 1.0)
			{
				fps = flame_count / elapsed_time;
				fps_last_time = current_time;
				flame_count = 0;
			}

			elapsed_time = current_time - exec_last_time;
			if (elapsed_time >= (1.0 / 60.0))
			{
				exec_last_time = current_time;

				GameUpdata(elapsed_time);

				// �Q�[���̕`��
				Direct3D_Clear();

				Sprite_Begin();

				GameDraw();


#ifdef _DEBUG
				std::stringstream ss;
				ss << "fps:" << fps;
				dt.SetText(ss.str().c_str());
				dt.Draw();
				dt.Clear();
#endif


				Direct3D_Present();


				flame_count++;
			}

		}

	} while (msg.message != WM_QUIT);


	/*===============================�@�@�I���@�@==================================*/
	GameFinalize();
	Shader_Finalize();
	Direct3D_Finalize();


	return (int)msg.wParam;
}