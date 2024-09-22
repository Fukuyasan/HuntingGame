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

// �r���h����
void NavMeshObstacles::Build(Model* model)
{
	// ���f�����璸�_�𒊏o
	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<int> indices;
	// �o�E���f�B���O�{�b�N�X	
	DirectX::XMFLOAT3 boundMin, boundMax;

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// �m�[�h�擾
		const auto& nodes = model->GetNodes();
		const Model::Node& node = nodes.at(mesh.nodeIndex);

		// ���[���h��Ԓ��_���W�Z�o
		DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).worldTransform);

		// �o�E���f�B���O�{�b�N�X�̂��߂̒��_�Z�o
		DirectX::XMVECTOR V = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh.boundsMin), W);
		DirectX::XMStoreFloat3(&boundMin, V);

		V = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh.boundsMax), W);
		DirectX::XMStoreFloat3(&boundMax, V);

	}	
	
	// �p�����[�^�ݒ�
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

	// ��Q���̊��蓖��
	tileCache = dtAllocTileCache();

	// ��Q��������
	dtStatus status;
	status = tileCache->init(&params, nullptr, nullptr, nullptr);
	_ASSERT_EXPR_A(status, "buildNavigation: Out of memory 'pmdtl'.");

	status = tileCache->addBoxObstacle(&boundMin.x, &boundMax.x, 0);
}

// �N���[������
void NavMeshObstacles::Clean()
{
	// �I�u�W�F�N�g�̉��
	dtFreeTileCache(tileCache);
	tileCache = nullptr;
}

// �X�V����
void NavMeshObstacles::Update(const float& elapsedTime,dtNavMesh* navMesh)
{
	tileCache->update(elapsedTime, navMesh);
}
