#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "ModelResource.h"
#include "Transform/Transform.h"

// ���f��
class Model
{
public:
	struct Node
	{
		const char*			name;
		Node*				parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	localTransform;
		DirectX::XMFLOAT4X4	worldTransform;
		DirectX::XMFLOAT4X4 globalTransform;

		std::vector<Node*>	children;
	};

	int	rootMotionNodeIndex = -1;
public:
	Model(const char* filename);
	Model(std::shared_ptr<ModelResource> resource);
	~Model() {}

	// ���f���̃��[�h
	void LoadModel(std::shared_ptr<ModelResource> resource);
	void LoadAnimation(const char* filename);

	// �s��v�Z
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// �Ȃ񂿂����LookAtIK�̐ݒ�
	void SetLookAtIK(const char* name);

	// LookAtIK
	void LookAtTarget(const DirectX::XMFLOAT4X4& worldTransform, const DirectX::XMFLOAT3& target);

	void SetupRootMotion(std::string name);

#pragma region �Q�b�^�[
public:
	// �m�[�h���X�g�擾
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }

	void SetNodeTransforms(size_t nodeIndex, 
		const DirectX::XMVECTOR& S = DirectX::g_XMOne3,
		const DirectX::XMVECTOR& R = DirectX::g_XMIdentityR3,
		const DirectX::XMVECTOR& T = DirectX::g_XMZero)
	{
		DirectX::XMStoreFloat3(&nodes[nodeIndex].scale,		S);
		DirectX::XMStoreFloat4(&nodes[nodeIndex].rotate,	R);
		DirectX::XMStoreFloat3(&nodes[nodeIndex].translate, T);
	}

	void SetNodeTranslate(size_t nodeIndex, const DirectX::XMVECTOR& T = DirectX::g_XMZero)
	{
		DirectX::XMStoreFloat3(&nodes[nodeIndex].translate, T);
	}

	// �m�[�h����
	Node* FindNode(const char* name);

	//	�m�[�h�ԍ�����
	int FindNodeIndex(std::string name) const;

	// ���\�[�X�擾
	const ModelResource* GetResource() const { return resource.get(); }

	// �m�[�h�̈ʒu�擾
	const DirectX::XMFLOAT3& GetNodePos(int index);
	const DirectX::XMFLOAT3& GetNodePos(const char* nodeName);

	Node& GetAimNode() { return *aimNode; }

#pragma endregion

private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;

#pragma region LookAtIK
private:
	Node* aimNode;
#pragma endregion
};
