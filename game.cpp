/*������������������������������������������������������������
��															��
��			�Q�[������[game.cpp]							��
��															��
��������������������������������������������������������������
��							Author: aki hoeikoshi			��
��							  data: 2025.6.25				��
��															��
������������������������������������������������������������*/


/*===========================    �C���N���[�h     =============================*/
#include <DirectXmath.h>
using namespace DirectX;
#include "direct3d.h"
#include "game_window.h"
#include "game.h"
#include "texture.h"
#include "sprite.h"
#include "sprite_anim.h"


/*=============================    �萔��`     ===============================*/
enum ANIMID
{
	BACK,
	R_WALK,
	L_WALK,
	NEMUI,
	TRESURE,
	RUNNER,
	ANIMID_MAX
};


/*===========================    �C���N���[�h     =============================*/
struct Character
{
	int m_texid;
	int m_animid;
	int m_pid;
	XMFLOAT2 m_pos;
	XMFLOAT2 m_size;
	float m_angle;
};


/*==========================    �O���[�o���ϐ�     ============================*/
Character g_sprite[ANIMID_MAX]{};
static float g_angle{};


/*=============================    �֐��錾     ===============================*/
void GameInitialize()
{
	TextureInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	SpriteAnimInitialize();

	/**********************************  �w�i�摜������  **************************************/
	g_sprite[BACK].m_texid  = TextureLoad(L"resource/texture/background.jpg");
	g_sprite[BACK].m_animid = SpriteAnimRefisterPattern(g_sprite[BACK].m_texid, 1, 1, 1.0, { 0, 0 }, { 1000, 400 }, false);
	g_sprite[BACK].m_pid    = SpriteAnimCreatePlayer(g_sprite[BACK].m_animid);
	g_sprite[BACK].m_pos    = { 0.0f, 0.0f };
	g_sprite[BACK].m_size   = { SCREEN_WIDTH, SCREEN_HEIGHT };


	/**********************************  �R�R�f�ފe�평����  **************************************/
	// �e�N�X�`���ǂݍ���
	for (int i = 0; i < TRESURE + 1 - R_WALK; i++)
	{
		g_sprite[i + R_WALK].m_texid = TextureLoad(L"resource/texture/kokosozai.png");
	}

	// �A�j���[�V�����p�^�[���ݒ�
	g_sprite[R_WALK].m_animid  = SpriteAnimRefisterPattern(g_sprite[R_WALK].m_texid, 13, 16, 0.1, { 0     ,  0     }, { 32, 32 }, true);
	g_sprite[L_WALK].m_animid  = SpriteAnimRefisterPattern(g_sprite[L_WALK].m_texid, 13, 16, 0.3, { 0     , 32 * 1 }, { 32, 32 }, true);
	g_sprite[NEMUI].m_animid   = SpriteAnimRefisterPattern(g_sprite[NEMUI].m_texid,  15, 16, 0.1, { 0     , 32 * 4 }, { 32, 32 }, true);
	g_sprite[TRESURE].m_animid = SpriteAnimRefisterPattern(g_sprite[TRESURE].m_texid, 4, 16, 0.1, { 32 * 2, 32 * 5 }, { 32, 32 }, false);

	// �Đ��Ώ۔ԍ�
	g_sprite[R_WALK].m_pid  = SpriteAnimCreatePlayer(g_sprite[R_WALK].m_animid);
	g_sprite[L_WALK].m_pid  = SpriteAnimCreatePlayer(g_sprite[L_WALK].m_animid);
	g_sprite[NEMUI].m_pid   = SpriteAnimCreatePlayer(g_sprite[NEMUI].m_animid);
	g_sprite[TRESURE].m_pid = SpriteAnimCreatePlayer(g_sprite[TRESURE].m_animid);

	// �����|�W�V�����ݒ�
	g_sprite[R_WALK].m_pos  = { 0.0f, 0.0f };
	g_sprite[L_WALK].m_pos  = { 0.0f, 0.0f };
	g_sprite[NEMUI].m_pos   = { 0.0f, 0.0f };
	g_sprite[TRESURE].m_pos = { 0.0f, 0.0f };

	// �X�v���C�g�T�C�Y�ݒ�
	g_sprite[R_WALK].m_size  = { 256.0f, 256.0f };
	g_sprite[L_WALK].m_size  = { 256.0f, 256.0f };
	g_sprite[NEMUI].m_size   = { 256.0f, 256.0f };
	g_sprite[TRESURE].m_size = { 256.0f, 256.0f };

	/**********************************  �����j���O�}��������  **************************************/
	g_sprite[RUNNER].m_texid  = TextureLoad(L"resource/texture/runningman001.png");
	g_sprite[RUNNER].m_animid = SpriteAnimRefisterPattern(g_sprite[RUNNER].m_texid, 10, 5, 0.1, { 0,  0 }, { 700 / 5, 400 / 2 }, true);
	g_sprite[RUNNER].m_pid    = SpriteAnimCreatePlayer(g_sprite[RUNNER].m_animid);
	g_sprite[RUNNER].m_pos    = { SCREEN_WIDTH * 0.5f - 128.0f,SCREEN_HEIGHT * 0.5f - 128.0f };
	g_sprite[RUNNER].m_size   = { 256.0f,256.0f };


}

void GameFinalize()
{
}

void GameUpdata(double elapsed_time)
{	// �Q�[���̍X�V

	for (int i = 0; i < ANIM_PLAY_MAX; i++)
	{
		if (i == R_WALK)
		{
			SpriteAnimUpdate(elapsed_time, i, false, false, false, 1.0);
		}
		if (i == RUNNER)
		{
			SpriteAnimUpdate(elapsed_time, i, false, false, false, 10.0);
		}
	}


	g_sprite[BACK].m_pos.x += 10.0f * -1.0f;
	if (g_sprite[BACK].m_pos.x <= -1000.0f)
	{
		g_sprite[BACK].m_pos.x += 2000.0f;
	}

	g_angle += XM_2PI * (float)elapsed_time;
}

void GameDraw()
{
	SpriteAnimDraw(g_sprite[BACK].m_pid, g_sprite[BACK].m_pos, g_sprite[BACK].m_size);
	SpriteAnimDraw(g_sprite[R_WALK].m_animid, g_sprite[R_WALK].m_pos, g_sprite[R_WALK].m_size);
	SpriteAnimDraw(g_sprite[RUNNER].m_pid, g_sprite[RUNNER].m_pos, g_sprite[RUNNER].m_size);
}