#include "../ParticleBase.hlsli"
#include "BreathParticle.hlsli"

RWStructuredBuffer<ParticleStatus> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint id = DTid.x;
	
	float f0 = rand(id * 2);
	float f1 = rand(f0 * 2);
	float f2 = rand(f1 * 2);
	
	ParticleStatus status = particleBuffer[id];
	
	status.position += status.velocity * deltaTime;
	
	if (status.age < 0.0)
	{
		status.color.w = 0.0f;
	}
	status.age -= deltaTime;
	
	const float maxAge = 10.0;
	float rate = 1 - (status.age / maxAge);
	status.velocity = lerp(status.velocity, 0, rate * rate);
	status.color.w  = lerp(status.color.w,  0, rate * rate);
	
	particleBuffer[id] = status;
}
