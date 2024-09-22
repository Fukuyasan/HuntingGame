#include "sprite.hlsli"

Texture2D texture0		 : register(t0);
SamplerState sampler0[5] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = texture0.Sample(sampler0[LINEAR], pin.texcoord) * pin.color;
	float alpha  = color.a;
#if 0
	// ‹tƒKƒ“ƒ}ˆ—
	const float GAMMA = 2.2;
	color.rgb = pow(color.rgb, GAMMA);
#endif
	return float4(color.rgb, alpha) * pin.color;

}

