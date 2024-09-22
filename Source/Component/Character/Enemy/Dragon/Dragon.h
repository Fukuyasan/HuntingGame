#pragma once
#include "Component/Character/Enemy/Enemy.h"

#include "BehaviorTree/DragonBehaviorTree.h"

#include "Graphics/Particle/BreathParticle.h"

class Dragon : public Enemy
{
public:
	// �A�j���[�V����
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
	
	// �f�o�b�O���\��
	void DebugCondition();
	void OnDebugGUI() override;
	void OnDebugPrimitive() override;

	// �m�[�h�ƃv���C���[�̏Փˏ���
	void CollisionNodeVSPlayer(const unsigned int nodeIndex);
	void CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position);
	void ProjectileVSPlayer();
		
	// ���΂�
	void RockShot(std::shared_ptr<ModelResource> resource, const DirectX::XMFLOAT3& position, const int rockCount);
	bool stopRockShot = false;

	// ��Ԑݒ�
	void SetCondition() override;

	void SetAnimationSpeed(float normal, float angry);

	void SetLookFlg(bool flg) { lookFlg = flg; }

protected:
	// ���S�����Ƃ��ɌĂ΂��
	//void OnLanding() override;

	// ���S�����Ƃ��ɌĂ΂��
	void OnDead() override;

	// �_���[�W���󂯂��Ƃ�
	void OnDamaged() override;

private:
#pragma region �r�w�C�r�A�x�[�XAI
	std::unique_ptr<DragonBehaviorTree> dragonTree;
#pragma endregion

#pragma region �p�[�e�B�N��
public:
	std::unique_ptr<BreathParticle> breath   = nullptr;
	std::unique_ptr<BreathParticle> eruption = nullptr;
#pragma endregion

#pragma region �m�[�h�ʒu�擾
	UINT headIndex   = -1;
	UINT handIndex   = -1;
	UINT tongueIndex = -1;
#pragma endregion

	// �f�o�b�O
	bool debugFlg[Debug::Max] = {true, false, false, false};

private:
	// ���ʔj��
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