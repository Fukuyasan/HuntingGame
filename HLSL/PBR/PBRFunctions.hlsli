static const float PI = 3.1415926f;

//--------------------------------------------
// フレネル項
//--------------------------------------------
// F0    : 垂直入射時の反射率
// VdotH : 視線ベクトルとハーフベクトル
float3 CalcFresnel(float3 f0, float VdotH)
{
#if 1
	return f0 + (1.0f - f0) * pow(1.0f - VdotH, 5.0f);
#else	
	return f0 + (1.0f - f0) * pow((-5.55473f * VdotH - 6.98316f), VdotH);
#endif
}

//--------------------------------------------
// 法線分布関数
//--------------------------------------------
// NdotH	 : 法線ベクトルとハーフベクトル
// roughness : 粗いさん
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;
	return alpha2 / (PI * (denom * denom));
}

//--------------------------------------------
// 幾何減衰項の算出
//--------------------------------------------
// Ndotv : 法線と他ベクトルとの内積
// k	 : 粗いさん / 2
float CalcGeometryFunction(float Ndotv, float k)
{
	return Ndotv / (Ndotv * (1.0f - k) + k);
}

// NdotL	 : 法線ベクトルと光源へのベクトルとの内積
// NdotV	 : 法線ベクトルと視線へのベクトルとの内積
// roughness : 粗さ
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
	float r = (roughness + 1);
	float k = (r * r) / 8;
	float shadowing = CalcGeometryFunction(NdotL, k);
	float masking = CalcGeometryFunction(NdotV, k);
	return shadowing * masking;
}

//--------------------------------------------
// 鏡面反射 BRDF（クック・トランスのマイクロファセットモデル）
//--------------------------------------------
// NdotV	 : 法線ベクトルと視線へのベクトルとの内積
// NdotL	 : 法線ベクトルと光源へのベクトルとの内積
// NdotH	 : 法線ベクトルとハーフベクトルとの内積
// VdotH	 : 視線へのベクトルとハーフベクトルとの内積
// fresnelF0 : 垂直入射時のフレネル反射色
// roughness : 粗さ
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float
roughness)
{
	// D 項(法線分布)
	float D = CalcNormalDistributionFunction(NdotH, roughness);
	// G 項(幾何減衰項)
	float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	// F 項(フレネル反射)
	float F = CalcFresnel(fresnelF0, VdotH);
	
	return (D * F * G) / max(0.0001f, 4 * NdotL * NdotV);
}

//--------------------------------------------
// 拡散反射 BRDF(正規化ランバートの拡散反射)
//--------------------------------------------
// diffuseReflectance : 入射光のうち拡散反射になる割合
float3 DiffuseBRDF(float3 diffuseReflectance)
{
	return diffuseReflectance / PI;
}

//--------------------------------------------
// 直接光の物理ベースライティング
//--------------------------------------------
// diffuseReflectance : 入射光のうち拡散反射になる割合
// F0				  : 垂直入射時のフレネル反射色
// normal			  : 法線ベクトル(正規化済み)
// eyeVector		  : 視点に向かうベクトル(正規化済み)
// lightVector		  : 光源に向かうベクトル(正規化済み)
// lightColor		  : ライトカラー
// roughness	      : 粗さ
void DirectBDRF(float3 diffuseReflectance,
				float3 F0,
				float3 normal,
				float3 eyeVector,
				float3 lightVector,
				float3 lightColor,
				float roughness,
			out float3 outDiffuse,
			out float3 outSpecular)
{
	float3 N = normal;
	float3 L = -lightVector;
	float3 V = eyeVector;
	float3 H = normalize(L + V);
	float NdotV = max(0.0001f, dot(N, V));
	float NdotL = max(0.0001f, dot(N, L));
	float NdotH = max(0.0001f, dot(N, H));
	float VdotH = max(0.0001f, dot(V, H));
	float3 irradiance = lightColor * NdotL;
	// レガシーなライティングとの互換性を保つ場合は PI で乗算する
	irradiance *= PI;
	// 拡散反射 BRDF(正規化ランバートの拡散反射)
	outDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;
	// 鏡面反射 BRDF（クック・トランスのマイクロファセットモデル）
	outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}

//--------------------------------------------
// 環境光の BRDF
//--------------------------------------------
// F0		 : 垂直入射時のフレネル反射色
// roughness : 粗さ
// NdotV	 : 法線ベクトルと視線へのベクトルとの内積
float3 EnvironmentBRDFApprox(float3 F0, float roughness, float NdotV)
{
	const float4 c0 = { -1.0f, -0.0275f, -0.572f, 0.022f };
	const float4 c1 = { 1.0f, 0.0425f, 1.04f, -0.04f };
	float4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28f * NdotV)) * r.x + r.y;
	float2 AB = float2(-1.04f, 1.04f) * a004 + r.zw;
	return F0 * AB.x + AB.y;
}

//--------------------------------------------
//	ハーフランバート拡散反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// kd			: 反射率(反射の強さ)
float3 ClacHalfLambert(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{
	float d = (dot(lightVector, normal) * 0.5f + 0.5f) * kd;
	return d;
}