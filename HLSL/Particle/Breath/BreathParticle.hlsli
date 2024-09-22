cbuffer CBBreathParticle : register(b13)
{
	float3 position;
	float  particleSize;
	float  time;
	float  deltaTime;
};

struct ParticleStatus
{
	float3 position;
	float4 color;
	float3 velocity;
	float  age;
	int    state;
};
