#pragma once

#include "Graphics/Model/Model.h"

#include <DetourNavMesh.h>
#include <DetourTileCache.h>
#include <Recast.h>


// �o�H�T���ł̏�Q���쐬
class NavMeshObstacles
{
public:
	NavMeshObstacles(Model* model);
	~NavMeshObstacles();

	// �r���h����
	void Build(Model* model);

	// �N���[������
	void Clean();

	// �X�V����
	void Update(const float& elapsedTime, dtNavMesh* navMesh);

	// ��Q���擾
	dtTileCache* GetTIleCache() { return tileCache; }

private:
	dtTileCache* tileCache = nullptr;
};

