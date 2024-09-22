// IBL
float2 IBL(float3 E, float3 N)
{
#if 1
	// üÌ½ËxNg
	float3 R = reflect(-E, N); //üÆ@üÅ½Ë
	// ½ËxNgðUVÉÏ·
	// R : -1.0 ` 1.0
	// uv:¼0.0 ` 0.5
	float3 R2 = R;
	R2.y = 0; // ãºûüðÅ¿Á·
	R2 = normalize(R2); //Èûü
	float2 euv;
	euv.x = R2.x * 0.25 + 0.25;
	// ì¼Î
	if (R.z < 0)
	{
		// ¼0 <---> 0.5
		// ¼1.0 <---> 0.5
		euv.x = 1.0 - euv.x;
	}
	
	euv.y = -R.y * 0.5 + 0.5;
#else
	
#endif
	return euv;
}