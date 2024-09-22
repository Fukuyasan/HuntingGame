cbuffer CBNavigationParticle : register(b9)
{
	float3 emitterPosition;
	float  particleSize;	
	float3 direction;
	float  time;
	float3 targetPosition;
	float  deltaTime;
};

struct ParticleStatus
{
	float3 position;
	float3 direction;
	float3 moveSpeed;
	float4 color;
	int    state;
};
