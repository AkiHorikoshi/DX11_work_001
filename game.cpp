/*����������������������������������������������������������������������������������������������������������������������
��															��
��			�Q�[������[game.cpp]								��
��															��
��������������������������������������������������������������������������������������������������������������������������
��							Author: aki hoeikoshi			��
��							  data: 2025.6.25				��
��															��
������������������������������������������������������������������������������������������������������������������������*/


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
enum BACKID
{// �w�i�A�e�L�X�g�Ǘ��ԍ�
	BACK_SKY,
	BACK_SUN,
	BACK_GROUND,
	TEXT_UTE,
	BACK_MAX
};

enum ANIMID
{// �L�����N�^�[�Ǘ��ԍ�
	R_WALK,
	L_WALK,
	NEMUI,
	TRESURE,
	RUNNER,
	ANIMID_MAX
};


/*===========================    �\���̒�`     =============================*/
struct BackGround
{// �w�i�A�e�L�X�g�p�X�e�[�^�X
	int m_texid;			// �e�N�X�`��ID�Ǘ�
	XMFLOAT2 m_pos;			// �|���S��������W
	XMFLOAT2 m_size;		// �|���S���T�C�Y
	XMFLOAT2 m_texcoord;	// UV������W
	XMFLOAT2 m_texsize;		// �e�N�X�`����WRAP�ŉ���J��Ԃ���
	float m_scrollspeed;	// �X�N���[���X�s�[�h
	float m_angle;			// ��]�p�x
	float m_anglespeed;		// ��]���x
};

struct Character
{// �L�����N�^�[�p�X�e�[�^�X
	int m_texid;
	int m_animid;
	int m_pid;
	XMFLOAT2 m_pos;
	XMFLOAT2 m_size;
	float m_angle;
};


/*==========================    �O���[�o���ϐ�     ============================*/
BackGround g_Back[BACK_MAX]{};
Character g_Chara[ANIMID_MAX];


/*=============================    �֐��錾     ===============================*/
void GameInitialize()
{
	TextureInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	SpriteAnimInitialize();

	/**********************************  �w�i�摜������  **************************************/
	g_Back[BACK_SKY].m_texid = TextureLoad(L"resource/texture/sky.png");
	g_Back[BACK_SKY].m_pos   = { 0.0f, 0.0f };
	g_Back[BACK_SKY].m_size  = { SCREEN_WIDTH, SCREEN_HEIGHT };
	g_Back[BACK_SKY].m_texcoord = { 0.0f, 0.0f };
	g_Back[BACK_SKY].m_texsize  = { 2.0f, 1.0f };
	g_Back[BACK_SKY].m_scrollspeed = -0.1f;
	g_Back[BACK_SKY].m_angle = 0.0f;
	g_Back[BACK_SKY].m_anglespeed = 0.0f;

	g_Back[BACK_SUN].m_texid  = TextureLoad(L"resource/texture/sun.png");
	g_Back[BACK_SUN].m_pos    = { SCREEN_WIDTH - 172.0f, 0.0f + 32.0f };
	g_Back[BACK_SUN].m_size   = { 128, 128 };
	g_Back[BACK_SUN].m_texcoord = { 0.0f, 0.0f };
	g_Back[BACK_SUN].m_texsize  = { 1.0f, 1.0f };
	g_Back[BACK_SUN].m_scrollspeed = 0.0f;
	g_Back[BACK_SUN].m_angle = 0.0f;
	g_Back[BACK_SUN].m_anglespeed = (XM_2PI / 6);

	g_Back[BACK_GROUND].m_texid  = TextureLoad(L"resource/texture/siba.png");
	g_Back[BACK_GROUND].m_pos    = { 0.0f, 0.0f };
	g_Back[BACK_GROUND].m_size   = { SCREEN_WIDTH, SCREEN_HEIGHT };
	g_Back[BACK_GROUND].m_texcoord = { 0.0f, 0.0f };
	g_Back[BACK_GROUND].m_texsize  = { 3.0f, 1.0f };
	g_Back[BACK_GROUND].m_scrollspeed = -0.5f;
	g_Back[BACK_GROUND].m_angle = 0.0f;
	g_Back[BACK_GROUND].m_anglespeed = 0.0f;

	/**********************************  �e�L�X�g�摜������  **************************************/
	g_Back[TEXT_UTE].m_texid  = TextureLoad(L"resource/texture/text_ute.png");
	g_Back[TEXT_UTE].m_pos    = { 0.0f, 32.0f };
	g_Back[TEXT_UTE].m_size   = { SCREEN_WIDTH, 128 };
	g_Back[TEXT_UTE].m_texcoord = { 0.0f, 0.0f };
	g_Back[TEXT_UTE].m_texsize  = { 2.0f, 1.0 };
	g_Back[TEXT_UTE].m_scrollspeed = -0.8f;
	g_Back[TEXT_UTE].m_angle = 0.0f;
	g_Back[TEXT_UTE].m_anglespeed = 0.0f;


	/**********************************  �R�R�f�ފe�평����  **************************************/
	// �e�N�X�`���ǂݍ���
	for (int i = 0; i < TRESURE + 1 - R_WALK; i++)
	{
		g_Chara[i + R_WALK].m_texid = TextureLoad(L"resource/texture/kokosozai.png");
	}

	// �A�j���[�V�����p�^�[���ݒ�
	g_Chara[R_WALK].m_animid  = SpriteAnimRefisterPattern(g_Chara[R_WALK].m_texid, 13, 16, 0.1, { 0     ,  0     }, { 32, 32 }, true);
	g_Chara[L_WALK].m_animid  = SpriteAnimRefisterPattern(g_Chara[L_WALK].m_texid, 13, 16, 0.3, { 0     , 32 * 1 }, { 32, 32 }, true);
	g_Chara[NEMUI].m_animid   = SpriteAnimRefisterPattern(g_Chara[NEMUI].m_texid,  15, 16, 0.1, { 0     , 32 * 4 }, { 32, 32 }, true);
	g_Chara[TRESURE].m_animid = SpriteAnimRefisterPattern(g_Chara[TRESURE].m_texid, 4, 16, 0.1, { 32 * 2, 32 * 5 }, { 32, 32 }, false);

	// �Đ��Ώ۔ԍ�
	g_Chara[R_WALK].m_pid  = SpriteAnimCreatePlayer(g_Chara[R_WALK].m_animid);
	g_Chara[L_WALK].m_pid  = SpriteAnimCreatePlayer(g_Chara[L_WALK].m_animid);
	g_Chara[NEMUI].m_pid   = SpriteAnimCreatePlayer(g_Chara[NEMUI].m_animid);
	g_Chara[TRESURE].m_pid = SpriteAnimCreatePlayer(g_Chara[TRESURE].m_animid);


	/**********************************  �����j���O�}��������  **************************************/
	g_Chara[RUNNER].m_texid  = TextureLoad(L"resource/texture/runningman001.png");
	g_Chara[RUNNER].m_animid = SpriteAnimRefisterPattern(g_Chara[RUNNER].m_texid, 10, 5, 0.1, { 0,  0 }, { 700 / 5, 400 / 2 }, true);
	g_Chara[RUNNER].m_pid    = SpriteAnimCreatePlayer(g_Chara[RUNNER].m_animid);
	g_Chara[RUNNER].m_pos    = { SCREEN_WIDTH * 0.5f - 128.0f,SCREEN_HEIGHT * 0.5f};
	g_Chara[RUNNER].m_size   = { 256.0f,256.0f };
}

void GameFinalize()
{
}

void GameUpdata(double elapsed_time)
{
	/**********************************    �摜�X�V    **************************************/
	/* UV�X�N���[��  U�̒l�����Z */
	g_Back[BACK_SKY].m_texcoord.x += g_Back[BACK_SKY].m_scrollspeed * elapsed_time;
	g_Back[BACK_SKY].m_texcoord.x = fmodf(g_Back[BACK_SKY].m_texcoord.x, g_Back[BACK_SKY].m_texsize.x);
	if (g_Back[BACK_SKY].m_texcoord.x < 0)
	{
		g_Back[BACK_SKY].m_texcoord.x += g_Back[BACK_SKY].m_texsize.x;
	}

	g_Back[BACK_GROUND].m_texcoord.x += g_Back[BACK_GROUND].m_scrollspeed * elapsed_time;
	g_Back[BACK_GROUND].m_texcoord.x = fmodf(g_Back[BACK_GROUND].m_texcoord.x, g_Back[BACK_GROUND].m_texsize.x);
	if (g_Back[BACK_GROUND].m_texcoord.x < 0)
	{
		g_Back[BACK_GROUND].m_texcoord.x += g_Back[BACK_GROUND].m_texsize.x;
	}

	g_Back[TEXT_UTE].m_texcoord.x += g_Back[TEXT_UTE].m_scrollspeed * elapsed_time;
	g_Back[TEXT_UTE].m_texcoord.x = fmodf(g_Back[TEXT_UTE].m_texcoord.x, g_Back[TEXT_UTE].m_texsize.x);
	if (g_Back[TEXT_UTE].m_texcoord.x < 0)
	{
		g_Back[TEXT_UTE].m_texcoord.x += g_Back[TEXT_UTE].m_texsize.x;
	}

	g_Back[BACK_SUN].m_angle += g_Back[BACK_SUN].m_anglespeed * (float)elapsed_time;


	/**********************************  �L�����N�^�[�X�V  **************************************/
	for (int i = 0; i < ANIM_PLAY_MAX; i++)
	{
		if (i == R_WALK)
		{
			SpriteAnimUpdate(elapsed_time, i, false, true);
		}
		if (i == RUNNER)
		{
			SpriteAnimUpdate(elapsed_time, i, false, false, true, 10.0);
		}
	}

}

void GameDraw()
{
	/* �w�i�A�e�L�X�g�\�� */
	Sprite_Draw(g_Back[BACK_SKY].m_texid   , g_Back[BACK_SKY].m_pos   , g_Back[BACK_SKY].m_size   , g_Back[BACK_SKY].m_texcoord   , g_Back[BACK_SKY].m_texsize);
	Sprite_Draw(g_Back[BACK_GROUND].m_texid, g_Back[BACK_GROUND].m_pos, g_Back[BACK_GROUND].m_size, g_Back[BACK_GROUND].m_texcoord, g_Back[BACK_GROUND].m_texsize);
	Sprite_Draw(g_Back[BACK_SUN].m_texid   , g_Back[BACK_SUN].m_pos   , g_Back[BACK_SUN].m_size   , { 0, 0 }, { 296, 300 }        , g_Back[BACK_SUN].m_angle);
	Sprite_Draw(g_Back[TEXT_UTE].m_texid   , g_Back[TEXT_UTE].m_pos   , g_Back[TEXT_UTE].m_size   , g_Back[TEXT_UTE].m_texcoord   , g_Back[TEXT_UTE].m_texsize);

	/* �L�����N�^�[�\�� */
	SpriteAnimDraw(g_Chara[RUNNER].m_pid, g_Chara[RUNNER].m_pos, g_Chara[RUNNER].m_size);
}