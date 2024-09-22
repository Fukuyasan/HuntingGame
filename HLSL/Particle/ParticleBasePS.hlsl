#include "ParticleBase.hlsli"

float4 main(GS_OUT pin) : SV_TARGET
{
#if 1
	// ƒKƒ“ƒ}ˆ—
	const float GAMMA = 2.2f;
	pin.color.rgb = pow(pin.color.rgb, GAMMA);
#endif
	
	float dist  = length(pin.texcoord - 0.5) * 2.0;
	float alpha = 1.0 - saturate(dist);
	pin.color.a *= alpha;
	
	return pin.color;
}