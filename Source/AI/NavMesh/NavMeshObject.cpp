#include "NavMeshObject.h"
#include <DetourNavMesh.h>
#include <DetourTileCacheBuilder.h>
#include "System/Misc.h"
#include <map>

NavMeshObstacles::NavMeshObstacles(Model* model)
{
	Build(model);
}

NavMeshObstacles::~NavMeshObstacles()
{
	Clean();
}

// ビルド処理
void NavMeshObstacles::Build(Model* model)
{
	// モデルから頂点を抽出
	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<int> indices;
	// バウンディングボックス	
	DirectX::XMFLOAT3 boundMin, boundMax;

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ノード取得
		const auto& nodes = model->GetNodes();
		const Model::Node& node = nodes.at(mesh.nodeIndex);

		// ワールド空間頂点座標算出
		DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).worldTransform);

		// バウンディングボックスのための頂点算出
		DirectX::XMVECTOR V = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh.boundsMin), W);
		DirectX::XMStoreFloat3(&boundMin, V);

		V = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh.boundsMax), W);
		DirectX::XMStoreFloat3(&boundMax, V);

	}	
	
	// パラメータ設定
	dtTileCacheParams params;
	memset(&params, 0, sizeof(params));

	rcVcopy(params.orig, &boundMin.x);
	params.cs                     = 10.0f;
	params.ch                     = 1.0f;
	params.width                  = (int)params.cs;
	params.height                 = (int)params.cs;
	params.walkableHeight         = 1.0f;
	params.walkableRadius         = 1.0f;
	params.walkableClimb          = 1.0f;
	params.maxSimplificationError = 1.3f;
	params.maxTiles               = 128;
	params.maxObstacles           = 128;

	// 障害物の割り当て
	tileCache = dtAllocTileCache();

	// 障害物初期化
	dtStatus status;
	status = tileCache->init(&params, nullptr, nullptr, nullptr);
	_ASSERT_EXPR_A(status, "buildNavigation: Out of memory 'pmdtl'.");

	status = tileCache->addBoxObstacle(&boundMin.x, &boundMax.x, 0);
}

// クリーン処理
void NavMeshObstacles::Clean()
{
	// オブジェクトの解放
	dtFreeTileCache(tileCache);
	tileCache = nullptr;
}

// 更新処理
void NavMeshObstacles::Update(const float& elapsedTime,dtNavMesh* navMesh)
{
	tileCache->update(elapsedTime, navMesh);
}
