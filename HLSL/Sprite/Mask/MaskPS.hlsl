#include "../Sprite.hlsli"
#include "Mask.hlsli"

Texture2D texture0	  : register(t0);
Texture2D maskTexture : register(t22);

SamplerState sampler0[5] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = texture0.Sample(sampler0[LINEAR], pin.texcoord) * pin.color;

	// マスク画像から赤色を取得
	float mask = maskTexture.Sample(sampler0[LINEAR], pin.texcoord).r;

	float alpha = step(mask, dissolveThreshold);
    
    // 縁の処理
	float edgeValue = step(dissolveThreshold - edgeThreshold, mask);
	edgeValue *= alpha;

	color.rgb += edgeColor.rgb * edgeValue;
	alpha = saturate(alpha + edgeValue);

	// colorの透過値に乗算する
	color.a *= alpha;

    // アルファが0以下ならそもそも描画しないようにする
	clip(color.a - 0.01f);

	return color;
}