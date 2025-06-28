/*================================================

		スプライト表示[sprite.h]

					Author: aki hoeikoshi
					  data: 2025.6.12

================================================*/
#ifndef SPRITE_H
#define SPRITE_H

#include <d3d11.h>
#include <DirectXMath.h>

void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Sprite_Finalize(void);
void Sprite_Begin();
void Sprite_Updata(void);
// テクスチャ全表示
// 引数 : テクスチャID  ポリゴン左上ｘ  ポリゴン左上y  頂点カラー(デフォルト白)
void Sprite_Draw(int texid, const DirectX::XMFLOAT2& dPos, const DirectX::XMFLOAT4& color = {1.0f, 1.0f, 1.0f, 1.0f});

// テクスチャ全表示 サイズ変更可
// 引数 : テクスチャID  ポリゴン左上ｘ  ポリゴン左上y  ポリゴンwigth  ポリゴンheight  頂点カラー(デフォルト白)
void Sprite_Draw(int texid, const DirectX::XMFLOAT2& dPos, const DirectX::XMFLOAT2& dSize, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

//// UVカット
//// 引数 : テクスチャID  ポリゴン左上ｘ  ポリゴン左上y　UV座標左上x  UV座標左上y   UVサイズwigth  UVサイズheight  頂点カラー(デフォルト白)
//void Sprite_Draw(int texid, float dx, float dy, int tx, int ty, int tw, int th, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

// UVカット  サイズ変更可
// 引数 : テクスチャID  ポリゴン左上ｘ  ポリゴン左上y  ポリゴンwigth  ポリゴンheight　UV座標左上x  UV座標左上y   UVサイズwigth  UVサイズheight  頂点カラー(デフォルト白)
void Sprite_Draw(int texid, const DirectX::XMFLOAT2& dPos, const DirectX::XMFLOAT2& dSize, const DirectX::XMUINT2& pPos, const DirectX::XMUINT2& pSize, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

// UVカット  サイズ変更可 回転可
// 引数 : テクスチャID  ポリゴン左上ｘ  ポリゴン左上y  ポリゴンwigth  ポリゴンheight　UV座標左上x  UV座標左上y   UVサイズwigth  UVサイズheight  回転量　頂点カラー(デフォルト白)
void Sprite_Draw(int texid, const DirectX::XMFLOAT2& dPos, const DirectX::XMFLOAT2& dSize, const DirectX::XMUINT2& pPos, const DirectX::XMUINT2& pSize, float angle, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

#endif //SPRITE_H