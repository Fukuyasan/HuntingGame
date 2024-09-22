
#include "Lambert.hlsli"

#define ANISOTROPIC 2

Texture2D diffuseMap : register(t0);
SamplerState SamplerStates[5] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	return diffuseMap.Sample(SamplerStates[ANISOTROPIC], pin.texcoord) * pin.color;
}

