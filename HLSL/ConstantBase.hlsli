cbuffer CbScene : register(b0)
{
	row_major float4x4 viewProjection;
	float4 lightDirection;
};

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4