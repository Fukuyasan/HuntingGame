#pragma once
#include "Component/Character/Enemy/Enemy.h"

#include "Component/Character/Enemy/Wyvern/BehaviorTree/WyvernBehaviorTree.h"

#include "Graphics/Particle/BreathParticle.h"

#ifdef Wyvern

class Wyvern : public Enemy
{
public:
	Wyvern();
	~Wyvern() override;

	// 更新処理
	void Update(const float& elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグ描画
	void DrawDebugPrimitive() override;
	void DrawDebugGUI() override;
	void DebugBehaviorTree();
	void DebugCondition();

	// ノードとプレイヤーの衝突処理
	void CollisionNodeVSPlayer(const char* nodeName, float boneRadius);
	void CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position, float boneRadius);
	//void ProjectileVSPlayer();

	// 状態設定
	void SetCondition() override;

protected:
	// 死亡したときに呼ばれる
	void OnLanding() override {}

	// 死亡したときに呼ばれる
	void OnDead() override;

	// ダメージを受けたとき
	void OnDamaged() override {}

public:
	// アニメーション
	enum class EnemyAnimation
	{
		BiteAttack,
		ClawAttack_L,
		ClawAttack_R,
		HornAttack,
		Idle,
		Roar,
		Run,
		TailAttack,
		Aburrido,
		HornAttack_Smale,
		Walk,
		Fire,
		Fly,
		Land,
		Rush,
		GetHit,
		Die
	};

private:
#pragma region ビヘイビアベースAI
	std::unique_ptr<WyvernBehaviorTree> wyvernTree;
#pragma endregion

#pragma region パーティクル
public:
	std::unique_ptr<BreathParticle> breath   = nullptr;
	std::unique_ptr<BreathParticle> eruption = nullptr;
#pragma endregion

public:
#pragma region ノード位置取得
	int headIndex  = -1;
	int rHandIndex = -1;
	int lHandIndex = -1;
#pragma endregion

	enum Debug
	{
		PlayBehabior,
		HitCapsule,
		Penetration,
		AimIK,
		Max
	};

	// デバッグ
	bool debugFlg[Debug::Max] = { true, false, false, false };

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

};

#endif