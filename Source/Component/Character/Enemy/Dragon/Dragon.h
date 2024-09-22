#pragma once
#include "Component/Character/Enemy/Enemy.h"

#include "BehaviorTree/DragonBehaviorTree.h"

#include "Graphics/Particle/BreathParticle.h"

class Dragon : public Enemy
{
public:
	// アニメーション
	enum class EnemyAnimation
	{
		IdleBattle,
		BasicAttack,
		ClawAttack,
		HornAttack,
		Defend,
		GetHit,
		Scream,
		Sleep,
		Jump,
		Walk,
		Run,
		IdleNormal,
		Die,
		Walk_Right,
		Walk_Back = 14,
		BigScream = 16
	};

	enum Debug
	{
		PlayBehabior,
		HitCapsule,
		Penetration,
		AimIK,
		Max
	};

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	const char* GetName() const override { return "Dragon"; }
	
	// デバッグ情報表示
	void DebugCondition();
	void OnDebugGUI() override;
	void OnDebugPrimitive() override;

	// ノードとプレイヤーの衝突処理
	void CollisionNodeVSPlayer(const unsigned int nodeIndex);
	void CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position);
	void ProjectileVSPlayer();
		
	// 岩飛ばし
	void RockShot(std::shared_ptr<ModelResource> resource, const DirectX::XMFLOAT3& position, const int rockCount);
	bool stopRockShot = false;

	// 状態設定
	void SetCondition() override;

	void SetAnimationSpeed(float normal, float angry);

	void SetLookFlg(bool flg) { lookFlg = flg; }

protected:
	// 死亡したときに呼ばれる
	//void OnLanding() override;

	// 死亡したときに呼ばれる
	void OnDead() override;

	// ダメージを受けたとき
	void OnDamaged() override;

private:
#pragma region ビヘイビアベースAI
	std::unique_ptr<DragonBehaviorTree> dragonTree;
#pragma endregion

#pragma region パーティクル
public:
	std::unique_ptr<BreathParticle> breath   = nullptr;
	std::unique_ptr<BreathParticle> eruption = nullptr;
#pragma endregion

#pragma region ノード位置取得
	UINT headIndex   = -1;
	UINT handIndex   = -1;
	UINT tongueIndex = -1;
#pragma endregion

	// デバッグ
	bool debugFlg[Debug::Max] = {true, false, false, false};

private:
	// 部位破壊
	struct CBClack
	{
		UINT  clackFlg;
		float dummy[3];
	};
	CBClack clack;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cbClack;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> clackTexture;

private:
	float underTime      = 0.0f;
	float underRotate    = 0.0f;
	float maxUnderRotate = 0.5f;
	DirectX::XMFLOAT3 lookPosition{};
	bool lookFlg = false;
};