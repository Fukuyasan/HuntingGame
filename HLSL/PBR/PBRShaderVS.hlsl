#include "PBRShader.hlsli"

VS_OUT main(
	float4 position    : POSITION,
	float3 normal      : NORMAL,
	float3 tangent     : TANGENT,
	float2 texcoord    : TEXCOORD,
	float4 color       : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices  : BONES
)
{
	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	float3 t = { 0, 0, 0 };
	
	for (int i = 0; i < 4; i++)
	{
		p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
		n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
		t += (boneWeights[i] * mul(float4(tangent.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
		
	}

	VS_OUT vout;
	vout.position = mul(float4(p, 1.0), viewProjection);
	vout.worldPosition	= p;
	
	float3 N    = normalize(n);
	float3 L    = normalize(-lightDirection.xyz);
	float  d    = dot(L, N);
	float power = max(0, d) * 0.5f + 0.5f;
	
	vout.normal   = N;
	vout.tangent  = normalize(t);
	vout.binormal = normalize(cross(vout.tangent, vout.normal));
		
	vout.color.rgb = color.rgb * materialColor.rgb * power;
	vout.color.a   = color.a * materialColor.a;
	vout.texcoord  = texcoord;
	
	// シャドウマップで使用する情報を算出
	vout.shadowTexcoord = CalcShadowTexcoord(p, lightViewProjection);
	
	return vout;
}
