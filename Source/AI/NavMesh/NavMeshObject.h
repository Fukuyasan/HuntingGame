#pragma once

#include "Graphics/Model/Model.h"

#include <DetourNavMesh.h>
#include <DetourTileCache.h>
#include <Recast.h>


// 経路探索での障害物作成
class NavMeshObstacles
{
public:
	NavMeshObstacles(Model* model);
	~NavMeshObstacles();

	// ビルド処理
	void Build(Model* model);

	// クリーン処理
	void Clean();

	// 更新処理
	void Update(const float& elapsedTime, dtNavMesh* navMesh);

	// 障害物取得
	dtTileCache* GetTIleCache() { return tileCache; }

private:
	dtTileCache* tileCache = nullptr;
};

