cbuffer CbMask : register(b0)
{
	float dissolveThreshold; // ディゾルブ量
	float edgeThreshold;	 // 縁の閾値
	float2 dummy;
	float4 edgeColor;		 // 縁の色

};