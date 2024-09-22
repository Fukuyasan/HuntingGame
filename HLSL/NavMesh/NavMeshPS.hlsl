#include "NavMesh.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

Texture2D texture0		 : register(t0);
SamplerState sampler0[3] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	return texture0.Sample(sampler0[ANISOTROPIC], pin.texcoord) * pin.color;
}