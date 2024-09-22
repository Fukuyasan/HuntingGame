#include "FullscreenQuad.hlsli"
#include "../Functions.hlsli"

SamplerState samplerState[6] : register(s0);
Texture2D textureMap[7] : register(t6);

float4 main(VS_OUT pin) : SV_TARGET
{		
	// �V�[���̃J���[���擾
	float4 sceneColor     = textureMap[0].Sample(samplerState[ANISOTROPIC], pin.texcoord);
	// �z��
	float4 heatColor      = textureMap[1].Sample(samplerState[LINEAR_BORDER_BLACK], pin.texcoord);
	// ���W�A���u���[
	float4 shockBlurColor = textureMap[2].Sample(samplerState[LINEAR_BORDER_BLACK], pin.texcoord);
	
	// �u���[
	float3 blurColor          = 0;
	float gaussianKernelTotal = 0;
	
	const int gaussianHalfKernelSize = 3;
	float sigmaPow2           = gaussianSigma * gaussianSigma;

	// �쐣���u���[
	for (int j = 0; j < 4; ++j)
	{
		int i = j + 3;  // texture�� 3 ~ 6 ���u���[�̃e�N�X�`��
		
		// �V�F�[�_�[���\�[�X�r���[�̕��A�������擾
		uint mipLevel = 0, width, height, numberOfLevels;
		textureMap[i].GetDimensions(mipLevel, width, height, numberOfLevels);
		
		[unroll]
		for (int x = -gaussianHalfKernelSize; x <= +gaussianHalfKernelSize; x += 1)
		{
			[unroll]
			for (int y = -gaussianHalfKernelSize; y <= +gaussianHalfKernelSize; y += 1)
			{
				float gaussianKernel = exp(-((x * x + y * y) / 2 * sigmaPow2)) / (2 * PI * sigmaPow2);

				blurColor += textureMap[i].Sample(samplerState[LINEAR_BORDER_BLACK], pin.texcoord +
					float2(x * 1.0 / width, y * 1.0 / height)).rgb * gaussianKernel;
			
				gaussianKernelTotal += gaussianKernel;
			}
		}
	}

	// �u���[
	blurColor /= gaussianKernelTotal;
	sceneColor.rgb = blurColor * bloomIntensity;
	
	// �c��̃e�N�X�`�������킹��
	sceneColor += shockBlurColor;
	sceneColor += heatColor;
	
	// COLOR_BALANCE
	sceneColor.rgb *= colorBalance.rgb * colorBalance.a;
			
#if 1
	// �g�[���}�b�s���O : HDR ->SDR
	sceneColor.rgb = 1 - exp(-sceneColor.rgb * exposure);
	
	// �K���}����
	const float GAMMA = 2.2;
	sceneColor.rgb = pow(sceneColor.rgb, 1.0 / GAMMA);
#endif
	
	return sceneColor;
}