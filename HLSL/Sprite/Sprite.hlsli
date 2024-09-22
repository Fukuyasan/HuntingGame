struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
	float2 texcoord : TEXCOORD;
};

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2