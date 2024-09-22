// IBL
float2 IBL(float3 E, float3 N)
{
#if 1
	// �����̔��˃x�N�g��
	float3 R = reflect(-E, N); //�����Ɩ@���Ŕ���
	// ���˃x�N�g����UV�ɕϊ�
	// R : -1.0 �` 1.0
	// uv:��0.0 �` 0.5��
	float3 R2 = R;
	R2.y = 0; // �㉺������ł�����
	R2 = normalize(R2); //�����ȕ���
	float2 euv;
	euv.x = R2.x * 0.25 + 0.25;
	// �씼���Ή�
	if (R.z < 0)
	{
		// ��0 <---> 0.5��
		// ��1.0 <---> 0.5��
		euv.x = 1.0 - euv.x;
	}
	
	euv.y = -R.y * 0.5 + 0.5;
#else
	
#endif
	return euv;
}