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

// �N���[������
void NavMesh::Clean()
{
	// �e�I�u�W�F�N�g�����

	// �n�C�g�t�B�[���h
	rcFreeHeightField(heightField);

	// �ÓI�n�C�g�t�B�[���h
	rcFreeCompactHeightfield(compactHeightField);

	rcFreeContourSet(contourSet);

	// �|���S�����b�V��
	rcFreePolyMesh(polyMesh);

	// �ڍ׃|���S�����b�V��
	rcFreePolyMeshDetail(polyMeshDetail);

	// �i�r���b�V��
	dtFreeNavMesh(navMesh);

	// �i�r���b�V���N�G��
	dtFreeNavMeshQuery(navMeshQuery);
}

#pragma region ���f�����璸�_�𒊏o

void NavMesh::DetectedModel(Model* model)
{
	const ModelResource* resource = model->GetResource();
	// ���b�V���̐����[�v
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// �m�[�h�擾
		const auto& nodes = model->GetNodes();
		const Model::Node& node = nodes.at(mesh.nodeIndex);

		// ���_���W�f�[�^���擾
		std::vector<DirectX::XMFLOAT3> positions;

		// ���_�f�[�^�o��
		if (mesh.nodeIndices.size() > 0)
		{
			size_t offsetTransformsCount = mesh.offsetTransforms.size();

			// �{�[���ϊ��s��Z�o
			std::vector<DirectX::XMMATRIX> boneTransforms;
			boneTransforms.resize(offsetTransformsCount);

			for (size_t i = 0; i < offsetTransformsCount; ++i)
			{
				DirectX::XMMATRIX O = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);

				// ���[���h�s����擾
				boneTransforms.at(i) = DirectX::XMMatrixMultiply(O, W);
			}

			// ���[���h��Ԓ��_���W�Z�o
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
			// ���[���h��Ԓ��_���W�Z�o
			DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).worldTransform);

			for (const ModelResource::Vertex& vertex : mesh.vertices)
			{
				DirectX::XMVECTOR V = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.position), W);
				DirectX::XMFLOAT3 v;
				DirectX::XMStoreFloat3(&v, V);
				positions.emplace_back(v);
			}
		}

		// �g���C�A���O�����X�g�œK���i�d�����钸�_���폜�j
		std::map<DirectX::XMFLOAT3, int> caches;
		size_t meshCount = mesh.indices.size();
		for (size_t i = 0; i < meshCount; ++i)
		{
			UINT index = mesh.indices.at(i);

			UINT vertexCount = static_cast<UINT>(vertices.size());
			const DirectX::XMFLOAT3& position = positions.at(index);

			auto it = caches.find(position);

			// �����ʒu�ɕʂ̒��_������ꍇ
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

	// �o�E���f�B���O�{�b�N�X�v�Z
	rcCalcBounds(&vertices.at(0).x, static_cast<int>(vertices.size()), &boundMin.x, &boundMax.x);
}
#pragma endregion

// �f�o�b�O
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

		//	// �i�r���b�V���N�G�����蓖��
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

	// ������������
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

	// �^�C��������������
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

	// �w�b�_�[�̓ǂݍ���
	NavmeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavmeshSetHeader), 1, fp);

	// ��񂪂Ȃ������ꍇ�I��
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

	// �i�r���b�V���̊��蓖��
	dtNavMesh* mesh = dtAllocNavMesh();
	_ASSERT_EXPR_A(mesh, "Could not create Detour navmesh");

	// �i�r���b�V��������
	dtStatus status = mesh->init(&header.params);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh");

	// �^�C�����b�V���ǂݍ���
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);

		// �f�[�^�Ȃ�������I��
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
