#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

#include "Transform/Transform.h"

#include "Component/Component.h"

// アニメーション
class Animator : public Component
{
public:
	Animator() = default;
	~Animator() override = default;

	void Start() override;
	const char* GetName() const override { return "Animator"; }

	// アニメーション更新処理
	void UpdateAnimation(const float& elapsedTime);

	// アニメーション再生
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.25f);
	void SetAnimSpeed(float speed) { animationSpeed = speed; }

	// アニメーション設定
	void SetAnimation(int index, bool loop);
	// ブレンド設定
	void SetBlending(float seconds);
	// ルートモーション設定
	void SetRootMotion();

	// アニメーション再生中か
	bool IsPlayAnimation() const;

	// ルートモーション
	void UpdateRootMotion(Transform& transform);

#pragma region 計算処理
private:
	// アニメーションで動くノードの計算処理
	void ComputeAnimation(const float& elapsedTime);
	// ブレンドモーション計算処理
	void ComputeBlending(const float& elapsedTime);
	// ルートモーション計算処理
	void ComputeRootMotion(const float& elapsedTime);
#pragma endregion

public:
	const int   GetAnimationIndex()	  const { return currentAnimationIndex; }
	// 現在のアニメーション再生時間取得
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

#pragma region アニメーションブレンド
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
#pragma region ルートモーション
private:
	DirectX::XMFLOAT3 beginRootMotionTranslation = { 0,0,0 };
	DirectX::XMFLOAT3 cacheRootMotionTranslation = { 0,0,0 };
	DirectX::XMFLOAT3 rootMotionTranslation      = { 0,0,0 };
	bool			  rootMotionFlag             = false;
#pragma endregion
};