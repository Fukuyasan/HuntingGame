// IBL
float2 IBL(float3 E, float3 N)
{
#if 1
	// 視線の反射ベクトル
	float3 R = reflect(-E, N); //視線と法線で反射
	// 反射ベクトルをUVに変換
	// R : -1.0 ～ 1.0
	// uv:西0.0 ～ 0.5東
	float3 R2 = R;
	R2.y = 0; // 上下方向を打ち消す
	R2 = normalize(R2); //純粋な方向
	float2 euv;
	euv.x = R2.x * 0.25 + 0.25;
	// 南半球対応
	if (R.z < 0)
	{
		// 西0 <---> 0.5東
		// 西1.0 <---> 0.5東
		euv.x = 1.0 - euv.x;
	}
	
	euv.y = -R.y * 0.5 + 0.5;
#else
	
#endif
	return euv;
}