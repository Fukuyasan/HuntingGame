struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

cbuffer LIMINANCE_BUFFER : register(b6)
{
	float minL;
	float maxL;
	float gaussianSigma;
	float bloomIntensity;
	float exposure;
}

cbuffer CbHeatHaze : register(b10)
{
	float amplitude;
	float depthOffset;
	float timeScale;
	float seedScale;
	float4 colorBalance;
	float time;
};

cbuffer CbShockBlur : register(b11)
{
	float2 centerTexel;
	float  blurPower;
};

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
#define LINEAR_CLAMP 5
#define PI  3.14159265358979