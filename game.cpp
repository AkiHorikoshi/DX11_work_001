/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃															┃
┃			ゲーム処理[game.cpp]								┃
┃															┃
┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃							Author: aki hoeikoshi			┃
┃							  data: 2025.6.25				┃
┃															┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/


/*===========================    インクルード     =============================*/
#include <DirectXmath.h>
using namespace DirectX;
#include "direct3d.h"
#include "game_window.h"
#include "game.h"
#include "texture.h"
#include "sprite.h"
#include "sprite_anim.h"


/*=============================    定数定義     ===============================*/
enum ANIMID
{
	BACK_GROUND,
	BACK_SKY,
	BACK_SUN,
	TEXT_UTE,
	R_WALK,
	L_WALK,
	NEMUI,
	TRESURE,
	RUNNER,
	ANIMID_MAX
};

constexpr int BACK_MAX = BACK_SUN + 1;

/*===========================    構造体定義     =============================*/
struct Character
{
	int m_texid;
	int m_animid;
	int m_pid;
	XMFLOAT2 m_pos;
	XMFLOAT2 m_size;
	float m_angle;
};


/*==========================    グローバル変数     ============================*/
Character g_sprite[ANIMID_MAX]{};
static XMUINT2 g_Scroll[BACK_MAX]{};
static int g_ScrollSpeed[BACK_MAX]{};
static float g_angle{};


/*=============================    関数宣言     ===============================*/
void GameInitialize()
{
	TextureInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	SpriteAnimInitialize();

	/**********************************  背景画像初期化  **************************************/
	g_sprite[BACK_GROUND].m_texid  = TextureLoad(L"resource/texture/siba.png");
	g_sprite[BACK_GROUND].m_animid = SpriteAnimRefisterPattern(g_sprite[BACK_GROUND].m_texid, 1, 1, 1.0, { 0, 0 }, { 425, 340 }, false);
	g_sprite[BACK_GROUND].m_pid    = SpriteAnimCreatePlayer(g_sprite[BACK_GROUND].m_animid);
	g_sprite[BACK_GROUND].m_pos    = { 0.0f, 0.0f };
	g_sprite[BACK_GROUND].m_size   = { SCREEN_WIDTH, SCREEN_HEIGHT };
	g_Scroll[BACK_GROUND] = { 1, 0 };
	g_ScrollSpeed[BACK_GROUND] = -100;

	g_sprite[BACK_SKY].m_texid = TextureLoad(L"resource/texture/sky.png");
	g_sprite[BACK_SKY].m_animid = SpriteAnimRefisterPattern(g_sprite[BACK_SKY].m_texid, 1, 1, 1.0, { 0, 0 }, { 425, 340 }, false);
	g_sprite[BACK_SKY].m_pid = SpriteAnimCreatePlayer(g_sprite[BACK_SKY].m_animid);
	g_sprite[BACK_SKY].m_pos = { 0.0f, 0.0f };
	g_sprite[BACK_SKY].m_size = { SCREEN_WIDTH, SCREEN_HEIGHT };
	g_Scroll[BACK_SKY] = { 1, 0 };
	g_ScrollSpeed[BACK_SKY] = -30;

	g_sprite[BACK_SUN].m_texid = TextureLoad(L"resource/texture/sun.png");
	g_sprite[BACK_SUN].m_animid = SpriteAnimRefisterPattern(g_sprite[BACK_SUN].m_texid, 1, 1, 1.0, { 0, 0 }, { 425, 340 }, false);
	g_sprite[BACK_SUN].m_pid = SpriteAnimCreatePlayer(g_sprite[BACK_SUN].m_animid);
	g_sprite[BACK_SUN].m_pos = { SCREEN_WIDTH - 172.0f, 0.0f + 32.0f};
	g_sprite[BACK_SUN].m_size = { 128, 128 };
	g_Scroll[BACK_SUN] = { 0, 0 };
	g_ScrollSpeed[BACK_SUN] = 0;

	/**********************************  テキスト画像初期化  **************************************/
	g_sprite[TEXT_UTE].m_texid  = TextureLoad(L"resource/texture/text_ute.png");
	g_sprite[TEXT_UTE].m_animid = SpriteAnimRefisterPattern(g_sprite[TEXT_UTE].m_texid, 1, 1, 1.0, { 0, 0 }, { 512, 256 }, false);
	g_sprite[TEXT_UTE].m_pid    = SpriteAnimCreatePlayer(g_sprite[TEXT_UTE].m_animid);
	g_sprite[TEXT_UTE].m_pos    = { 0.0f, 0.0f };
	g_sprite[TEXT_UTE].m_size   = { SCREEN_WIDTH, 256.0f };

	/**********************************  ココ素材各種初期化  **************************************/
	// テクスチャ読み込み
	for (int i = 0; i < TRESURE + 1 - R_WALK; i++)
	{
		g_sprite[i + R_WALK].m_texid = TextureLoad(L"resource/texture/kokosozai.png");
	}

	// アニメーションパターン設定
	g_sprite[R_WALK].m_animid  = SpriteAnimRefisterPattern(g_sprite[R_WALK].m_texid, 13, 16, 0.1, { 0     ,  0     }, { 32, 32 }, true);
	g_sprite[L_WALK].m_animid  = SpriteAnimRefisterPattern(g_sprite[L_WALK].m_texid, 13, 16, 0.3, { 0     , 32 * 1 }, { 32, 32 }, true);
	g_sprite[NEMUI].m_animid   = SpriteAnimRefisterPattern(g_sprite[NEMUI].m_texid,  15, 16, 0.1, { 0     , 32 * 4 }, { 32, 32 }, true);
	g_sprite[TRESURE].m_animid = SpriteAnimRefisterPattern(g_sprite[TRESURE].m_texid, 4, 16, 0.1, { 32 * 2, 32 * 5 }, { 32, 32 }, false);

	// 再生対象番号
	g_sprite[R_WALK].m_pid  = SpriteAnimCreatePlayer(g_sprite[R_WALK].m_animid);
	g_sprite[L_WALK].m_pid  = SpriteAnimCreatePlayer(g_sprite[L_WALK].m_animid);
	g_sprite[NEMUI].m_pid   = SpriteAnimCreatePlayer(g_sprite[NEMUI].m_animid);
	g_sprite[TRESURE].m_pid = SpriteAnimCreatePlayer(g_sprite[TRESURE].m_animid);


	/**********************************  ランニングマン初期化  **************************************/
	g_sprite[RUNNER].m_texid  = TextureLoad(L"resource/texture/runningman001.png");
	g_sprite[RUNNER].m_animid = SpriteAnimRefisterPattern(g_sprite[RUNNER].m_texid, 10, 5, 0.1, { 0,  0 }, { 700 / 5, 400 / 2 }, true);
	g_sprite[RUNNER].m_pid    = SpriteAnimCreatePlayer(g_sprite[RUNNER].m_animid);
	g_sprite[RUNNER].m_pos    = { SCREEN_WIDTH * 0.5f - 128.0f,SCREEN_HEIGHT * 0.5f};
	g_sprite[RUNNER].m_size   = { 256.0f,256.0f };
}

void GameFinalize()
{
}

void GameUpdata(double elapsed_time)
{	// ゲームの更新

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

	g_Scroll[BACK_GROUND].x += g_ScrollSpeed[BACK_GROUND] * elapsed_time;
	if (g_Scroll[BACK_GROUND].x <= 100)
	{
		g_Scroll[BACK_GROUND].x = SCREEN_WIDTH;
	}

	g_Scroll[BACK_SKY].x += g_ScrollSpeed[BACK_SKY] * elapsed_time;
	if (g_Scroll[BACK_SKY].x <= 100)
	{
		g_Scroll[BACK_SKY].x = SCREEN_WIDTH;
	}

	g_angle += (XM_2PI * 1 / 3)* (float)elapsed_time;
}

void GameDraw()
{
	Sprite_Draw(g_sprite[BACK_SKY].m_pid, g_sprite[BACK_SKY].m_pos, g_sprite[BACK_SKY].m_size, { 10 + g_Scroll[BACK_SKY].x, 0 }, { SCREEN_WIDTH, 630 });
	Sprite_Draw(g_sprite[BACK_GROUND].m_pid, g_sprite[BACK_GROUND].m_pos, g_sprite[BACK_GROUND].m_size, { 10 + g_Scroll[BACK_GROUND].x, 0 }, { SCREEN_WIDTH, 340 });
	Sprite_Draw(g_sprite[BACK_SUN].m_pid, g_sprite[BACK_SUN].m_pos, g_sprite[BACK_SUN].m_size, { 0, 0 }, { 296, 300 }, g_angle);
	//Sprite_Draw(g_sprite[TEXT_UTE].m_animid, g_sprite[TEXT_UTE].m_pos, g_sprite[TEXT_UTE].m_size, { 0, 0 }, { SCREEN_WIDTH, 256});
	//SpriteAnimDraw(g_sprite[R_WALK].m_animid, { (int)(SCREEN_WIDTH * 0.5f - 128.0f) - 128, (int)(SCREEN_HEIGHT * 0.5f - 128.0f) }, { 256, 256 });
	SpriteAnimDraw(g_sprite[RUNNER].m_pid, g_sprite[RUNNER].m_pos, g_sprite[RUNNER].m_size);
}