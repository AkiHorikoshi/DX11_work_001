/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃															┃
┃		スプラトアニメーション描画[sprite_anim.h]			┃
┃															┃
┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃							Author: aki hoeikoshi			┃
┃							  data: 2025.6.17				┃
┃															┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

#ifndef SPRITE_ANIM_H
#define SPRITE_ANIM_H


#include <DirectXMath.h>


static constexpr int ANIM_PATTERN_MAX = 256;
static constexpr int ANIM_PLAY_MAX = 256;


void SpriteAnimInitialize();
void SpriteAnimFinalize();

void SpriteAnimUpdate(double elapsed_time, int i, bool stop = false, bool revers = false, bool switcher = false, double speed = 1.0);
void SpriteAnimDraw(int playid, const DirectX::XMFLOAT2& dPosition, const DirectX::XMFLOAT2& dSize);

int SpriteAnimRefisterPattern(int texId, int pattMax, int hPattMax, double pattSeco, const DirectX::XMUINT2& pattPos, const DirectX::XMUINT2& pattSize, bool isLooped);

int SpriteAnimCreatePlayer(int anim_pattern_id);

int SpriteAnimStop(bool stop);

int SpriteAnimPlayBack(bool reverse);

double SpriteAnimChangeSpeed(bool switcher, double speed);

#endif //SPRITE_ANIM_H