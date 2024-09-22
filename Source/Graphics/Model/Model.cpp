#include "Graphics/Graphics.h"
#include "Model.h"
#include "System/Math/Mathf.h"

#include "ResourceManager.h"

#include <algorithm>
#include <string>
#include <deque>

// �R���X�g���N�^
Model::Model(const char* filename)
{
	resource = ResourceManager::Instance().LoadModelResource(filename);
	
	// ���f���̃��[�h
	LoadModel(resource);
}

// �R���X�g���N�^
Model::Model(std::shared_ptr<ModelResource> resource) : resource(resource)
{
	// ���f���̃��[�h
	LoadModel(resource);
}

void Model::LoadModel(std::shared_ptr<ModelResource> resource)
{
	const auto& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name      = src.name.c_str();
		dst.parent    = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale     = src.scale;
		dst.rotate    = src.rotate;
		dst.translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// �s��v�Z
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0,	0, 1, 0, 0,	0, 0, 1, 0,	0, 0, 0, 1 };
	UpdateTransform(transform);
}

void Model::LoadAnimation(const char* filename)
{
	resource->LoadAnimation(filename);
}

// �ϊ��s��v�Z
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		// ���[�J���s��Z�o
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ���[���h�s��Z�o
		DirectX::XMMATRIX ParentTransform;
		if (node.parent)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->globalTransform);
		}
		else
		{
			ParentTransform = DirectX::XMMatrixIdentity();
		}
		DirectX::XMMATRIX GlobalTransform = LocalTransform  * ParentTransform;
		DirectX::XMMATRIX WorldTransform  = GlobalTransform * Transform;

		// �v�Z���ʂ��i�[
		DirectX::XMStoreFloat4x4(&node.localTransform,  LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform,  WorldTransform);
		DirectX::XMStoreFloat4x4(&node.globalTransform, GlobalTransform);
	}
}


// �Ȃ񂿂����LookAtIK�̐ݒ�
void Model::SetLookAtIK(const char* name)
{
	aimNode = FindNode(name);
}

// LookAtIK
void Model::LookAtTarget(const DirectX::XMFLOAT4X4& worldTransform, const DirectX::XMFLOAT3& target)
{
	DirectX::XMMATRIX WT     = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&worldTransform));
	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);

	// ��������
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, DirectX::XMVector3TransformCoord(Target, WT));

	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dir));

	//���������A���������r���[�s����쐬
	DirectX::XMVECTOR Up   = { 0,1,0 };
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH({ 0,0,0 }, Direction, Up);

	//�r���[���t�s�񉻂��A���[���h���W�ɖ߂�
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);

	//�s�񂩂�N�H�[�^�j�I���ɕϊ�
	DirectX::XMStoreFloat4(&aimNode->rotate, DirectX::XMQuaternionRotationMatrix(World));
}

const DirectX::XMFLOAT3& Model::GetNodePos(int index)
{
	const Model::Node& node = GetNodes().at(index);

	// ���[���h���W����ʒu���擾
	DirectX::XMFLOAT3 position = GameMath::GetPositionFrom4x4(node.worldTransform);

	return position;
}

const DirectX::XMFLOAT3& Model::GetNodePos(const char* nodeName)
{
	const int nodeIndex = FindNodeIndex(nodeName);

	return GetNodePos(nodeIndex);
}

// �m�[�h����
Model::Node* Model::FindNode(const char* name)
{	
#if 1
	std::string findName = name;

	// �S�Ẵm�[�h�𑍓�����Ŗ��O��r����
	for (Node& node : nodes)
	{
		if (findName == node.name) return &node;
	}

	return nullptr;
#endif
}

// �m�[�h�ԍ�����
int Model::FindNodeIndex(std::string name) const
{
	size_t nodeCount = nodes.size();
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		const Node& node = nodes.at(nodeIndex);
		if (name != node.name) continue;
		
		return static_cast<int>(nodeIndex);
	}

	return -1;
}

void Model::SetupRootMotion(std::string name)
{
	rootMotionNodeIndex = FindNodeIndex(name);
}

