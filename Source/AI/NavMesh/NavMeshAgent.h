#pragma once
#include <DetourNavMesh.h>
#include <DetourCrowd.h>
#include <Recast.h>
#include <DirectXMath.h>

#include "NavMesh.h"

// 群衆オブジェクトのパラメータ
struct AgentParams
{
	float radius;
	float height;
	float maxAcceleration;
	float maxSpeed;
};

// 群衆オブジェクト
class NavMeshAgent
{
public:
	NavMeshAgent(NavMesh* navMesh);
	~NavMeshAgent();

	// エージェント追加
	void AddAgent(const DirectX::XMFLOAT3& position, const AgentParams& params);

	// クリーン処理
	void Clean();

	// 更新処理
	void Update(const float& elapsedTime);

	// GUI描画
	void DrawGUI() {}

	// 群衆取得
	dtCrowd* GetCrowd() { return crowd; }

	// 移動開始
	void StartMove(
		const int id,
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& goal);

	void Target(const int id,
		const DirectX::XMFLOAT3& target);

private:
	dtCrowd* crowd = nullptr;
};
