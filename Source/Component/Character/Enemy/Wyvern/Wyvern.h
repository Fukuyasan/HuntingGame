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

	// �X�V����
	void Update(const float& elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// �f�o�b�O�`��
	void DrawDebugPrimitive() override;
	void DrawDebugGUI() override;
	void DebugBehaviorTree();
	void DebugCondition();

	// �m�[�h�ƃv���C���[�̏Փˏ���
	void CollisionNodeVSPlayer(const char* nodeName, float boneRadius);
	void CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position, float boneRadius);
	//void ProjectileVSPlayer();

	// ��Ԑݒ�
	void SetCondition() override;

protected:
	// ���S�����Ƃ��ɌĂ΂��
	void OnLanding() override {}

	// ���S�����Ƃ��ɌĂ΂��
	void OnDead() override;

	// �_���[�W���󂯂��Ƃ�
	void OnDamaged() override {}

public:
	// �A�j���[�V����
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
#pragma region �r�w�C�r�A�x�[�XAI
	std::unique_ptr<WyvernBehaviorTree> wyvernTree;
#pragma endregion

#pragma region �p�[�e�B�N��
public:
	std::unique_ptr<BreathParticle> breath   = nullptr;
	std::unique_ptr<BreathParticle> eruption = nullptr;
#pragma endregion

public:
#pragma region �m�[�h�ʒu�擾
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

	// �f�o�b�O
	bool debugFlg[Debug::Max] = { true, false, false, false };

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

};

#endif