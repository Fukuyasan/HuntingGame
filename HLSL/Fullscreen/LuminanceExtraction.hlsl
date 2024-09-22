#include "FullscreenQuad.hlsli"

SamplerState samplerState[5]  : register(s0);
Texture2D	 textureMap[2]    : register(t6);

float4 main(VS_OUT pin) : SV_TARGET
{
	// �Q�[���V�[���̐F�擾
	float4 color = textureMap[0].Sample(samplerState[ANISOTROPIC], pin.texcoord);
	
	// �P�x���o
	color.rgb = smoothstep(minL, maxL, dot(color.rgb, float3(0.299, 0.587, 0.114))) * color.rgb;
	
	return color;
}