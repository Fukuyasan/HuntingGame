#include "Animator.h"

void Animator::Start()
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	// �L���b�V���̔z��� resize
	nodeCaches.resize(object->GetModel()->GetResource()->GetNodes().size());
}

void Animator::UpdateAnimation(const float& elapsedTime)
{
	// �A�j���[�V�����v�Z����
	ComputeAnimation(elapsedTime);

	// �u�����h�v�Z����
	ComputeBlending(elapsedTime);

	// ���[�g���[�V�����v�Z����
	ComputeRootMotion(elapsedTime);
}

void Animator::PlayAnimation(int index, bool loop, float blendSeconds)
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	// �A�j���[�V�����ݒ�
	SetAnimation(index, loop);

	// �u�����h�ݒ�
	SetBlending(blendSeconds);

	if (object->GetModel()->rootMotionNodeIndex == -1) return;

	// ���[�g���[�V�����ݒ�
	SetRootMotion();
}

void Animator::SetAnimation(int index, bool loop)
{
	currentAnimationIndex   = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag       = loop;
	animationEndFlag        = false;
}

void Animator::SetBlending(float seconds)
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	blendFlag          = true;
	blendFirstFlag     = true;
	blendSeconds       = 0.0f;
	blendSecondsLength = seconds;

	// ���݂̎p�����L���b�V������
	const auto& nodeData = object->GetModel()->GetNodes();
	for (size_t i = 0; i < nodeData.size(); ++i)
	{
		const Model::Node& src = nodeData[i];
		NodeCache& dst		   = nodeCaches[i];

		dst.scale     = src.scale;
		dst.rotate    = src.rotate;
		dst.translate = src.translate;
	}
}

void Animator::SetRootMotion()
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	rootMotionTranslation      = { 0,0,0 };
	//cacheRootMotionTranslation = { 0,0,0 };

	auto& rootMotionNode = object->GetModel()->GetNodes()[object->GetModel()->rootMotionNodeIndex];
	cacheRootMotionTranslation = rootMotionNode.translate;
}

bool Animator::IsPlayAnimation() const
{
	std::shared_ptr<GameObject> object = gameObject.lock();
	size_t animSize = object->GetModel()->GetResource()->GetAnimations().size();

	if (currentAnimationIndex < 0) return false;
	if (currentAnimationIndex >= animSize) return false;

	return true;
}

void Animator::UpdateRootMotion(Transform& transform)
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	if (!rootMotionFlag) return;
	if (object->GetModel()->rootMotionNodeIndex < 0) return;

	Model::Node& rootMotionNode = object->GetModel()->GetNodes()[object->GetModel()->rootMotionNodeIndex];

	// �ϊ������O���[�o���ړ������l��Y�������O�ɂ���B
	rootMotionTranslation.y = 0;

	// ����̃��[�g���[�V�����m�[�h�̃O���[�o���ʒu���Z�o����B
	DirectX::XMMATRIX ParentGlobalTransform = DirectX::XMLoadFloat4x4(&rootMotionNode.parent->globalTransform);
	DirectX::XMVECTOR RootMotionTranslate = DirectX::XMLoadFloat3(&rootMotionNode.translate);
	DirectX::XMVECTOR RootGlobalTransform = DirectX::XMVector3TransformNormal(RootMotionTranslate, ParentGlobalTransform);

	// �Z�o�����O���[�o���ʒu��XZ�������O�ɂ���B
	DirectX::XMFLOAT3 globalTransform;
	DirectX::XMStoreFloat3(&globalTransform, RootGlobalTransform);

	globalTransform.x = 0;
	globalTransform.z = 0;

	RootGlobalTransform = DirectX::XMLoadFloat3(&globalTransform);

	// �O���[�o���ʒu�����[�J���ʒu�ɕϊ�����B
	DirectX::XMMATRIX ParentLocalTransform = DirectX::XMMatrixInverse(nullptr, ParentGlobalTransform);
	DirectX::XMVECTOR LocalTransform = DirectX::XMVector3TransformCoord(RootGlobalTransform, ParentLocalTransform);

	// �ϊ��������[�J���ʒu�����[�g���[�V�����m�[�h�ɐݒ肷��B
	rootMotionNode.translate.x = DirectX::XMVectorGetX(LocalTransform);
	rootMotionNode.translate.y = DirectX::XMVectorGetY(LocalTransform);
	rootMotionNode.translate.z = DirectX::XMVectorGetX(LocalTransform);

	// �O���[�o���ړ������l���L�����N�^�[�̃��[���h�s��ɏ�Z����B
	DirectX::XMVECTOR RootMotionTranslation = DirectX::XMLoadFloat3(&rootMotionTranslation);
	DirectX::XMVECTOR GlobalTransform = DirectX::XMVector3TransformNormal(RootMotionTranslation, ParentGlobalTransform);

	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&transform.m_transform);
	DirectX::XMVECTOR WorldTranslation = DirectX::XMVector3TransformNormal(GlobalTransform, WorldTransform);

	transform.m_position.x += DirectX::XMVectorGetX(WorldTranslation);
	transform.m_position.y += DirectX::XMVectorGetY(WorldTranslation);
	transform.m_position.z += DirectX::XMVectorGetZ(WorldTranslation);

	rootMotionTranslation = { 0,0,0 };
}

void Animator::ComputeAnimation(const float& elapsedTime)
{
	// �A�j���[�V�����Đ����łȂ��Ȃ珈�����Ȃ�
	if (!IsPlayAnimation()) return;

	std::shared_ptr<GameObject> object = gameObject.lock();

	// �A�j���[�V���������擾
	const auto& animations                    = object->GetModel()->GetResource()->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);
	const auto& keyframes                     = animation.keyframes;

	// �A�j���[�V�����̒������擾
	animationLength = animation.secondsLength;

	// �ŏI�t���[������
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}

	// ���Ԍo��
	currentAnimationSeconds += elapsedTime * animationSpeed;

	// �Đ����Ԃ��I�[���Ԃ𒴂�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		// �Đ����Ԃ������߂�
		if (animationLoopFlag)
		{
			currentAnimationSeconds -= animation.secondsLength;

			// ���[�g���[�V������߂��Ă���
			if (object->GetModel()->rootMotionNodeIndex != -1)
			{
				cacheRootMotionTranslation = rootMotionTranslation;
			}
		}
		// �Đ��I�����Ԃɂ���
		else
		{
			rootMotionFlag = false;
			animationEndFlag = true;
			currentAnimationSeconds = animation.secondsLength;
		}

		return;
	}

	// �A�j���[�V������񂩂�L�[�t���[�������擾
	size_t keyCount = keyframes.size();
	for (size_t keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		size_t nextKeyIndex = keyIndex + 1;

		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(nextKeyIndex);

		// �L�[�t���[�����łȂ��Ȃ� continue
		if (currentAnimationSeconds < keyframe0.seconds ||
			currentAnimationSeconds > keyframe1.seconds) continue;

		// �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮���Z�o
		float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

		size_t nodeCount = object->GetModel()->GetNodes().size();
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			// 2�̃L�[�t���[���Ԃ̕⊮�v�Z
			const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
			const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

			auto& node = object->GetModel()->GetNodes()[nodeIndex];

			// �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
			DirectX::XMVECTOR S = DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&key0.scale),
				DirectX::XMLoadFloat3(&key1.scale),
				rate);

			DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(
				DirectX::XMLoadFloat4(&key0.rotate),
				DirectX::XMLoadFloat4(&key1.rotate),
				rate);

			DirectX::XMVECTOR T = DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&key0.translate),
				DirectX::XMLoadFloat3(&key1.translate),
				rate);

			object->GetModel()->SetNodeTransforms(nodeIndex, S, R, T);

			// ����̈ʒu���擾
			if (nodeIndex == object->GetModel()->rootMotionNodeIndex && 0 >= keyframe0.seconds && 0 <= keyframe1.seconds)
			{
				DirectX::XMStoreFloat3(&beginRootMotionTranslation, T);
			}
		}
		break;
	}

	rootMotionFlag = true;
}

void Animator::ComputeBlending(const float& elapsedTime)
{
	if (!blendFlag) return;

	std::shared_ptr<GameObject> object = gameObject.lock();

	// ���ԍX�V
	blendSeconds += elapsedTime;

	if (blendSeconds >= blendSecondsLength)
	{
		blendSeconds = blendSecondsLength;
		blendFlag = false;
		return;
	}

	// �u�����h���̌v�Z
	float rate = blendSeconds / blendSecondsLength;

	// �u�����h�v�Z
	size_t count = object->GetModel()->GetNodes().size();
	for (size_t nodeIndex = 0; nodeIndex < count; ++nodeIndex)
	{
		const NodeCache& cache = nodeCaches.at(nodeIndex);
		auto& node = object->GetModel()->GetNodes()[nodeIndex];

		DirectX::XMVECTOR S = DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&cache.scale),
			DirectX::XMLoadFloat3(&node.scale),
			rate);

		DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(
			DirectX::XMLoadFloat4(&cache.rotate),
			DirectX::XMLoadFloat4(&node.rotate),
			rate);

		DirectX::XMVECTOR T = DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&cache.translate),
			DirectX::XMLoadFloat3(&node.translate),
			rate);

		object->GetModel()->SetNodeTransforms(nodeIndex, S, R, T);
	}

	rootMotionFlag = true;
}

void Animator::ComputeRootMotion(const float& elapsedTime)
{
	if (!rootMotionFlag) return;

	std::shared_ptr<GameObject> object = gameObject.lock();
	if (object->GetModel()->rootMotionNodeIndex < 0) return;

	auto& rootMotionNode = object->GetModel()->GetNodes()[object->GetModel()->rootMotionNodeIndex];

	// �O��̃{�[���̈ʒu���獡��̈ʒu�̃{�[���̍����l���擾
	rootMotionTranslation.x = rootMotionNode.translate.x - cacheRootMotionTranslation.x;
	rootMotionTranslation.y = rootMotionNode.translate.y - cacheRootMotionTranslation.y;
	rootMotionTranslation.z = rootMotionNode.translate.z - cacheRootMotionTranslation.z;

	cacheRootMotionTranslation = rootMotionNode.translate;
}
