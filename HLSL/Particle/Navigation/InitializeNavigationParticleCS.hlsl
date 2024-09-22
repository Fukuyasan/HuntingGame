#include "../ParticleBase.hlsli"
#include "NavigationParticle.hlsli"

RWStructuredBuffer<ParticleStatus> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint id = DTid.x;
	
	const float noise = 2.0f;
	float f0 = rand(id * noise);
	float f1 = rand(f0 * noise);
	float f2 = rand(f1 * noise);
	
	ParticleStatus status = particleBuffer[id];
	
	// �ʒu�ݒ� : �Z�̌`�ɂ���
	// ���̓_�Q�����
	float theta = asin(1 - 2 * f2);
	float phi = f1 * 2 * PI - PI;
	float radius = 0.5f;
	
	// �ʒu�ݒ� : �Z�̌`�ɂ���
	status.position.x = emitterPosition.x + cos(theta) * cos(phi) * radius;
	status.position.y = emitterPosition.y + cos(theta) * sin(phi) * radius;
	status.position.z = emitterPosition.z + sin(theta) * radius;
	
	// �������肩��o������
	status.position.y += 0.7f;
	
	// �O�����ݒ�
	status.direction = normalize(direction);
	
	// �F�ݒ�@�F�@��
	status.color.x = 0.1;
	status.color.y = 1.0;
	status.color.z = 0.1;
	status.color.w = 1.0f;
	
	status.color.y *= 3.0;
	
	status.state = 0;

	particleBuffer[id] = status;
}