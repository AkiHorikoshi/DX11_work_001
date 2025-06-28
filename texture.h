/*================================================

		テクスチャ―管理[texture.h]

					Author: aki hoeikoshi
					  data: 2025.6.13

================================================*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>

void TextureInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
void TextureFinalize();

// テクスチャ画像の読み込み
// 戻り値：管理番号。読み込めなかった場合 -1
// 引数：ファイル名
int TextureLoad(const wchar_t* pFilename);

// テクスチャの全開放
// 個別解放が作れたらより良い
void TextureAllRelease();

// テクスチャの管理番号を渡すと張り付けてくれる
// 引数：テクスチャ管理番号
void TextureSetTexture(int texid);

// テクスチャの幅
unsigned int GetTextureWidth(int texid);

// テクスチャの高さ
unsigned int GetTextureHeight(int texid);

#endif //TEXTURE_H