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
	TEXT_ITETU,
	LINE,
	BACKID_MAX
};

constexpr float DISPTIME_ITETU = 0.3f;


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
constexpr float AREA_SIZERATIO_TOP    = 0.444f;		// ��w�G���A�̃T�C�Y��
constexpr float AREA_SIZERATIO_MIDDLE = 0.667f;		// ���w�G���A�̃T�C�Y��
constexpr float AREA_SIZERATIO_UNDER  = 1.0f;		// ���w�G���A�̃T�C�Y��

enum BULLETID
{
	BULLET01,
	BULLET02,
	BULLET03,
	BULLETID_MAX
};


enum ANIMID
{// �L�����N�^�[�Ǘ��ԍ�
	RUNNER01,
	RUNNER02,
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

struct Bullet
{// �L�����N�^�[�p�X�e�[�^�X
	bool m_use;				// ���ݔ��˂���Ă��邩�m�F�i��ʓ���true , ��ʊO��false�j�i�q�b�g������false�ɂ���j
	int m_areaid;			// ���݂̏��݃G���A����ID
	XMFLOAT2 m_pos;			// �|���S��������W�im_use == false �̂Ƃ��͉�ʊO�ҋ@�j
	XMFLOAT2 m_size;		// �|���S���T�C�Y
	XMFLOAT2 m_defaltsize;	// �f�t�H���g�|���S���T�C�Y
	float m_speed;			// �e�ۑ��x
};

struct Character
{// �L�����N�^�[�p�X�e�[�^�X
	int m_texid;			// �e�N�X�`��ID�Ǘ�
	int m_animid;			// �A�j���[�V�����p�^�[���Ǘ�ID
	int m_pid;				// �A�j���[�V�����v���C���[�Ǘ�ID
	int m_areaid;			// ���݂̏��݃G���A����ID
	XMFLOAT2 m_pos;			// �|���S��������W
	XMFLOAT2 m_size;		// �|���S���T�C�Y
	XMFLOAT2 m_defaltsize;	// �f�t�H���g�|���S���T�C�Y
	bool m_stop;			// �A�j���[�V������~�t���O
	bool m_reverse;			// �A�j���[�V�����t�Đ��t���O
	bool m_switcher;		// �A�j���[�V�����{���t���O
	double m_animspeed;		// �A�j���[�V�����Đ����x
};


/*==========================    �O���[�o���ϐ�     ============================*/
static float g_Time{};					// �X�V�������s��ꂽ����
static float g_MovedTime{};				// RUNNER01 �����������̎���
static float g_ShotTime{};				// RUNNER02 ���e�ۂ�����������
BackGround g_Back[BACKID_MAX]{};
static bool g_BackDispTextItetu{};		// �e�L�X�g�@�C�e�b��\�����Ă��邩�`�F�b�N
static float g_BackDispTimeTextItetu{};	// �e�L�X�g�@�C�e�b��\�����鎞��
Bullet g_Bullet[BULLETID_MAX];
int g_BulletTexid;						// �e�ۃe�N�X�`��ID�Ǘ�
Character g_Chara[ANIMID_MAX];
static int g_RunningmanHp{};			// �����Ă��郉���j���O�}����HP


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

	g_Back[LINE].m_texid = TextureLoad(L"resource/texture/siro.png");
	g_Back[LINE].m_pos = { 0.0f, SCREEN_HEIGHT * 0.5f + 128.0f };
	g_Back[LINE].m_size = { SCREEN_WIDTH, 2.0f };
	g_Back[LINE].m_texcoord = { 0.0f, 0.0f };
	g_Back[LINE].m_texsize = { 1.0f, 1.0f };
	g_Back[LINE].m_scrollspeed = 0.0f;
	g_Back[LINE].m_angle = 0.0f;
	g_Back[LINE].m_anglespeed = 0.0f;

	/**********************************  �e�L�X�g�摜������  **************************************/
	g_Back[TEXT_UTE].m_texid  = TextureLoad(L"resource/texture/text_ute.png");
	g_Back[TEXT_UTE].m_pos    = { 0.0f, 32.0f };
	g_Back[TEXT_UTE].m_size   = { SCREEN_WIDTH, 128.0f };
	g_Back[TEXT_UTE].m_texcoord = { 0.0f, 0.0f };
	g_Back[TEXT_UTE].m_texsize  = { 2.0f, 1.0 };
	g_Back[TEXT_UTE].m_scrollspeed = -0.8f;
	g_Back[TEXT_UTE].m_angle = 0.0f;
	g_Back[TEXT_UTE].m_anglespeed = 0.0f;

	g_Back[TEXT_ITETU].m_texid = TextureLoad(L"resource/texture/text_itetu.png");
	g_Back[TEXT_ITETU].m_pos = { 0.0f, 0.0f };
	g_Back[TEXT_ITETU].m_size = { 256.0f, 128.0f };
	g_Back[TEXT_ITETU].m_texcoord = { 0.0f, 0.0f };
	g_Back[TEXT_ITETU].m_texsize = { 1.0f, 1.0 };
	g_Back[TEXT_ITETU].m_scrollspeed = 0.0f;
	g_Back[TEXT_ITETU].m_angle = 0.0f;
	g_Back[TEXT_ITETU].m_anglespeed = 0.0f;
	g_BackDispTextItetu = false;
	g_BackDispTimeTextItetu = DISPTIME_ITETU;

	/**********************************  �e�ۏ�����  **************************************/
	g_BulletTexid = TextureLoad(L"resource/texture/tama.png");
	for (int i = 0; i < BULLETID_MAX; i++)
	{
		g_Bullet[i].m_use    = false;
		g_Bullet[i].m_areaid = TOP;
		g_Bullet[i].m_pos    = { SCREEN_WIDTH + 512.0f, AREA_POSY_TOP - 48.0f };
		g_Bullet[i].m_size   = { 48.0f, 48.0f };
		g_Bullet[i].m_defaltsize = { 48.0f, 48.0f };
		g_Bullet[i].m_speed  = -200.0f;
	}


	/**********************************  �����j���O�}��������  **************************************/
	g_Chara[RUNNER01].m_texid  = TextureLoad(L"resource/texture/runningman001.png");
	g_Chara[RUNNER01].m_animid = SpriteAnimRefisterPattern(g_Chara[RUNNER01].m_texid, 10, 5, 0.1, { 0,  0 }, { 700 / 5, 400 / 2 }, true);
	g_Chara[RUNNER01].m_pid    = SpriteAnimCreatePlayer(g_Chara[RUNNER01].m_animid);
	g_Chara[RUNNER01].m_areaid = MIDDLE;
	g_Chara[RUNNER01].m_pos    = { SCREEN_WIDTH * 0.5f -256.0f,AREA_POSY_MIDDLE - 160.0f };
	g_Chara[RUNNER01].m_size   = { 160.0f, 160.0f };
	g_Chara[RUNNER01].m_defaltsize = { 160.0f, 160.0f };
	g_Chara[RUNNER01].m_stop   = false;
	g_Chara[RUNNER01].m_reverse = false;
	g_Chara[RUNNER01].m_switcher = false;
	g_Chara[RUNNER01].m_animspeed = 1.0f;
	g_RunningmanHp = 5;

	g_Chara[RUNNER02].m_texid  = TextureLoad(L"resource/texture/runningman003.png");
	g_Chara[RUNNER02].m_animid = SpriteAnimRefisterPattern(g_Chara[RUNNER02].m_texid, 10, 5, 0.1, { 0,  0 }, { 700 / 5, 400 / 2 }, true);
	g_Chara[RUNNER02].m_pid    = SpriteAnimCreatePlayer(g_Chara[RUNNER02].m_animid);
	g_Chara[RUNNER02].m_areaid = MIDDLE;
	g_Chara[RUNNER02].m_pos    = { SCREEN_WIDTH * 0.5f + 256.0f,AREA_POSY_MIDDLE - 160.0f };
	g_Chara[RUNNER02].m_size   = { 160.0f, 160.0f };
	g_Chara[RUNNER02].m_defaltsize = { 160.0f, 160.0f };
	g_Chara[RUNNER01].m_stop = false;
	g_Chara[RUNNER01].m_reverse = false;
	g_Chara[RUNNER01].m_switcher = false;
	g_Chara[RUNNER01].m_animspeed = 1.0f;
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

	// �e�L�X�g �C�e�b�@�X�V
	if (g_BackDispTextItetu == true)
	{
		g_BackDispTimeTextItetu += g_Time * elapsed_time * -1.0f;
		if (g_BackDispTimeTextItetu <= 0.0f)
		{
			g_BackDispTextItetu = false;
			g_BackDispTimeTextItetu = DISPTIME_ITETU;
		}
	}


	/**********************************  �L�����N�^�[�X�V  **************************************/
	// RUNNER01 �ړ��@�i�����j
	g_Time += 1.0 * elapsed_time;
	// �O��̈ړ�����2�b��������ړ�
	if (g_Time - g_MovedTime >= 2.0f)
	{
		// �ړ��������Ԃ�ۑ�
		g_MovedTime = g_Time;

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
			g_Chara[RUNNER01].m_size.x = g_Chara[RUNNER01].m_defaltsize.x * AREA_SIZERATIO_TOP;
			g_Chara[RUNNER01].m_size.y = g_Chara[RUNNER01].m_defaltsize.y * AREA_SIZERATIO_TOP;
			g_Chara[RUNNER01].m_pos.y = AREA_POSY_TOP - g_Chara[RUNNER01].m_size.y;
			break;
		case MIDDLE:
			g_Chara[RUNNER01].m_size.x = g_Chara[RUNNER01].m_defaltsize.x * AREA_SIZERATIO_MIDDLE;
			g_Chara[RUNNER01].m_size.y = g_Chara[RUNNER01].m_defaltsize.y * AREA_SIZERATIO_MIDDLE;
			g_Chara[RUNNER01].m_pos.y = AREA_POSY_MIDDLE - g_Chara[RUNNER01].m_size.y;
			break;
		case UNDER:
			g_Chara[RUNNER01].m_size.x = g_Chara[RUNNER01].m_defaltsize.x * AREA_SIZERATIO_UNDER;
			g_Chara[RUNNER01].m_size.y = g_Chara[RUNNER01].m_defaltsize.y * AREA_SIZERATIO_UNDER;
			g_Chara[RUNNER01].m_pos.y = AREA_POSY_UNDER - g_Chara[RUNNER01].m_size.y;
			break;
		}
	}

	// �e�ۂƂ̂����蔻��
	for (int i = 0; i < BULLETID_MAX; i++)
	{
		if (g_Bullet[i].m_use == true && (g_Chara[RUNNER01].m_areaid == g_Bullet[i].m_areaid) 
			&& (g_Chara[RUNNER01].m_pos.x + g_Chara[RUNNER01].m_size.x - g_Bullet[i].m_pos.x > 0.0f) 
			&& (g_Chara[RUNNER01].m_pos.x - g_Bullet[i].m_pos.x + g_Bullet[i].m_size.x < 0.0f))
		{
			g_Bullet[i].m_use = false;
			g_RunningmanHp += -1;
			g_BackDispTextItetu = true;
			g_Back[TEXT_ITETU].m_pos = { g_Chara[RUNNER01].m_pos.x, g_Chara[RUNNER01].m_pos.y - g_Back[TEXT_ITETU].m_size.y };
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
		g_Chara[RUNNER02].m_size.x = g_Chara[RUNNER02].m_defaltsize.x * AREA_SIZERATIO_TOP;
		g_Chara[RUNNER02].m_size.y = g_Chara[RUNNER02].m_defaltsize.y * AREA_SIZERATIO_TOP;
		g_Chara[RUNNER02].m_pos.y = AREA_POSY_TOP - g_Chara[RUNNER02].m_size.y;
		break;
	case MIDDLE:
		g_Chara[RUNNER02].m_size.x = g_Chara[RUNNER02].m_defaltsize.x * AREA_SIZERATIO_MIDDLE;
		g_Chara[RUNNER02].m_size.y = g_Chara[RUNNER02].m_defaltsize.y * AREA_SIZERATIO_MIDDLE;
		g_Chara[RUNNER02].m_pos.y = AREA_POSY_MIDDLE - g_Chara[RUNNER02].m_size.y;
		break;
	case UNDER:
		g_Chara[RUNNER02].m_size.x = g_Chara[RUNNER02].m_defaltsize.x * AREA_SIZERATIO_UNDER;
		g_Chara[RUNNER02].m_size.y = g_Chara[RUNNER02].m_defaltsize.y * AREA_SIZERATIO_UNDER;
		g_Chara[RUNNER02].m_pos.y = AREA_POSY_UNDER - g_Chara[RUNNER02].m_size.y;
		break;
	}

	// RUNNER02 �e�۔���
	if (KeyLoggerIsTrigger(KK_SPACE) && (g_Time - g_ShotTime >= 0.5f))
	{
		for (int i = 0; i < BULLETID_MAX; i++)
		{
			// �g���Ă��Ȃ��e�ۂ�������
			if (g_Bullet[i].m_use == true)
			{
				continue;
			}
			else
			{
				// �e�ۂ����������Ԃ�ۑ�
				g_ShotTime = g_Time;
				// �g�p���ɕύX
				g_Bullet[i].m_use = true;
				// �����𔭎ˈʒu�ɍ��킹��
				g_Bullet[i].m_areaid = g_Chara[RUNNER02].m_areaid;
				g_Bullet[i].m_pos.x  = g_Chara[RUNNER02].m_pos.x;
				break;
			}
		}
	}


	/**********************************     �e�ۍX�V    **************************************/
	for (int i = 0; i < BULLETID_MAX; i++)
	{
		if (g_Bullet[i].m_use == false)
		{
			g_Bullet[i].m_areaid = TOP;
			g_Bullet[i].m_pos = { SCREEN_WIDTH + 512.0f, AREA_POSY_TOP - 48.0f };
		}
		else
		{
			switch (g_Bullet[i].m_areaid)
			{
			case TOP:
				g_Bullet[i].m_size.x =g_Bullet[i].m_defaltsize.x * AREA_SIZERATIO_TOP;
				g_Bullet[i].m_size.y =g_Bullet[i].m_defaltsize.y * AREA_SIZERATIO_TOP;
				g_Bullet[i].m_pos.y = AREA_POSY_TOP - g_Bullet[i].m_size.y;
				break;
			case MIDDLE:
				g_Bullet[i].m_size.x = g_Bullet[i].m_defaltsize.x * AREA_SIZERATIO_MIDDLE;
				g_Bullet[i].m_size.y = g_Bullet[i].m_defaltsize.y * AREA_SIZERATIO_MIDDLE;
				g_Bullet[i].m_pos.y = AREA_POSY_MIDDLE - g_Bullet[i].m_size.y;
				break;
			case UNDER:
				g_Bullet[i].m_size.x = g_Bullet[i].m_defaltsize.x * AREA_SIZERATIO_UNDER;
				g_Bullet[i].m_size.y = g_Bullet[i].m_defaltsize.y * AREA_SIZERATIO_UNDER;
				g_Bullet[i].m_pos.y = AREA_POSY_UNDER - g_Bullet[i].m_size.y;
				break;
			}
			g_Bullet[i].m_pos.x += g_Bullet[i].m_speed * elapsed_time;
			if (g_Bullet[i].m_pos.x <= 0 - g_Bullet[i].m_size.x)
			{
				g_Bullet[i].m_use = false;
			}
		}
	}

	// RUNNER01 ��HP��0�ɂȂ�����I���
	if (g_RunningmanHp <= 0)
	{
		// �S��~
		g_RunningmanHp = 0;
		g_Time = 0.0f;
		g_Back[BACK_SKY].m_scrollspeed    = 0.0f;
		g_Back[BACK_GROUND].m_scrollspeed = 0.0f;
		g_Back[TEXT_UTE].m_scrollspeed    = 0.0f;
		g_Back[BACK_SUN].m_anglespeed     = 0.0f;
		g_Chara[RUNNER01].m_stop = true;
		g_Chara[RUNNER02].m_stop = true;
		// RUNNER01�͏��������ď����B
		g_Chara[RUNNER01].m_size.x += 32.0f * elapsed_time * -1.0f;
		g_Chara[RUNNER01].m_size.y += 32.0f * elapsed_time * -1.0f;
		if (g_Chara[RUNNER01].m_size.x <= 0.0f)
		{
			g_Chara[RUNNER01].m_size.x = 0.0f;
		}
		if (g_Chara[RUNNER01].m_size.y <= 0.0f)
		{
			g_Chara[RUNNER01].m_size.y = 0.0f;
		}
		// �C�e�b ���ꏏ�ɏ��������ď���
		g_Back[TEXT_ITETU].m_size.x += 256.0f * 0.2f * elapsed_time * -1.0f;
		g_Back[TEXT_ITETU].m_size.y += 128.0f * 0.2f * elapsed_time * -1.0f;
		g_Back[TEXT_ITETU].m_pos.y  += 128.0f * 0.2f * elapsed_time;
		if (g_Back[TEXT_ITETU].m_size.x <= 0.0f)
		{
			g_Back[TEXT_ITETU].m_size.x = 0.0f;
		}
		if (g_Back[TEXT_ITETU].m_size.y <= 0.0f)
		{
			g_Back[TEXT_ITETU].m_size.y = 0.0f;
		}

	}

	// �A�j���[�V�������̍X�V
	for (int i = 0; i < ANIMID_MAX; i++)
	{
		if (i == RUNNER01)
		{
			SpriteAnimUpdate(elapsed_time, i, g_Chara[RUNNER01].m_stop, g_Chara[RUNNER01].m_reverse, g_Chara[RUNNER01].m_switcher, g_Chara[RUNNER01].m_animspeed);
		}
		if (i == RUNNER02)
		{
			SpriteAnimUpdate(elapsed_time, i, g_Chara[RUNNER02].m_stop, g_Chara[RUNNER02].m_reverse, g_Chara[RUNNER02].m_switcher, g_Chara[RUNNER02].m_animspeed);
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
	Sprite_Draw(g_Back[LINE].m_texid, { g_Back[LINE].m_pos.x, AREA_POSY_TOP    }, g_Back[LINE].m_size, g_Back[LINE].m_texcoord, g_Back[LINE].m_texsize, { 0.0f, 1.0f, 0.0f, 1.0f });
	Sprite_Draw(g_Back[LINE].m_texid, { g_Back[LINE].m_pos.x, AREA_POSY_MIDDLE }, g_Back[LINE].m_size, g_Back[LINE].m_texcoord, g_Back[LINE].m_texsize, { 0.0f, 1.0f, 0.0f, 1.0f });
	Sprite_Draw(g_Back[LINE].m_texid, { g_Back[LINE].m_pos.x, AREA_POSY_UNDER  }, g_Back[LINE].m_size, g_Back[LINE].m_texcoord, g_Back[LINE].m_texsize, { 0.0f, 1.0f, 0.0f, 1.0f });
	if (g_BackDispTextItetu == true)
	{
		Sprite_Draw(g_Back[TEXT_ITETU].m_texid, g_Back[TEXT_ITETU].m_pos, g_Back[TEXT_ITETU].m_size, g_Back[TEXT_ITETU].m_texcoord, g_Back[TEXT_ITETU].m_texsize);
	}

	/* �e�ە\�� */
	Sprite_Draw(g_BulletTexid, g_Bullet[BULLET01].m_pos, g_Bullet[BULLET01].m_size);
	Sprite_Draw(g_BulletTexid, g_Bullet[BULLET02].m_pos, g_Bullet[BULLET02].m_size);
	Sprite_Draw(g_BulletTexid, g_Bullet[BULLET03].m_pos, g_Bullet[BULLET03].m_size);

	/* �L�����N�^�[�\�� */
	SpriteAnimDraw(g_Chara[RUNNER01].m_pid, g_Chara[RUNNER01].m_pos, g_Chara[RUNNER01].m_size);
	SpriteAnimDraw(g_Chara[RUNNER02].m_pid, g_Chara[RUNNER02].m_pos, g_Chara[RUNNER02].m_size);
}