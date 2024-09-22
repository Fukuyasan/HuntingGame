#include "../ConstantBase.hlsli"

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4 boneTransforms[MAX_BONES];
};