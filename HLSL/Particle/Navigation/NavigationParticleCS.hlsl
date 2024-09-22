#include "../ParticleBase.hlsli"
#include "NavigationParticle.hlsli"

RWStructuredBuffer<ParticleStatus> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 DTid : SV_DISPATCHTHREADID)
{
	uint id = DTid.x;
	
	// �����_���l
	const float noise = 2.0f;
	float f0 = rand(id * noise);
	float f1 = rand(f0 * noise);
	float f2 = rand(f1 * noise);
	
	ParticleStatus status = particleBuffer[id];
	
#if 1
	// �ړ����x�̐ݒ�
	const float speedNoise = 25.0f;
	status.moveSpeed = (speedNoise * f2) * deltaTime;
	
	// �^�[�Q�b�g
	float theta = asin(1 - 2 * f2);
	float phi = f1 * 2 * PI - PI;
	float radius = 0.5f;
	
	float3 target;
	target.x = targetPosition.x + cos(theta) * cos(phi) * radius;
	target.y = targetPosition.y + cos(theta) * sin(phi) * radius;
	target.z = targetPosition.z + sin(theta) * radius;
	
	// �^�[�Q�b�g�܂ł̃x�N�g�������߂�
	float3 vec = target - status.position;
	vec = normalize(vec);
	
	// �ʒu�X�V
	status.position += vec * status.moveSpeed;
	
#endif
		
	particleBuffer[id] = status;
}