#include <map>
#include <string>
#include <DetourNavMeshBuilder.h>
#include "NavMesh.h"
#include "Graphics/Graphics.h" 
#include "System/Misc.h"

namespace DirectX
{
	bool operator <(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		if (lhs.x == rhs.x)
		{
			if (lhs.y == rhs.y)
			{
				return lhs.z < rhs.z;
			}
			return lhs.y < rhs.y;
		}
		return lhs.x < rhs.x;
	}
}

// クリーン処理
void NavMesh::Clean()
{
	// 各オブジェクトを解放

	// ハイトフィールド
	rcFreeHeightField(heightField);

	// 静的ハイトフィールド
	rcFreeCompactHeightfield(compactHeightField);

	rcFreeContourSet(contourSet);

	// ポリゴンメッシュ
	rcFreePolyMesh(polyMesh);

	// 詳細ポリゴンメッシュ
	rcFreePolyMeshDetail(polyMeshDetail);

	// ナビメッシュ
	dtFreeNavMesh(navMesh);

	// ナビメッシュクエリ
	dtFreeNavMeshQuery(navMeshQuery);
}

#pragma region モデルから頂点を抽出

void NavMesh::DetectedModel(Model* model)
{
	const ModelResource* resource = model->GetResource();
	// メッシュの数ループ
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ノード取得
		const auto& nodes = model->GetNodes();
		const Model::Node& node = nodes.at(mesh.nodeIndex);

		// 頂点座標データを取得
		std::vector<DirectX::XMFLOAT3> positions;

		// 頂点データ出力
		if (mesh.nodeIndices.size() > 0)
		{
			size_t offsetTransformsCount = mesh.offsetTransforms.size();

			// ボーン変換行列算出
			std::vector<DirectX::XMMATRIX> boneTransforms;
			boneTransforms.resize(offsetTransformsCount);

			for (size_t i = 0; i < offsetTransformsCount; ++i)
			{
				DirectX::XMMATRIX O = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);

				// ワールド行列を取得
				boneTransforms.at(i) = DirectX::XMMatrixMultiply(O, W);
			}

			// ワールド空間頂点座標算出
			for (const ModelResource::Vertex& vertex : mesh.vertices)
			{
				const int polyCount = 4;
				// 
				const float* boneWeights = &vertex.boneWeight.x;
				const UINT* boneIndices = &vertex.boneIndex.x;
				DirectX::XMVECTOR V = DirectX::XMVectorZero();
				for (int i = 0; i < polyCount; ++i)
				{
					DirectX::XMMATRIX B = boneTransforms.at(boneIndices[i]);
					DirectX::XMVECTOR P = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.position), B);
					V = DirectX::XMVectorAdd(V, DirectX::XMVectorScale(P, boneWeights[i]));
				}
				DirectX::XMFLOAT3 v;
				DirectX::XMStoreFloat3(&v, V);
				positions.emplace_back(v);
			}
		}
		else
		{
			// ワールド空間頂点座標算出
			DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).worldTransform);

			for (const ModelResource::Vertex& vertex : mesh.vertices)
			{
				DirectX::XMVECTOR V = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.position), W);
				DirectX::XMFLOAT3 v;
				DirectX::XMStoreFloat3(&v, V);
				positions.emplace_back(v);
			}
		}

		// トライアングルリスト最適化（重複する頂点を削除）
		std::map<DirectX::XMFLOAT3, int> caches;
		size_t meshCount = mesh.indices.size();
		for (size_t i = 0; i < meshCount; ++i)
		{
			UINT index = mesh.indices.at(i);

			UINT vertexCount = static_cast<UINT>(vertices.size());
			const DirectX::XMFLOAT3& position = positions.at(index);

			auto it = caches.find(position);

			// 同じ位置に別の頂点がある場合
			if (it == caches.end())
			{
				caches[position] = vertexCount;
				it = caches.find(position);

				vertices.emplace_back(position);
			}
			vertexCount = it->second;

			indices.emplace_back(vertexCount);
		}
	}

	// バウンディングボックス計算
	rcCalcBounds(&vertices.at(0).x, static_cast<int>(vertices.size()), &boundMin.x, &boundMax.x);
}
#pragma endregion

// デバッグ
void NavMesh::DrawGUI(/*Model* model*/)
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_Once);

	if (ImGui::Begin("NavMesh", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::DragFloat("cellSize", &cellSize, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("cellHeight", &cellHeight, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("maxAcceleration", &agentHeight, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("agentRadius", &agentRadius, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("agentMaxClimb", &agentMaxClimb, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("agentMaxSlope", &agentMaxSlope, 0.0f, 1.0f, 90.0f, "%.3f");
		ImGui::DragFloat("regionMinSize", &regionMinSize, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("regionMergeSize", &regionMergeSize, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("edgeMaxLen", &edgeMaxLen, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("edgeMaxError", &edgeMaxError, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("vertsPerPoly", &vertsPerPoly, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("detailSampleDist", &detailSampleDist, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("detailSampleMaxError", &detailSampleMaxError, 0.1f, 0.1f, 100.0f, "%.3f");
		
		//if (ImGui::Button("Build"))
		//{
		//	BuildMesh(model);
		//}

		//if (ImGui::Button("Save"))
		//{
		//	SaveNavmesh("navMeshData.bin", navMesh);
		//}

		//if (ImGui::Button("Load"))
		//{
		//	dtFreeNavMesh(navMesh);
		//	navMesh = LoadNavmesh("navMeshData.bin");

		//	// ナビメッシュクエリ割り当て
		//	navMeshQuery = dtAllocNavMeshQuery();
		//	navMeshQuery->init(navMesh, 2048);
		//}
	}
	ImGui::End();
}

void NavMesh::SaveNavmesh(const char* filename, const dtNavMesh* mesh)
{
#if 1
	if (!mesh) return;

	FILE* fp;
	fopen_s(&fp, filename, "wb");

	if (!fp) return;

	// 情報を書き込む
	NavmeshSetHeader header;
	header.magic    = NAVMESHSSET_MAGIC;
	header.version  = NAVMESHSSET_VERTION;
	header.numTiles = 0;

	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavmeshSetHeader), 1, fp);

	// タイル情報を書き込む
	NavMeshTileHeader tileHeader;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}

	fclose(fp);
#endif
}

dtNavMesh* NavMesh::LoadNavmesh(const char* filename)
{
	FILE* fp;
	fopen_s(&fp, filename, "rb");
	if (!fp) return 0;

	// ヘッダーの読み込み
	NavmeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavmeshSetHeader), 1, fp);

	// 情報がなかった場合終了
	if (readLen != 1)
	{
		fclose(fp);
		return 0;
	}
	if (header.magic != NAVMESHSSET_MAGIC)
	{
		fclose(fp);
		return 0;
	}
	if (header.version != NAVMESHSSET_VERTION)
	{
		fclose(fp);
		return 0;
	}

	// ナビメッシュの割り当て
	dtNavMesh* mesh = dtAllocNavMesh();
	_ASSERT_EXPR_A(mesh, "Could not create Detour navmesh");

	// ナビメッシュ初期化
	dtStatus status = mesh->init(&header.params);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh");

	// タイルメッシュ読み込み
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);

		// データなかったら終了
		if (readLen != 1)
		{
			fclose(fp);
			return 0;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return 0;
		}

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	return mesh;
}
