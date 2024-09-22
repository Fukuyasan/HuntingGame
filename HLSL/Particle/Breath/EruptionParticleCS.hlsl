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
	
	// 位置更新
	status.position += status.velocity * deltaTime;
	
	status.velocity.x += cos(f1 * time) * deltaTime;
	status.velocity.z += sin(f1 * time) * deltaTime;
	
	// 速度に重力を加える
	const float gravity = 9.8f;
	status.velocity.y += -gravity * deltaTime;
	
	// 透明度の計算
	const float maxAge = 10.0;
	const float rate = 1 - (status.age / maxAge);
	status.color.w = lerp(status.color.w, 0, rate * rate);
	
	particleBuffer[id] = status;
}