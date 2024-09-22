#include "FullscreenQuad.hlsli"

SamplerState samplerState[5] : register(s0);
Texture2D    textureMap		 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
	return textureMap.Sample(samplerState[LINEAR_BORDER_BLACK], pin.texcoord);
}