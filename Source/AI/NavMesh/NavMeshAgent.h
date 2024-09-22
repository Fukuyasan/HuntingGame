#pragma once
#include <DetourNavMesh.h>
#include <DetourCrowd.h>
#include <Recast.h>
#include <DirectXMath.h>

#include "NavMesh.h"

// �Q�O�I�u�W�F�N�g�̃p�����[�^
struct AgentParams
{
	float radius;
	float height;
	float maxAcceleration;
	float maxSpeed;
};

// �Q�O�I�u�W�F�N�g
class NavMeshAgent
{
public:
	NavMeshAgent(NavMesh* navMesh);
	~NavMeshAgent();

	// �G�[�W�F���g�ǉ�
	void AddAgent(const DirectX::XMFLOAT3& position, const AgentParams& params);

	// �N���[������
	void Clean();

	// �X�V����
	void Update(const float& elapsedTime);

	// GUI�`��
	void DrawGUI() {}

	// �Q�O�擾
	dtCrowd* GetCrowd() { return crowd; }

	// �ړ��J�n
	void StartMove(
		const int id,
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& goal);

	void Target(const int id,
		const DirectX::XMFLOAT3& target);

private:
	dtCrowd* crowd = nullptr;
};
