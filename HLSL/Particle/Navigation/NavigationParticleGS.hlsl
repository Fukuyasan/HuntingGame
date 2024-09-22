#include "../ParticleBase.hlsli"
#include "NavigationParticle.hlsli"

StructuredBuffer<ParticleStatus> particleBuffer : register(t9);

[maxvertexcount(4)]
void main(
	point VS_OUT vin[1] : SV_POSITION, 
	inout TriangleStream<GS_OUT> gout
)
{
	const float2 corners[] =
	{
		float2(-1.0f, -1.0f),
		float2(-1.0f, +1.0f),
		float2(+1.0f, -1.0f),
		float2(+1.0f, +1.0f),
	};
	const float2 texcoords[] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
	};
	
	ParticleStatus p = particleBuffer[vin[0].vertexID];

	const float aspectRatio = 1280.0 / 720.0;
	float2 particle_scale = float2(particleSize, particleSize * aspectRatio);
	float f = rand(aspectRatio);

	[unroll]
	for (uint vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
	{
		GS_OUT gin;

		// Transform to clip space
		gin.position = mul(float4(p.position, 1), viewProjection);
		// Make corner position as billboard		
		gin.position.xy += corners[vertexIndex] * particle_scale * f;

		gin.color = p.color;
		gin.texcoord = texcoords[vertexIndex];
		gout.Append(gin);
	}

	gout.RestartStrip();
}