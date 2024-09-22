#include "ImGui.hlsli"

VS_OUT main(float4 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
	VS_OUT vout;
	vout.position = mul(position, viewProjection);
	vout.texcoord = texcoord;
	vout.color    = color;

	return vout;
}
