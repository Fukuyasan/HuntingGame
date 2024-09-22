#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

#include "Transform/Transform.h"

#include "Component/Component.h"

// �A�j���[�V����
class Animator : public Component
{
public:
	Animator() = default;
	~Animator() override = default;

	void Start() override;
	const char* GetName() const override { return "Animator"; }

	// �A�j���[�V�����X�V����
	void UpdateAnimation(const float& elapsedTime);

	// �A�j���[�V�����Đ�
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.25f);
	void SetAnimSpeed(float speed) { animationSpeed = speed; }

	// �A�j���[�V�����ݒ�
	void SetAnimation(int index, bool loop);
	// �u�����h�ݒ�
	void SetBlending(float seconds);
	// ���[�g���[�V�����ݒ�
	void SetRootMotion();

	// �A�j���[�V�����Đ�����
	bool IsPlayAnimation() const;

	// ���[�g���[�V����
	void UpdateRootMotion(Transform& transform);

#pragma region �v�Z����
private:
	// �A�j���[�V�����œ����m�[�h�̌v�Z����
	void ComputeAnimation(const float& elapsedTime);
	// �u�����h���[�V�����v�Z����
	void ComputeBlending(const float& elapsedTime);
	// ���[�g���[�V�����v�Z����
	void ComputeRootMotion(const float& elapsedTime);
#pragma endregion

public:
	const int   GetAnimationIndex()	  const { return currentAnimationIndex; }
	// ���݂̃A�j���[�V�����Đ����Ԏ擾
	const float GetAnimationSeconds() const { return currentAnimationSeconds; }
	const float GetAnimationLength()  const { return animationLength; }

private:
	int currentAnimationIndex     = -1;
	float currentAnimationSeconds = 0.0f;
	float oldAnimationSeconds     = 0.0f;
	float animationLength         = 0.0f;
	float animationSpeed          = 1.0f;
	bool animationLoopFlag        = false;
	bool animationEndFlag         = false;

#pragma region �A�j���[�V�����u�����h
private:
	struct NodeCache
	{
		DirectX::XMFLOAT3 scale     = { 1,1,1 };
		DirectX::XMFLOAT4 rotate    = { 0,0,0,1 };
		DirectX::XMFLOAT3 translate = { 0,0,0 };
	};
	std::vector<NodeCache> nodeCaches;

	float blendTime          = 0.0f;
	float blendSeconds       = 0.0f;
	float blendSecondsLength = -1.0f;
	bool  blendFirstFlag     = false;
	bool  blendFlag          = false;

#pragma endregion
#pragma region ���[�g���[�V����
private:
	DirectX::XMFLOAT3 beginRootMotionTranslation = { 0,0,0 };
	DirectX::XMFLOAT3 cacheRootMotionTranslation = { 0,0,0 };
	DirectX::XMFLOAT3 rootMotionTranslation      = { 0,0,0 };
	bool			  rootMotionFlag             = false;
#pragma endregion
};