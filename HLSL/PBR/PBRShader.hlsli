#include "../ConstantBase.hlsli"
#include "ShadowmapFunctions.hlsli"
#include "PBRFunctions.hlsli"
#include "IBLFunctions.hlsli"

struct VS_OUT
{
	float4 position		  : SV_POSITION;
	float3 worldPosition  : POSITION;
	float3 normal		  : NORMAL;
	float3 tangent		  : TANGENT;
	float3 binormal		  : BINORMAL;
	float2 texcoord		  : TEXCOORD;
	float4 color		  : COLOR;
	float3 shadowTexcoord : SHADOW;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4 boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
	float4 materialColor;
	int textureNarrow;
	float3 dummy;
};

cbuffer CbPerFrame : register(b3)
{
	float4 lightDir;
	float4 lightColor;
	float4 EyePos;
	//float3 playerPos;
};

cbuffer CbShadowmap : register(b4)
{
	row_major float4x4 lightViewProjection;
	float3 shadowColor;
	float shadowBias;
};

// �U�d���i������ł� �Œ� 4%(0.0f4) �͋��ʔ��˂��邽�߁A������v�Z���邽�߂̒萔�j
static const float3 Dielectric = 0.04f;

//--------------------------------------------
// �������C�e�B���O
//--------------------------------------------
// Normal      : �@��
// SkyColor    : ��F
// GroundColor : �n�ʐF
float3 HemiSphereLighting(float3 Normal, float3 SkyColor, float3 GroundColor)
{
	return lerp(GroundColor, SkyColor, Normal.y);
}

//--------------------------------------------
//	�J�����̋����Ńf�B�U�����O����
//--------------------------------------------
float DitheringFromCameraDistance(VS_OUT pin, float3 eyePos, Texture2D ditheringTex, SamplerState state)
{
	const float range = 4.0f;
	const float2 screenSize = { 1280, 720 };
	const float blockSize = 5.0f;
	
	// �J��������s�N�Z���̋���
	//float playerDist = distance(playerPos, eyePos.xyz);
	float dist = distance(pin.worldPosition, eyePos.xyz);
	
	//if (playerDist < dist) return pin.color.w;
	
	dist = saturate(dist / range);
	
	// �X�N���[�����W
	float2 screenPos = pin.position.xy / screenSize;
	
	// UV���v�Z
	float2 uv = screenPos * (screenSize / blockSize);

	float threshold = ditheringTex.Sample(state, uv).r;
	
	float alpha = dist - threshold;
	
	// alpha��0�������Ȃ��悤�ɂ���
	clip(alpha);
	
	return pin.color.w;
}