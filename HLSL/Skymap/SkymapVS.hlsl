#include "Skymap.hlsli"

VS_OUT main( float4 position : POSITION)
{
	position.z = 1.0f;
	VS_OUT vout;
	vout.position = position;

	//NDC座標からワールド座標に変換
	float4 p;
	p = mul(vout.position, inverseProjection);
	p /= p.w;
	p = mul(p, inverseView);
	vout.worldPosition = float4(p.xyz, 1);
	
	return vout;
}