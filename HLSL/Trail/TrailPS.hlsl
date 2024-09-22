#include "Trail.hlsli"
#include "../Functions.hlsli"

#define LINEAR 1
#define ANISOTROPIC 2

Texture2D trailTexture		  : register(t15);
SamplerState samplerStates[6] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float2 uv = pin.texcoord;
	
	float2 offset = float2(
	sin(
		value_noise(uv * 8.0)   * 0.2 +
		value_noise(uv * 16.0)  * 0.2 +
		value_noise(uv * 32.0)  * 0.2 +
		value_noise(uv * 64.0)  * 0.2 +
		value_noise(uv * 128.0) * 0.2
	) - 0.5
	,
	//sin(
	//	value_noise(uv * 8.0)   * 0.2 +
	//	value_noise(uv * 16.0)  * 0.2 +
	//	value_noise(uv * 32.0)  * 0.2 +
	//	value_noise(uv * 64.0)  * 0.2 +
	//	value_noise(uv * 128.0) * 0.2
	//) - 0.5
	0.0f
	);
	
	return trailTexture.Sample(samplerStates[ANISOTROPIC], pin.texcoord + offset);
}