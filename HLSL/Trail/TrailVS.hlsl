#include "Trail.hlsli"

VS_OUT main(float3 pos : POSITION, float2 tex : TEXCOORD)
{
	VS_OUT vout;
	const float2 uvScroll = { -0.5f, 0.5f };
	
	vout.position = mul(float4(pos, 1), ViewProjection);
	vout.texcoord = tex /*+ uvScroll*/;
	vout.color    = 1;
	
	return vout;
}