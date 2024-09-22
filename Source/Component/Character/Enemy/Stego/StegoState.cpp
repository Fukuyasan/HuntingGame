#include "StegoState.h"
#include "AI/MetaAI/MetaAI.h"
#include "Component/Character/Player/player.h"

#include "Camera/Camera.h"
#include "Camera/CameraController.h"

#ifdef Stego

// ************************************************************
//		�ҋ@�X�e�[�g
// ************************************************************

void StegoIdleState::Enter()
{
	owner->animator->PlayAnimation(static_cast<int>(Stego::Animation::A_Idle), true);
}

void StegoIdleState::Execute(const float& elapsedTime)
{
	// �G���ړ�������	
	if (owner->GetCondition() & Stego::Condition::C_AreaMove)
	{
		owner->ChangeState(Stego::State::S_Run);
	}
}

// ************************************************************
//		����X�e�[�g
// ************************************************************

void StegoRunState::Enter()
{
	owner->animator->PlayAnimation(static_cast<int>(Stego::Animation::A_Run), true);
	owner->animator->SetAnimSpeed(3.0f);

	// �ړ��ꏊ�Z�b�g3
	int num = owner->GetAreaNum();
	num = ++num % 5;
	owner->SetAreaNum(num);
	
	// �G���A�ړ�
	areaPos = MetaAI::Instance().GetAreaData(num).areaPos;
	owner->GetAgent()->StartMove(owner->GetAgentID(), owner->transform.GetPosition(), areaPos);
}

void StegoRunState::Execute(const float& elapsedTime)
{
	owner->UpdateCoolTimer(elapsedTime);

	//owner->GetAgent()->Target(owner->GetAgentID(), Player::Instance().transform.GetPosition());

	// �G���ړ�������
	dtCrowd* crowd = owner->GetAgent()->GetCrowd();
	owner->transform.SetPositionX(crowd->getAgent(owner->GetAgentID())->npos[0]);
	owner->transform.SetPositionY(crowd->getAgent(owner->GetAgentID())->npos[1]);
	owner->transform.SetPositionZ(crowd->getAgent(owner->GetAgentID())->npos[2]);

	DirectX::XMFLOAT3 nextPos{};
	nextPos.x = owner->transform.GetPosition().x + crowd->getAgent(owner->GetAgentID())->vel[0];
	nextPos.z = owner->transform.GetPosition().z + crowd->getAgent(owner->GetAgentID())->vel[2];

	// �G�͈ړ���������Ɍ���
	owner->FaceTarget(elapsedTime, nextPos, DirectX::XMConvertToRadians(1080), 1.0f);

	// �ړI�n�ɂ�������s����
	const float lengthSq = GameMath::LengthSqFromXZ(owner->transform.GetPosition(), areaPos);
	const float radius   = 20.0f;
	if (lengthSq < radius * radius)
	{
		// �o�H�T�������Z�b�g
		crowd->resetMoveTarget(owner->GetAgentID());

		// �ҋ@�X�e�[�g�ɑJ��
		owner->ChangeState(Stego::State::S_Idle);
	}

	// �G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{		
		owner->smokeEffect->Play(owner->transform.GetPosition());
	}
}


// ************************************************************
//		���S�X�e�[�g
// ************************************************************

void StegoDeathState::Enter()
{
	owner->animator->PlayAnimation(static_cast<int>(Stego::Animation::A_Death), false);
}

void StegoDeathState::Execute(const float& elapsedTime)
{
	
}

#endif