#include "NavMeshAgent.h"
#include "System/Misc.h"
#include <DetourNavMeshBuilder.h>

#define MAX_AGENT  128
#define MAX_RADIUS 32

NavMeshAgent::NavMeshAgent(NavMesh* navMesh)
{
	// 群衆オブジェクトの割り当て
	crowd = dtAllocCrowd();

	// 群衆オブジェクトの設定
	bool result = crowd->init(MAX_AGENT, MAX_RADIUS, navMesh->GetNavMesh());
	_ASSERT_EXPR_A(result, "Could not init Detour crowd");
}

NavMeshAgent::~NavMeshAgent()
{
	Clean();
}

// エージェント追加
void NavMeshAgent::AddAgent(const DirectX::XMFLOAT3& position, const AgentParams& params)
{
	// エージェントのパラメータ設定
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

	// エージェント登録
	crowd->addAgent(&position.x, &ap);
}

// クリーン処理
void NavMeshAgent::Clean()
{
	// 群衆オブジェクトの解放
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

	// エージェントのスタート位置をモンスターの位置に
	ag->npos[0] = start.x;
	ag->npos[1] = start.y;
	ag->npos[2] = start.z;

	// 指定ポイントへ移動
	// 指定ポイントから一番近いルートを探す?
	dtPolyRef polyRef;
	navQuery->findNearestPoly(&goal.x, halfExtents, filter, &polyRef, 0);

	// エージェントの移動
	crowd->requestMoveTarget(id, polyRef, &goal.x);
}

void NavMeshAgent::Target(const int id, const DirectX::XMFLOAT3& target)
{
	const dtNavMeshQuery* navQuery = crowd->getNavMeshQuery();
	const dtQueryFilter* filter    = crowd->getFilter(0);
	const float* halfExtents       = crowd->getQueryExtents();

	// 指定ポイントへ移動
	// 指定ポイントから一番近いルートを探す?
	dtPolyRef polyRef;
	navQuery->findNearestPoly(&target.x, halfExtents, filter, &polyRef, 0);

	// エージェントの移動
	crowd->requestMoveTarget(id, polyRef, &target.x);
}
