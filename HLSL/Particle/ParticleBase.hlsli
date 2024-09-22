//#include "../ConstantBase.hlsli"

struct VS_OUT
{
	uint vertexID : VERTEXID;
};

struct GS_OUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
};

float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}

cbuffer CbScene : register(b0)
{
	row_major float4x4 viewProjection;
};


#define NUMTHREADS_X 16
#define PI			 3.14