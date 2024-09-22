#include "FullscreenQuad.hlsli"
#include "../Functions.hlsli"

SamplerState samplerState[6] : register(s0);
Texture2D textureMap[7] : register(t6);

float4 main(VS_OUT pin) : SV_TARGET
{		
	// シーンのカラーを取得
	float4 sceneColor     = textureMap[0].Sample(samplerState[ANISOTROPIC], pin.texcoord);
	// 陽炎
	float4 heatColor      = textureMap[1].Sample(samplerState[LINEAR_BORDER_BLACK], pin.texcoord);
	// ラジアルブラー
	float4 shockBlurColor = textureMap[2].Sample(samplerState[LINEAR_BORDER_BLACK], pin.texcoord);
	
	// ブラー
	float3 blurColor          = 0;
	float gaussianKernelTotal = 0;
	
	const int gaussianHalfKernelSize = 3;
	float sigmaPow2           = gaussianSigma * gaussianSigma;

	// 川瀬式ブラー
	for (int j = 0; j < 4; ++j)
	{
		int i = j + 3;  // textureの 3 ~ 6 がブラーのテクスチャ
		
		// シェーダーリソースビューの幅、高さを取得
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

	// ブラー
	blurColor /= gaussianKernelTotal;
	sceneColor.rgb = blurColor * bloomIntensity;
	
	// 残りのテクスチャも合わせる
	sceneColor += shockBlurColor;
	sceneColor += heatColor;
	
	// COLOR_BALANCE
	sceneColor.rgb *= colorBalance.rgb * colorBalance.a;
			
#if 1
	// トーンマッピング : HDR ->SDR
	sceneColor.rgb = 1 - exp(-sceneColor.rgb * exposure);
	
	// ガンマ処理
	const float GAMMA = 2.2;
	sceneColor.rgb = pow(sceneColor.rgb, 1.0 / GAMMA);
#endif
	
	return sceneColor;
}