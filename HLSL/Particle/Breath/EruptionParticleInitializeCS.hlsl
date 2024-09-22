#include "../ParticleBase.hlsli"
#include "BreathParticle.hlsli"

RWStructuredBuffer<ParticleStatus> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint id = DTid.x;
	
	ParticleStatus status = particleBuffer[id];
	
	const float noiseScale = 2.0;
	float f0 = rand(id * noiseScale);
	float f1 = rand(f0 * noiseScale);
	float f2 = rand(f1 * noiseScale);
	
	// 位置設定 : 〇の形にする
	// 球の点群を作る
	float theta  = asin(1 - 2 * f2);
	float phi    = f1 * 2 * PI - PI;
	float hphi   = f1 * PI;
	float radius = 2.0f * f0;
	
	// 位置
	status.position.x = position.x + cos(theta) * cos(hphi) * radius;
	status.position.y = position.y;
	status.position.z = position.z + sin(theta) * radius;

	radius = 50.0f * f0;
	// 速度
	status.velocity.x = cos(theta) * cos(hphi) * radius;
	status.velocity.y = cos(theta) * sin(hphi) * radius;
	status.velocity.z = sin(theta) * radius;
	
	// 色設定
	// 青
	status.color.x = 1.0;
	status.color.y = 0.2;
	status.color.z = 0.0;
	status.color.w = 1.0f;
	
	status.color  *= 3.0;
	
	status.age   = 10.0;
	status.state = 0;

	particleBuffer[id] = status;
}