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
#include <stdlib.h>				// �����p
#include "keyboard.h"
#include "key_logger.h"
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
	SIRO,
	BACKID_MAX
};

enum ANIMID
{// �L�����N�^�[�Ǘ��ԍ�
	R_WALK,
	L_WALK,
	NEMUI,
	TRESURE,
	RUNNER01,
	RUNNER02,
	ANIMID_MAX
};

enum AREAID
{// ���݂̃G���A���ǂ������ʂ���p
	TOP,
	MIDDLE,
	UNDER,
	AREAID_MAX
};

constexpr float AREA_POSY_TOP    = 475.0f;			// ��w�G���A�̉����W
constexpr float AREA_POSY_MIDDLE = 535.0f;			// ���w�G���A�̉����W
constexpr float AREA_POSY_UNDER  = 625.0f;			// ���w�G���A�̉����W

enum BULLETID
{
	BULLET01,
	BULLET02,
	BULLET03,
	BULLETID_MAX
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
	int m_texid;			// �e�N�X�`��ID�Ǘ�
	int m_animid;			// �A�j���[�V�����p�^�[���Ǘ�ID
	int m_pid;				// �A�j���[�V�����v���C���[�Ǘ�ID
	int m_areaid;			// ���݂̏��݃G���A����ID
	XMFLOAT2 m_pos;			// �|���S��������W
	XMFLOAT2 m_size;		// �|���S���T�C�Y
};


/*==========================    �O���[�o���ϐ�     ============================*/
static float g_Time{};				// �X�V�������s��ꂽ���ԁH�t���[���񐔁H
static float g_ActionTime{};		// RUNNER01 �����������̎��ԁH�t���[���񐔁H
BackGround g_Back[BACKID_MAX]{};
Character g_Chara[ANIMID_MAX];
static int g_RunningmanHp{};		// �����Ă��郉���j���O�}����HP


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

	g_Back[SIRO].m_texid = TextureLoad(L"resource/texture/siro.png");
	g_Back[SIRO].m_pos = { 0.0f, SCREEN_HEIGHT * 0.5f + 128.0f };
	g_Back[SIRO].m_size = { SCREEN_WIDTH, 2.0f };
	g_Back[SIRO].m_texcoord = { 0.0f, 0.0f };
	g_Back[SIRO].m_texsize = { 1.0f, 1.0f };
	g_Back[SIRO].m_scrollspeed = 0.0f;
	g_Back[SIRO].m_angle = 0.0f;
	g_Back[SIRO].m_anglespeed = 0.0f;

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
	g_Chara[RUNNER01].m_texid  = TextureLoad(L"resource/texture/runningman001.png");
	g_Chara[RUNNER01].m_animid = SpriteAnimRefisterPattern(g_Chara[RUNNER01].m_texid, 10, 5, 0.1, { 0,  0 }, { 700 / 5, 400 / 2 }, true);
	g_Chara[RUNNER01].m_pid    = SpriteAnimCreatePlayer(g_Chara[RUNNER01].m_animid);
	g_Chara[RUNNER01].m_areaid = UNDER;
	g_Chara[RUNNER01].m_pos    = { SCREEN_WIDTH * 0.5f -256.0f,SCREEN_HEIGHT * 0.5f};
	g_Chara[RUNNER01].m_size   = { 160.0f, 160.0f };

	g_Chara[RUNNER02].m_texid  = TextureLoad(L"resource/texture/runningman003.png");
	g_Chara[RUNNER02].m_animid = SpriteAnimRefisterPattern(g_Chara[RUNNER02].m_texid, 10, 5, 0.1, { 0,  0 }, { 700 / 5, 400 / 2 }, true);
	g_Chara[RUNNER02].m_pid    = SpriteAnimCreatePlayer(g_Chara[RUNNER02].m_animid);
	g_Chara[RUNNER02].m_areaid = MIDDLE;
	g_Chara[RUNNER02].m_pos    = { SCREEN_WIDTH * 0.5f + 256.0f,SCREEN_HEIGHT * 0.5f};
	g_Chara[RUNNER02].m_size   = { 160.0f, 160.0f };
}

void GameFinalize()
{
}

void GameUpdata(double elapsed_time)
{
	/**********************************    �摜�X�V    **************************************/
	/* UV�X�N���[�� ���X�N���[�� */
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
	// RUNNER01 �ړ��@�i�����j
	g_Time += 1.0 * elapsed_time;
	// �O��̈ړ�����2�b��������ړ�
	if (g_Time - g_ActionTime >= 2.0f)
	{
		// �ړ��������Ԃ�ۑ�
		g_ActionTime = g_Time;

		// �����Ŏ��̈ړ��ꏊ����
		srand((unsigned)g_Time);
		int updown = rand() % 3;
		switch (updown)
		{
		case 0:
			g_Chara[RUNNER01].m_areaid += -1;
			break;
		case 1:
			g_Chara[RUNNER01].m_areaid += 0;
			break;
		case 2:
			g_Chara[RUNNER01].m_areaid += 1;
			break;
		}
		// �ړ��悪�͈͊O�ɂȂ邱�Ƃ�j�~
		if (g_Chara[RUNNER01].m_areaid < TOP)
		{
			g_Chara[RUNNER01].m_areaid = TOP;
		}
		if (g_Chara[RUNNER01].m_areaid > UNDER)
		{
			g_Chara[RUNNER01].m_areaid = UNDER;
		}
		// AREAID�ɉ������|�W�V�����ֈړ�
		switch (g_Chara[RUNNER01].m_areaid)
		{
		case TOP:
			g_Chara[RUNNER01].m_pos.y = AREA_POSY_TOP - g_Chara[RUNNER01].m_size.y;
			break;
		case MIDDLE:
			g_Chara[RUNNER01].m_pos.y = AREA_POSY_MIDDLE - g_Chara[RUNNER01].m_size.y;
			break;
		case UNDER:
			g_Chara[RUNNER01].m_pos.y = AREA_POSY_UNDER - g_Chara[RUNNER01].m_size.y;
			break;
		}
	}

	// RUNNER02 �ړ��@�i�蓮�j
	if (KeyLoggerIsTrigger(KK_W))
	{
		g_Chara[RUNNER02].m_areaid += -1;
	}
	if (KeyLoggerIsTrigger(KK_S))
	{
		g_Chara[RUNNER02].m_areaid += 1;
	}
	// �ړ��悪�͈͊O�ɂȂ邱�Ƃ�j�~
	if (g_Chara[RUNNER02].m_areaid < TOP)
	{
		g_Chara[RUNNER02].m_areaid = TOP;
	}
	if (g_Chara[RUNNER02].m_areaid > UNDER)
	{
		g_Chara[RUNNER02].m_areaid = UNDER;
	}
	// AREAID�ɉ������|�W�V�����ֈړ�
	switch (g_Chara[RUNNER02].m_areaid)
	{
	case TOP:
		g_Chara[RUNNER02].m_pos.y = AREA_POSY_TOP - g_Chara[RUNNER02].m_size.y;
		break;
	case MIDDLE:
		g_Chara[RUNNER02].m_pos.y = AREA_POSY_MIDDLE - g_Chara[RUNNER02].m_size.y;
		break;
	case UNDER:
		g_Chara[RUNNER02].m_pos.y = AREA_POSY_UNDER - g_Chara[RUNNER02].m_size.y;
		break;
	}


	// �A�j���[�V�������̍X�V
	for (int i = 0; i < ANIM_PLAY_MAX; i++)
	{
		if (i == R_WALK)
		{
			SpriteAnimUpdate(elapsed_time, i, false, true);
		}
		if (i == RUNNER01)
		{
			SpriteAnimUpdate(elapsed_time, i, false, false, true, 10.0);
		}
		if (i == RUNNER02)
		{
			SpriteAnimUpdate(elapsed_time, i, false, false, true, 5.0);
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
	Sprite_Draw(g_Back[SIRO].m_texid, { g_Back[SIRO].m_pos.x, AREA_POSY_TOP    }, g_Back[SIRO].m_size, g_Back[SIRO].m_texcoord, g_Back[SIRO].m_texsize, { 0.0f, 1.0f, 0.0f, 1.0f });
	Sprite_Draw(g_Back[SIRO].m_texid, { g_Back[SIRO].m_pos.x, AREA_POSY_MIDDLE }, g_Back[SIRO].m_size, g_Back[SIRO].m_texcoord, g_Back[SIRO].m_texsize, { 0.0f, 1.0f, 0.0f, 1.0f });
	Sprite_Draw(g_Back[SIRO].m_texid, { g_Back[SIRO].m_pos.x, AREA_POSY_UNDER  }, g_Back[SIRO].m_size, g_Back[SIRO].m_texcoord, g_Back[SIRO].m_texsize, { 0.0f, 1.0f, 0.0f, 1.0f });

	/* �L�����N�^�[�\�� */
	SpriteAnimDraw(g_Chara[RUNNER01].m_pid, g_Chara[RUNNER01].m_pos, g_Chara[RUNNER01].m_size);
	SpriteAnimDraw(g_Chara[RUNNER02].m_pid, g_Chara[RUNNER02].m_pos, g_Chara[RUNNER02].m_size);
}