#include "ImGui.hlsli"

Texture2D    texture0 : register(t0);
SamplerState samplers[5] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	return texture0.Sample(samplers[LINEAR], pin.texcoord) * pin.color;
}
