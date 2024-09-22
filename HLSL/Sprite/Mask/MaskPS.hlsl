#include "../Sprite.hlsli"
#include "Mask.hlsli"

Texture2D texture0	  : register(t0);
Texture2D maskTexture : register(t22);

SamplerState sampler0[5] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = texture0.Sample(sampler0[LINEAR], pin.texcoord) * pin.color;

	// �}�X�N�摜����ԐF���擾
	float mask = maskTexture.Sample(sampler0[LINEAR], pin.texcoord).r;

	float alpha = step(mask, dissolveThreshold);
    
    // ���̏���
	float edgeValue = step(dissolveThreshold - edgeThreshold, mask);
	edgeValue *= alpha;

	color.rgb += edgeColor.rgb * edgeValue;
	alpha = saturate(alpha + edgeValue);

	// color�̓��ߒl�ɏ�Z����
	color.a *= alpha;

    // �A���t�@��0�ȉ��Ȃ炻�������`�悵�Ȃ��悤�ɂ���
	clip(color.a - 0.01f);

	return color;
}