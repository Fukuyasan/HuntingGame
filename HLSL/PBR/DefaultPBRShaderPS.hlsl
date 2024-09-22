#include "PBRShader.hlsli"

#define MAX_SPECULAR 2.2f

Texture2D albedoMap			  : register(t0);
Texture2D normalMap			  : register(t1);
Texture2D metallicMap		  : register(t2);
Texture2D roughnessMap		  : register(t3);
Texture2D ambientOcclusionMap : register(t4);

Texture2D environment		  : register(t10);
Texture2D shadowMap			  : register(t15);
Texture2D bayerTex			  : register(t21);

SamplerState samplerState[5] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{	
	// テクスチャ情報取得
	float4 albedo          = albedoMap.Sample(samplerState[ANISOTROPIC],           pin.texcoord * textureNarrow);
	float3 normal		   = normalMap.Sample(samplerState[ANISOTROPIC],           pin.texcoord * textureNarrow).xyz;
	float metallic         = metallicMap.Sample(samplerState[ANISOTROPIC],         pin.texcoord * textureNarrow).r;
	float roughness        = roughnessMap.Sample(samplerState[ANISOTROPIC],        pin.texcoord * textureNarrow).r;
	float ambientOcclusion = ambientOcclusionMap.Sample(samplerState[ANISOTROPIC], pin.texcoord * textureNarrow).r;
	
	// 入射光のうち拡散反射になる割合
	float3 diffuseReflectance = lerp(albedo.rgb, 0.02f, metallic);
	
	// 垂直反射時のフレネル反射率
	float3 F0 = lerp(Dielectric, albedo.rgb, metallic);
	
	float3 E = normalize(EyePos.xyz - pin.worldPosition);
	float3 L = normalize(lightDir.xyz);
	
	// 法線マップからxyz成分を取得して( -1 ～ +1 )の間にスケーリング
	normal      = normal.xyz * 2 - 1;
	float3x3 CM = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
	float3 N    = normalize(mul(normal, CM));
	
	// IBL
	float2 ibl = IBL(E, N);
	
	// 直接光の拡散反射と鏡面反射
	float3 directDiffuse = 0, directSpecular = 0;
	DirectBDRF(diffuseReflectance,
			   F0,
			   N,
			   E,
			   L,
			   lightColor.rgb,
			   roughness,
			   directDiffuse,
			   directSpecular);
	
	// 間接光の鏡面反射(映り込み)
	float3 indirectSpecular = 0;
	{
		// テクスチャからミップマップの情報を取得する
		uint width, height, mipMapLevel;
		environment.GetDimensions(0, width, height, mipMapLevel);
		int mipLevel = lerp(2, mipMapLevel - 1, roughness);
		// スカイマップから映り込みの情報を取得する
		float3 raddiance = environment.SampleLevel(samplerState[ANISOTROPIC], ibl, mipLevel).rgb;
		indirectSpecular = EnvironmentBRDFApprox(F0, roughness, max(0.0001f, dot(N, E))) * raddiance;
	}
	
	// 平行光源に対して影を適応
	float3 shadow = CalcShadowColorPCFFilter(shadowMap, samplerState[LINEAR_BORDER_WHITE], pin.shadowTexcoord, shadowColor, shadowBias);
	
	// 影の計算
	directDiffuse  *= shadow;
	directSpecular *= shadow;
	
	// ディザ抜き
	albedo.a = DitheringFromCameraDistance(pin, EyePos.xyz, bayerTex, samplerState[ANISOTROPIC]);
	
	return float4(directDiffuse + directSpecular + indirectSpecular, albedo.a);
}

