#include "FullscreenQuad.hlsli"
#include "../Functions.hlsli"

#define LINEAR_MIRROR 5

SamplerState samplerState[6] : register(s0);
Texture2D textureMap[2]		 : register(t6);

// ózâä
float4 main(VS_OUT pin) : SV_TARGET
{	
	// è≈îM
	const float scorhingHeat = colorBalance.w;
	// ê[ìxéÊìæ
	float depth = textureMap[1].Sample(samplerState[LINEAR_MIRROR], pin.texcoord).r - depthOffset;
	depth		= max(0, depth);
	
	float2 uvr = seedScale * pin.texcoord + float2(time * timeScale, 0);
	float2 uvg = seedScale * pin.texcoord + float2(-10.0f, time * timeScale);
	
	float2 offset = float2(
	sin(
		value_noise(uvr * 8.0)   * 0.2 +
		value_noise(uvr * 16.0)  * 0.2 +
		value_noise(uvr * 32.0)  * 0.2 +
		value_noise(uvr * 64.0)  * 0.2 +
		value_noise(uvr * 128.0) * 0.2
	) - 0.5
	,
	sin(
		value_noise(uvg * 8.0)   * 0.2 +
		value_noise(uvg * 16.0)  * 0.2 +
		value_noise(uvg * 32.0)  * 0.2 +
		value_noise(uvg * 64.0)  * 0.2 +
		value_noise(uvg * 128.0) * 0.2
	) - 0.5
	) * ((depth * depth) * amplitude * (scorhingHeat * 0.1));
	
	float4 heatColor = textureMap[0].Sample(samplerState[LINEAR_MIRROR], pin.texcoord + offset);
	
	return heatColor;
}