#include "NavMeshAgent.h"
#include "System/Misc.h"
#include <DetourNavMeshBuilder.h>

#define MAX_AGENT  128
#define MAX_RADIUS 32

NavMeshAgent::NavMeshAgent(NavMesh* navMesh)
{
	// �Q�O�I�u�W�F�N�g�̊��蓖��
	crowd = dtAllocCrowd();

	// �Q�O�I�u�W�F�N�g�̐ݒ�
	bool result = crowd->init(MAX_AGENT, MAX_RADIUS, navMesh->GetNavMesh());
	_ASSERT_EXPR_A(result, "Could not init Detour crowd");
}

NavMeshAgent::~NavMeshAgent()
{
	Clean();
}

// �G�[�W�F���g�ǉ�
void NavMeshAgent::AddAgent(const DirectX::XMFLOAT3& position, const AgentParams& params)
{
	// �G�[�W�F���g�̃p�����[�^�ݒ�
	dtCrowdAgentParams ap;
	memset(&ap, 0, sizeof(ap));
	ap.radius                = params.radius;
	ap.height                = params.height;
	ap.maxAcceleration       = params.maxAcceleration;
	ap.maxSpeed              = params.maxSpeed;
	ap.collisionQueryRange   = ap.radius * 6.0f;
	ap.pathOptimizationRange = ap.radius * 30.0f;

	ap.updateFlags  = 0;
	ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
	ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	ap.updateFlags |= DT_CROWD_SEPARATION;
	ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
	ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;

	ap.obstacleAvoidanceType = 3;
	ap.separationWeight      = 2.0f;

	// �G�[�W�F���g�o�^
	crowd->addAgent(&position.x, &ap);
}

// �N���[������
void NavMeshAgent::Clean()
{
	// �Q�O�I�u�W�F�N�g�̉��
	dtFreeCrowd(crowd);
	crowd = nullptr;
}

void NavMeshAgent::Update(const float& elapsedTime)
{
	crowd->update(elapsedTime, nullptr);
}

void NavMeshAgent::StartMove(const int id, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal)
{
	const dtNavMeshQuery* navQuery	  = crowd->getNavMeshQuery();
	const dtQueryFilter*  filter	  = crowd->getFilter(0);
	const float*		  halfExtents = crowd->getQueryExtents();
	dtCrowdAgent*		  ag		  = crowd->getEditableAgent(id);

	// �G�[�W�F���g�̃X�^�[�g�ʒu�������X�^�[�̈ʒu��
	ag->npos[0] = start.x;
	ag->npos[1] = start.y;
	ag->npos[2] = start.z;

	// �w��|�C���g�ֈړ�
	// �w��|�C���g�����ԋ߂����[�g��T��?
	dtPolyRef polyRef;
	navQuery->findNearestPoly(&goal.x, halfExtents, filter, &polyRef, 0);

	// �G�[�W�F���g�̈ړ�
	crowd->requestMoveTarget(id, polyRef, &goal.x);
}

void NavMeshAgent::Target(const int id, const DirectX::XMFLOAT3& target)
{
	const dtNavMeshQuery* navQuery = crowd->getNavMeshQuery();
	const dtQueryFilter* filter    = crowd->getFilter(0);
	const float* halfExtents       = crowd->getQueryExtents();

	// �w��|�C���g�ֈړ�
	// �w��|�C���g�����ԋ߂����[�g��T��?
	dtPolyRef polyRef;
	navQuery->findNearestPoly(&target.x, halfExtents, filter, &polyRef, 0);

	// �G�[�W�F���g�̈ړ�
	crowd->requestMoveTarget(id, polyRef, &target.x);
}
