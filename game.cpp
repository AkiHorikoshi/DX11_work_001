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
#include <stdlib.h>				// 乱数用
#include "keyboard.h"
#include "key_logger.h"
#include "game.h"
#include "texture.h"
#include "sprite.h"
#include "sprite_anim.h"


/*=============================    定数定義     ===============================*/
enum BACKID
{// 背景、テキスト管理番号
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
{// 現在のエリアがどこか判別する用
	TOP,
	MIDDLE,
	UNDER,
	AREAID_MAX
};


constexpr float AREA_POSY_TOP    = 475.0f;			// 上層エリアの下座標
constexpr float AREA_POSY_MIDDLE = 535.0f;			// 中層エリアの下座標
constexpr float AREA_POSY_UNDER  = 625.0f;			// 下層エリアの下座標
constexpr float AREA_SIZERATIO_TOP    = 0.444f;		// 上層エリアのサイズ比
constexpr float AREA_SIZERATIO_MIDDLE = 0.667f;		// 中層エリアのサイズ比
constexpr float AREA_SIZERATIO_UNDER  = 1.0f;		// 下層エリアのサイズ比

enum BULLETID
{
	BULLET01,
	BULLET02,
	BULLET03,
	BULLETID_MAX
};


enum ANIMID
{// キャラクター管理番号
	RUNNER01,
	RUNNER02,
	ANIMID_MAX
};


/*===========================    構造体定義     =============================*/
struct BackGround
{// 背景、テキスト用ステータス
	int m_texid;			// テクスチャID管理
	XMFLOAT2 m_pos;			// ポリゴン左上座標
	XMFLOAT2 m_size;		// ポリゴンサイズ
	XMFLOAT2 m_texcoord;	// UV左上座標
	XMFLOAT2 m_texsize;		// テクスチャをWRAPで何回繰り返すか
	float m_scrollspeed;	// スクロールスピード
	float m_angle;			// 回転角度
	float m_anglespeed;		// 回転速度
};

struct Bullet
{// キャラクター用ステータス
	bool m_use;				// 現在発射されているか確認（画面内＞true , 画面外＞false）（ヒットしたらfalseにする）
	int m_areaid;			// 現在の所在エリア判別ID
	XMFLOAT2 m_pos;			// ポリゴン左上座標（m_use == false のときは画面外待機）
	XMFLOAT2 m_size;		// ポリゴンサイズ
	XMFLOAT2 m_defaltsize;	// デフォルトポリゴンサイズ
	float m_speed;			// 弾丸速度
};

struct Character
{// キャラクター用ステータス
	int m_texid;			// テクスチャID管理
	int m_animid;			// アニメーションパターン管理ID
	int m_pid;				// アニメーションプレイヤー管理ID
	int m_areaid;			// 現在の所在エリア判別ID
	XMFLOAT2 m_pos;			// ポリゴン左上座標
	XMFLOAT2 m_size;		// ポリゴンサイズ
	XMFLOAT2 m_defaltsize;	// デフォルトポリゴンサイズ
	bool m_stop;			// アニメーション停止フラグ
	bool m_reverse;			// アニメーション逆再生フラグ
	bool m_switcher;		// アニメーション倍速フラグ
	double m_animspeed;		// アニメーション再生速度
};


/*==========================    グローバル変数     ============================*/
static float g_Time{};					// 更新処理が行われた時間
static float g_MovedTime{};				// RUNNER01 が動いた時の時間
static float g_ShotTime{};				// RUNNER02 が弾丸を撃った時間
BackGround g_Back[BACKID_MAX]{};
static bool g_BackDispTextItetu{};		// テキスト　イテッを表示しているかチェック
static float g_BackDispTimeTextItetu{};	// テキスト　イテッを表示する時間
Bullet g_Bullet[BULLETID_MAX];
int g_BulletTexid;						// 弾丸テクスチャID管理
Character g_Chara[ANIMID_MAX];
static int g_RunningmanHp{};			// 逃げているランニングマンのHP


/*=============================    関数宣言     ===============================*/
void GameInitialize()
{
	TextureInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	SpriteAnimInitialize();

	/**********************************  背景画像初期化  **************************************/
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

	/**********************************  テキスト画像初期化  **************************************/
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

	/**********************************  弾丸初期化  **************************************/
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


	/**********************************  ランニングマン初期化  **************************************/
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
	/**********************************    画像更新    **************************************/
	/* UVスクロール 横スクロール */
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

	// テキスト イテッ　更新
	if (g_BackDispTextItetu == true)
	{
		g_BackDispTimeTextItetu += g_Time * elapsed_time * -1.0f;
		if (g_BackDispTimeTextItetu <= 0.0f)
		{
			g_BackDispTextItetu = false;
			g_BackDispTimeTextItetu = DISPTIME_ITETU;
		}
	}


	/**********************************  キャラクター更新  **************************************/
	// RUNNER01 移動　（自動）
	g_Time += 1.0 * elapsed_time;
	// 前回の移動から2秒たったら移動
	if (g_Time - g_MovedTime >= 2.0f)
	{
		// 移動した時間を保存
		g_MovedTime = g_Time;

		// 乱数で次の移動場所決定
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
		// 移動先が範囲外になることを阻止
		if (g_Chara[RUNNER01].m_areaid < TOP)
		{
			g_Chara[RUNNER01].m_areaid = TOP;
		}
		if (g_Chara[RUNNER01].m_areaid > UNDER)
		{
			g_Chara[RUNNER01].m_areaid = UNDER;
		}
		// AREAIDに応じたポジションへ移動
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

	// 弾丸とのあたり判定
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

	// RUNNER02 移動　（手動）
	if (KeyLoggerIsTrigger(KK_W))
	{
		g_Chara[RUNNER02].m_areaid += -1;
	}
	if (KeyLoggerIsTrigger(KK_S))
	{
		g_Chara[RUNNER02].m_areaid += 1;
	}
	// 移動先が範囲外になることを阻止
	if (g_Chara[RUNNER02].m_areaid < TOP)
	{
		g_Chara[RUNNER02].m_areaid = TOP;
	}
	if (g_Chara[RUNNER02].m_areaid > UNDER)
	{
		g_Chara[RUNNER02].m_areaid = UNDER;
	}
	// AREAIDに応じたポジションへ移動
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

	// RUNNER02 弾丸発射
	if (KeyLoggerIsTrigger(KK_SPACE) && (g_Time - g_ShotTime >= 0.5f))
	{
		for (int i = 0; i < BULLETID_MAX; i++)
		{
			// 使われていない弾丸を見つける
			if (g_Bullet[i].m_use == true)
			{
				continue;
			}
			else
			{
				// 弾丸を撃った時間を保存
				g_ShotTime = g_Time;
				// 使用中に変更
				g_Bullet[i].m_use = true;
				// 高さを発射位置に合わせる
				g_Bullet[i].m_areaid = g_Chara[RUNNER02].m_areaid;
				g_Bullet[i].m_pos.x  = g_Chara[RUNNER02].m_pos.x;
				break;
			}
		}
	}


	/**********************************     弾丸更新    **************************************/
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

	// RUNNER01 のHPが0になったら終わり
	if (g_RunningmanHp <= 0)
	{
		// 全停止
		g_RunningmanHp = 0;
		g_Time = 0.0f;
		g_Back[BACK_SKY].m_scrollspeed    = 0.0f;
		g_Back[BACK_GROUND].m_scrollspeed = 0.0f;
		g_Back[TEXT_UTE].m_scrollspeed    = 0.0f;
		g_Back[BACK_SUN].m_anglespeed     = 0.0f;
		g_Chara[RUNNER01].m_stop = true;
		g_Chara[RUNNER02].m_stop = true;
		// RUNNER01は小さくして消す。
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
		// イテッ も一緒に小さくして消す
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

	// アニメーション情報の更新
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
	/* 背景、テキスト表示 */
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

	/* 弾丸表示 */
	Sprite_Draw(g_BulletTexid, g_Bullet[BULLET01].m_pos, g_Bullet[BULLET01].m_size);
	Sprite_Draw(g_BulletTexid, g_Bullet[BULLET02].m_pos, g_Bullet[BULLET02].m_size);
	Sprite_Draw(g_BulletTexid, g_Bullet[BULLET03].m_pos, g_Bullet[BULLET03].m_size);

	/* キャラクター表示 */
	SpriteAnimDraw(g_Chara[RUNNER01].m_pid, g_Chara[RUNNER01].m_pos, g_Chara[RUNNER01].m_size);
	SpriteAnimDraw(g_Chara[RUNNER02].m_pid, g_Chara[RUNNER02].m_pos, g_Chara[RUNNER02].m_size);
}