#include "FullscreenQuad.hlsli"

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
	const float2 position[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
	const float2 texcoord[4] = { {  0,  0 }, {  1,  0 }, {  0,  1 }, {  1,  1 } };
	
	VS_OUT vout;
	vout.position = float4(position[vertexid], 0, 1);
	vout.texcoord = texcoord[vertexid];
	
	return vout;
}