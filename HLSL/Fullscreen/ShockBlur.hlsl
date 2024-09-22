#include "FullscreenQuad.hlsli"

#define WIDTH 1280
#define HEIGHT 720

SamplerState samplerState[5] : register(s0);
Texture2D textureMap[2] : register(t6);

float4 main(VS_OUT pin) : SV_TARGET
{	
	// ブラーの中心位置から現在のテクセル位置のベクトルを求める
	float2 vec = centerTexel - pin.texcoord;
	
	// 距離を計算する
	float len = length(vec);
	
	// 方向ベクトルを正規化し、１テクセル分の長さとなる方向ベクトルを計算
	float textureU = 1.0 / WIDTH;
	float textureV = 1.0 / HEIGHT;
	
	vec = normalize(vec) * float2(textureU, textureV);
	
	// 距離を積算することで、爆発の中心位置に近いほどブラーの影響が小さくなるようにする
	vec *= blurPower * len;
	
	// 合成
#if 0
	float4 color;
	for (int i = 0; i < 10; ++i)
	{		
		color += textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + (vec * i)) * 0.19f - (0.02f * i);
	}
	return color;
#else
	float4 color[10];
	color[0] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord)				 * 0.19f;
	color[1] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec)		 * 0.17f;
	color[2] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 2.0f) * 0.15f;
	color[3] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 3.0f) * 0.13f;
	color[4] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 4.0f) * 0.11f;
	color[5] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 5.0f) * 0.09f;
	color[6] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 6.0f) * 0.07f;
	color[7] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 7.0f) * 0.05f;
	color[8] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 8.0f) * 0.03f;
	color[9] = textureMap[0].Sample(samplerState[LINEAR], pin.texcoord + vec * 9.0f) * 0.01f;
	return color[0] + color[1] + color[2] + color[3] + color[4] + color[5] + color[6] + color[7] + color[8] + color[9];
#endif

}
