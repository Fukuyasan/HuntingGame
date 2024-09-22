#pragma once
#include "AI/BehaviorTree/ActionBase.h"
#include "Component/Character/Enemy/Dragon/Dragon.h"

#pragma region �U��
// �p�ōU��
class A_Horn : public ActionBase<Dragon>
{
public:
	A_Horn(Dragon* dragon) :ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

// �܂ōU��
class A_Claw : public ActionBase<Dragon>
{
public:
	A_Claw(Dragon* dragon) :ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
private:
	std::shared_ptr<ModelResource> resource;

	const char* filenames[2];
	int random = -1;
};

// �ʏ�U��(���݂�)
class A_Bite : public ActionBase<Dragon>
{
public:
	A_Bite(Dragon* dragon, const float _animSpeed, bool _fetch) : ActionBase<Dragon>(dragon), animSpeed(_animSpeed), fatchFlg(_fetch){}
	
	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
private:
	float animSpeed = 0.0f;
	bool fatchFlg   = false;
};

// �W�����v
class A_Jump : public ActionBase<Dragon>
{
public:
	A_Jump(Dragon* dragon, float stepPower) : ActionBase<Dragon>(dragon), power(stepPower) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	float power = 0.0f;
};

// �W�����v�U��
class A_JumpAttack : public ActionBase<Dragon>
{
public:
	A_JumpAttack(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

// ������
class A_Back : public ActionBase<Dragon>
{
public:
	A_Back(Dragon* dragon, const float time = 1.0f) : ActionBase<Dragon>(dragon), backTime(time) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	float backTime = 0.0f;
};

// �ːi�s��
class A_Rush : public ActionBase<Dragon>
{
public:
	A_Rush(Dragon* dragon, bool stop = false, const float runTimer = 1.0f, const int turnCount = 0)
		: ActionBase<Dragon>(dragon)
		, stop(stop)
		, maxTurnCount(turnCount)
		, runTimer(runTimer) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	bool  stop       = false;

	// ������񂷂邩
	int   turnCount     = 0;
	int   maxTurnCount  = 0;

	// ���b���邩
	float runTimer   = 0.0f;

	// �G�t�F�N�g���o���Ԋu
	float effectCool = 0.0f;

	float rushSpeed    = 25.0f;
	float maxRushSpeed = 25.0f;
};

// �ǐՍs��
class A_Pursuit : public ActionBase<Dragon>
{
public:
	A_Pursuit(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};
#pragma endregion

#pragma region �p�j�s��
// �p�j�s��
class A_Wander : public ActionBase<Dragon>
{
public:
	A_Wander(Dragon* dragon) :ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

// �ҋ@�s��
class A_Idle : public ActionBase<Dragon>
{
public:
	A_Idle(Dragon* dragon) :ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};
#pragma endregion

#pragma region �_���[�W���󂯂��Ƃ��̍s��
// ���ݍs��
class A_Flinch : public ActionBase<Dragon>
{
public:
	A_Flinch(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

// ���S�s��
class A_Die : public ActionBase<Dragon>
{
public:
	A_Die(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
private:
	DirectX::XMFLOAT4 normalColor = {   1.0f,   1.0f,   1.0f, 1.0f };
	DirectX::XMFLOAT4 angryColor  = { 0.735f, 0.287f, 0.137f, 3.0f };

	float lerpRate = 0.0f;
};
#pragma endregion

// �G���A�ړ�
class A_AreaMove : public ActionBase<Dragon>
{
public:
	A_AreaMove(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	int areaNum = 0;
	DirectX::XMFLOAT3 areaPos = {};
	float areaRange           = 0.0f;
};

// ���K
class A_Roar : public ActionBase<Dragon>
{
public:
	A_Roar(Dragon* dragon, const float power = 0.0f) : ActionBase<Dragon>(dragon),roarPower(power) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	DirectX::XMFLOAT4 normalColor = {   1.0f,   1.0f,   1.0f, 1.0f };
	DirectX::XMFLOAT4 angryColor  = { 0.735f, 0.287f, 0.137f, 3.0f };

	float roarPower = 0.0f;
	float lerpTime  = 0.0f;
};

// �Њd
class A_Intimidate : public ActionBase<Dragon>
{
public:
	A_Intimidate(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

//	�U�����
class A_Turn : public ActionBase<Dragon>
{
public:
	A_Turn(Dragon* dragon, const float _rate = 0.5f) :ActionBase<Dragon>(dragon), rate(_rate) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	float rate = 0.0f;
};

// �Њd
class A_Tired : public ActionBase<Dragon>
{
public:
	A_Tired(Dragon* dragon, const float time) : ActionBase<Dragon>(dragon), tiredTime(time) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	float tiredTime = 0.0f;
};

#pragma region ���[�r�[�s��
class A_MovieClaw : public ActionBase<Dragon>
{
public:
	A_MovieClaw(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

class A_MovieRoar : public ActionBase<Dragon>
{
public:
	A_MovieRoar(Dragon* dragon) : ActionBase<Dragon>(dragon) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};
#pragma endregion