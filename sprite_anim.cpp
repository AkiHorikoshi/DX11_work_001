/*����������������������������������������������������������������������������������������������������������������������
��															��
��		�X�v���g�A�j���[�V�����`��[sprite_anim.cpp]				��
��															��
��������������������������������������������������������������������������������������������������������������������������
��							Author: aki hoeikoshi			��
��							  data: 2025.6.17				��
��															��
������������������������������������������������������������������������������������������������������������������������*/

#include <DirectXMath.h>
using namespace DirectX;
#include "sprite_anim.h"
#include "sprite.h"
#include "texture.h"
#include "game_window.h"


struct AnimPatternData
{
	int m_TextureId = -1;					// �e�N�X�`��ID

	int		 m_PatternMax = 0;				// �p�^�[������
	int		 m_hPatternMax = 0;				// ���ɕ���ł�p�^�[���ő吔
	double   m_SecondsParPatrern = 0.1;		// �P�p�^�[����\�����Ă����b��
	XMUINT2  m_StartPosition{};				// �A�j���[�V�����X�^�[�gX,Y���W
	XMUINT2  m_PatternSize{};				// �P�p�^�[���̕�,����
	bool	 m_IsLooped = true;				// ���[�v�Đ����邩
};

struct AnimPlayData
{
	int		m_PatternId = -1;					// �A�j���[�V�����p�^�[��ID
	int		m_PatternCount = 0;					// ���݂̃p�^�[���ԍ�
	double  m_Accumulated_time = 0;				// �ݐώ���
};


static AnimPatternData g_AnimPattern[ANIM_PATTERN_MAX];
static AnimPlayData g_AnimPlay[ANIM_PLAY_MAX];


void SpriteAnimInitialize()
{
	// �A�j���\�V�����p�^�[���Ǘ������������i���ׂĎg���ĂȂ���Ԃɂ���j
	for (AnimPatternData& data : g_AnimPattern)
	{
		// m_TextureId ���[�P�Ȃ�g���ĂȂ�
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
	// �A�j���[�V�����p�^�[���ω�
	if (g_AnimPlay[texnum].m_PatternId < 0)
	{
		return;
	}

	if (g_AnimPlay[texnum].m_Accumulated_time >= g_AnimPattern[texnum].m_SecondsParPatrern)
	{
		// �p�^�[���J�E���g���C���N�������g
		// 1 * ��~����(0 or 1) * �t�Đ�����(-1 or 1)
		g_AnimPlay[texnum].m_PatternCount += 1 *SpriteAnimStop(stop) * SpriteAnimPlayBack(reverse);

		// �Đ��p�^�[�����Đ����؂�����ŏ��̃p�^�[���ɖ߂�
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
		// �t�Đ��ōĐ��p�^�[�����Đ����؂�����Ō�̃p�^�[���ɖ߂�
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

	// �p�^�[���Đ����Ԓ���
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
		// �󂢂Ă�z���T��
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