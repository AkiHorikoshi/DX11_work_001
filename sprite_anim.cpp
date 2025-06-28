/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃															┃
┃		スプラトアニメーション描画[sprite_anim.cpp]				┃
┃															┃
┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃							Author: aki hoeikoshi			┃
┃							  data: 2025.6.17				┃
┃															┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

#include <DirectXMath.h>
using namespace DirectX;
#include "sprite_anim.h"
#include "sprite.h"
#include "texture.h"
#include "game_window.h"


struct AnimPatternData
{
	int m_TextureId = -1;					// テクスチャID

	int		 m_PatternMax = 0;				// パターン総数
	int		 m_hPatternMax = 0;				// 横に並んでるパターン最大数
	double   m_SecondsParPatrern = 0.1;		// １パターンを表示しておく秒数
	XMUINT2  m_StartPosition{};				// アニメーションスタートX,Y座標
	XMUINT2  m_PatternSize{};				// １パターンの幅,高さ
	bool	 m_IsLooped = true;				// ループ再生するか
};

struct AnimPlayData
{
	int		m_PatternId = -1;					// アニメーションパターンID
	int		m_PatternCount = 0;					// 現在のパターン番号
	double  m_Accumulated_time = 0;				// 累積時間
};


static AnimPatternData g_AnimPattern[ANIM_PATTERN_MAX];
static AnimPlayData g_AnimPlay[ANIM_PLAY_MAX];


void SpriteAnimInitialize()
{
	// アニメ―ションパターン管理情報を初期化（すべて使ってない状態にする）
	for (AnimPatternData& data : g_AnimPattern)
	{
		// m_TextureId がー１なら使ってない
		data.m_TextureId = -1;
	}

	for (AnimPlayData& data : g_AnimPlay)
	{
		data.m_PatternId = -1;
	}
}

void SpriteAnimFinalize()
{
}

void SpriteAnimUpdate(double elpsed_time, int texnum, bool stop,bool reverse, bool switcher, double speed)
{
	// アニメーションパターン変化
	if (g_AnimPlay[texnum].m_PatternId < 0)
	{
		return;
	}

	if (g_AnimPlay[texnum].m_Accumulated_time >= g_AnimPattern[texnum].m_SecondsParPatrern)
	{
		// パターンカウントをインクリメント
		// 1 * 停止判定(0 or 1) * 逆再生判定(-1 or 1)
		g_AnimPlay[texnum].m_PatternCount += 1 *SpriteAnimStop(stop) * SpriteAnimPlayBack(reverse);

		// 再生パターンを再生し切ったら最初のパターンに戻す
		if (g_AnimPlay[texnum].m_PatternCount >= g_AnimPattern[g_AnimPlay[texnum].m_PatternId].m_PatternMax)
		{
			if (g_AnimPattern[g_AnimPlay[texnum].m_PatternId].m_IsLooped == true)
			{
				g_AnimPlay[texnum].m_PatternCount = 0;
			}
			else
			{
				g_AnimPlay[texnum].m_PatternCount = g_AnimPattern[g_AnimPlay[texnum].m_PatternId].m_PatternMax - 1;
			}
		}
		// 逆再生で再生パターンを再生し切ったら最後のパターンに戻す
		if (reverse == true && g_AnimPlay[texnum].m_PatternCount <= 0)
		{
			if (g_AnimPattern[g_AnimPlay[texnum].m_PatternId].m_IsLooped == true)
			{
				g_AnimPlay[texnum].m_PatternCount = g_AnimPattern[g_AnimPlay[texnum].m_PatternId].m_PatternMax - 1;
			}
			else
			{
				g_AnimPlay[texnum].m_PatternCount = 0;
			}
		}

		g_AnimPlay[texnum].m_Accumulated_time -= g_AnimPattern[texnum].m_SecondsParPatrern;
	}

	// パターン再生時間調整
	g_AnimPlay[texnum].m_Accumulated_time += elpsed_time * SpriteAnimChangeSpeed(switcher, speed);
}

void SpriteAnimDraw(int playid, const DirectX::XMFLOAT2& dPosition, const DirectX::XMFLOAT2& dSize)
{
	int a_patt_id = g_AnimPlay[playid].m_PatternId;
	AnimPatternData* pAPattD = &g_AnimPattern[a_patt_id];

	XMUINT2 uvPos     = { (pAPattD->m_StartPosition.x + pAPattD->m_PatternSize.x * (g_AnimPlay[playid].m_PatternCount % g_AnimPattern[playid].m_hPatternMax)),
					      (pAPattD->m_StartPosition.y + pAPattD->m_PatternSize.y * (g_AnimPlay[playid].m_PatternCount / g_AnimPattern[playid].m_hPatternMax)) };
	XMUINT2 uvCutSize = { pAPattD->m_PatternSize.x, pAPattD->m_PatternSize.y };


	Sprite_Draw(g_AnimPattern[a_patt_id].m_TextureId,
		dPosition,
		dSize,
		uvPos,
		uvCutSize);
}

int SpriteAnimRefisterPattern(int texId, int pattMax, int hPattMax, double pattSeco, const DirectX::XMUINT2& pattPos, const DirectX::XMUINT2& pattSize, bool isLooped)
{
	for (int i = 0; i < ANIM_PATTERN_MAX; i++)
	{
		// 空いてる配列を探す
		if (g_AnimPattern[i].m_TextureId >= 0)
		{
			continue;
		}

		g_AnimPattern[i].m_TextureId = texId;
		g_AnimPattern[i].m_PatternMax = pattMax;
		g_AnimPattern[i].m_hPatternMax = hPattMax;
		g_AnimPattern[i].m_SecondsParPatrern = pattSeco;
		g_AnimPattern[i].m_StartPosition = pattPos;
		g_AnimPattern[i].m_PatternSize = pattSize;
		g_AnimPattern[i].m_IsLooped = isLooped;

		return i;
	}

	return -1;
}

int SpriteAnimCreatePlayer(int anim_pattern_id)
{
	for (int i = 0; i < ANIM_PLAY_MAX; i++)
	{
		if (g_AnimPlay[i].m_PatternId >= 0)
		{
			continue;
		}

		g_AnimPlay[i].m_PatternId = anim_pattern_id;
		g_AnimPlay[i].m_Accumulated_time = 0.0;
		g_AnimPlay[i].m_PatternCount = 0;

		return i;
	}


	return -1;
}

int SpriteAnimStop(bool stop)
{
	if (stop == true)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int SpriteAnimPlayBack(bool reverse)
{
	if (reverse == true)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

double SpriteAnimChangeSpeed(bool switcher, double speed)
{
	if (switcher == true)
	{
		return speed;
	}
	else
	{
		return 1.0;
	}
}