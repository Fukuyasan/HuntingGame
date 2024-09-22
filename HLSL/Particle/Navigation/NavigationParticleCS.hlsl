#include "../ParticleBase.hlsli"
#include "NavigationParticle.hlsli"

RWStructuredBuffer<ParticleStatus> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 DTid : SV_DISPATCHTHREADID)
{
	uint id = DTid.x;
	
	// ランダム値
	const float noise = 2.0f;
	float f0 = rand(id * noise);
	float f1 = rand(f0 * noise);
	float f2 = rand(f1 * noise);
	
	ParticleStatus status = particleBuffer[id];
	
#if 1
	// 移動速度の設定
	const float speedNoise = 25.0f;
	status.moveSpeed = (speedNoise * f2) * deltaTime;
	
	// ターゲット
	float theta = asin(1 - 2 * f2);
	float phi = f1 * 2 * PI - PI;
	float radius = 0.5f;
	
	float3 target;
	target.x = targetPosition.x + cos(theta) * cos(phi) * radius;
	target.y = targetPosition.y + cos(theta) * sin(phi) * radius;
	target.z = targetPosition.z + sin(theta) * radius;
	
	// ターゲットまでのベクトルを求める
	float3 vec = target - status.position;
	vec = normalize(vec);
	
	// 位置更新
	status.position += vec * status.moveSpeed;
	
#endif
		
	particleBuffer[id] = status;
}