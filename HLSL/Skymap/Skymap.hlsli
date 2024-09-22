struct VS_OUT
{
	float4 position      : SV_POSITION;
	float4 worldPosition : POSITION;
};

cbuffer CBScene : register(b12)
{
	row_major float4x4 inverseView;
	row_major float4x4 inverseProjection;
	float4 cameraPos;
};

static const float PI = 3.141592654f;