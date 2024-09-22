#include "Skymap.hlsli"

#define POINT 0
#define LINER 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
#define LINEAR_MIRROR 5

Texture2D skyTexture : register(t20);
SamplerState samplerStates[6] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{	
	float3 E = normalize(pin.worldPosition.xyz - cameraPos.xyz);

	//	視線ベクトルを正距円筒図法に則ったUV座標へ変換する
	float2 texcoord;
	{
		texcoord = float2(1.0f - atan2(E.z, E.x) / 2.0f, -atan2(E.y, length(E.xz)));
		texcoord = texcoord / PI + 0.5f;
	}
	
	// 
	float4 color = skyTexture.SampleLevel(samplerStates[ANISOTROPIC], texcoord, 0);
		
#if 0
	// ガンマ処理
	const float GAMMA = 2.2f;
	color.rgb = pow(color.rgb, GAMMA);
#endif
	
	return color;
}