#include "WyvernAction.h"

#include "System/Math/Mathf.h"

#include "Graphics/PostEffect.h"
#include "Graphics/Model/ResourceManager.h"

#include "Camera/CameraController.h"
#include "Manager/GameManager.h"

#include "Component/Character/Player/player.h"

#ifdef Wyvern

bool A_WyvernClaw::Start()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::TailAttack), false);

	// �U���͈͐ݒ�
	owner->SetAttackRadius(1.5f);
	owner->SetAttackPower(15.0f * owner->GetMagnification());

	return true;
}

ActionState A_WyvernClaw::Run(const float& elapsedTime)
{
	// ���s����
	const float animationTime = owner->animator->GetAnimationSeconds();

	// �U���������������
	if (owner->SwitchEventData("Fetch"))
	{
		owner->FaceTarget(elapsedTime, Player::Instance().transform.GetPosition(), DirectX::XMConvertToRadians(180), 0.8f);
	}
	// �Փ˔��菈��������
	if (owner->SwitchEventData("Collision"))
	{
		//owner->CollisionNodeVSPlayer(owner->headIndex);
	}
	// �G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play();
	}


	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_WyvernHorn::Start()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::HornAttack), false);

	return true;
}

ActionState A_WyvernHorn::Run(const float& elapsedTime)
{
	// �U���������������
	if (owner->SwitchEventData("Fetch"))
	{
		owner->FaceTarget(elapsedTime, Player::Instance().transform.GetPosition(), DirectX::XMConvertToRadians(180), 0.8f);
	}
	if (owner->SwitchEventData("Collision"))
	{

	}
	if (owner->SwitchEventData("Effect"))
	{

	}

	// ������Ԃ�
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_WyvernBite::Start()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::BiteAttack), false);

	return true;
}

ActionState A_WyvernBite::Run(const float& elapsedTime)
{
	// �U���������������
	if (owner->SwitchEventData("Fetch"))
	{
		owner->FaceTarget(elapsedTime, Player::Instance().transform.GetPosition(), DirectX::XMConvertToRadians(180), 0.8f);
	}
	if (owner->SwitchEventData("Collision"))
	{

	}

	// ������Ԃ�
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}


bool A_WyvernRoar::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Roar), false);

	// �J�����̋�����ݒ�
	if (owner->GetCondition() & Enemy::Condition::C_Angry)
	{
		CameraController::Instance().SetRangeMode(RangeMode::Range_Far, 4.0f);

		// �{����K�͋��܂Ȃ��悤��
		owner->SetNotFlinch(true);
	}

	owner->SetAttackPower(roarPower);
	owner->SetAttackRadius(20.0f);

	return true;
}

ActionState A_WyvernRoar::Run(const float& elapsedTime)
{
	// �|�X�g�G�t�F�N�g�̐ݒ�
	// �u���[�̈ʒu�ݒ�
	const DirectX::XMFLOAT3& headPos = owner->GetModel()->GetNodePos(owner->headIndex);
	PostEffect::Instance().SetBlurPos(headPos);

	// �p�[�e�B�N���A�G�t�F�N�g�̏o��
	if (owner->SwitchEventData("Effect"))
	{
		owner->bossSEs[Wyvern::SE::Roar]->Play(false);

		owner->smokeEffect->Play(owner->transform.GetPosition());

		// �{���Ԏ��̂݃p�[�e�B�N��
		if (owner->GetCondition() & Enemy::Condition::C_Angry)
		{
			//owner->breath->Initialize(owner->transform.GetPosition(), 0.1f);
			//owner->eruption->Initialize(owner->transform.GetPosition(), 0.1f);
		}
	}
	if (owner->SwitchEventData("Collision") && roarPower != 0.0f)
	{
		// �����蔻��
		//owner->CollisionNodeVSPlayer(headPos);
	}
	if (owner->SwitchEventData("Roar"))
	{
		// �u���[�ݒ�
		PostEffect::Instance().SetBlurParam(30.0f, 1.0f);

		// �J�����V�F�C�N
		CameraController::Instance().SetShakeMode(1.0f, 2.5f, 5.0f);

		if (owner->GetCondition() & Enemy::Condition::C_Angry)
		{
			lerpTime += elapsedTime;
			lerpTime = (std::min)(lerpTime, 1.0f);

			// �ʏ�[���g�F�ɕύX
			PostEffect::Instance().LerpColorBalance(normalColor, angryColor, lerpTime);
		}
	}

	if (!owner->animator->IsPlayAnimation())
	{
		normalColor = angryColor;

		owner->MakeCondition(Enemy::Condition::C_Battle);

		owner->SetNotFlinch(false);
		return ActionState::Complete;
	}

	return ActionState::Run;
}


#pragma region �v���C���[�Ɛ���ĂȂ����̍s��
#pragma region �p�j����
bool A_WyvernWander::Start()
{
	// �p�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Walk), true);

	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_WyvernWander::Run(const float& elapsedTime)
{
	// �ړI�n�_�܂ł�XZ���ʂł̋�������
	const DirectX::XMFLOAT3& position = owner->transform.GetPosition();
	const DirectX::XMFLOAT3& targetPosition = owner->GetTargetPosition();

	// �ړI�n�_�ֈړ�
	owner->FaceTarget(elapsedTime, targetPosition, DirectX::XMConvertToRadians(180), 0.8f);

	if (owner->SwitchEventData("EffectLeft"))
	{
		owner->bossSEs[Wyvern::SE::FootStep]->Play(false);
		owner->smokeEffect->Play(owner->transform.GetPosition());
	}
	if (owner->SwitchEventData("EffectRight"))
	{
		owner->bossSEs[Wyvern::SE::FootStep]->Play(false);
		owner->smokeEffect->Play(owner->transform.GetPosition());
	}

	// �ړI�n�֒����� || �v���C���[����
	const float range  = owner->GetRadius();
	const float distSq = GameMath::LengthSqFromXZ(position, targetPosition);

	bool playerFind = owner->GetCondition() & Wyvern::Condition::C_Find;

	// ���s��
	return (distSq < range * range || playerFind) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region �ҋ@�s��
bool A_WyvernIdle::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Idle), true);

	owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
	
	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_WyvernIdle::Run(const float& elapsedTime)
{
	// �^�C�}�[�X�V
	owner->SubRunTimer(elapsedTime);
	
	// �v���C���[���G����
	bool find = owner->GetCondition() & Wyvern::Condition::C_Find;

	// �ҋ@���Ԃ��߂�����	|| ������
	return (owner->GetRunTimer() < 0.0f || find) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region �U������������Ƃ��̍s��
#pragma region ���ݍs��
bool A_WyvernFlinch::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::GetHit), false);
	owner->animator->SetAnimSpeed(1.0f);

	owner->bossSEs[Wyvern::SE::Voice]->Play(false);

	return true;
}
ActionState A_WyvernFlinch::Run(const float& elapsedTime)
{
	if (!owner->animator->IsPlayAnimation())
	{
		// ���݃��Z�b�g
		owner->DropCondition(Wyvern::Condition::C_Flinch);

		return ActionState::Complete;
	}

	return ActionState::Run;
}
#pragma endregion

#pragma region ���S�s��
bool A_WyvernDie::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Die), false);

	// �J�������o
	CameraController::Instance().SetClearPerform(6.0f);
	CameraController::Instance().SetOwner(owner);
	CameraController::Instance().SetRangeMode(RangeMode::Range_Perform);

	return true;
}
ActionState A_WyvernDie::Run(const float& elapsedTime)
{
	// �����̃G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		owner->bossSEs[Wyvern::SE::Land]->Play(false);
		owner->smokeEffect->Play(owner->transform.GetPosition());
	}
	if (owner->SwitchEventData("Sound"))
	{
		owner->bossSEs[Wyvern::SE::Die]->Play(false);
	}

	// �����X�^�[�����S�������̃J��������
	if (CameraController::Instance().ClearPerform(elapsedTime))
	{
		// �|�X�g�G�t�F�N�g
		lerpRate += elapsedTime * 0.3f;

		if (lerpRate >= 1.0f)
		{
			//// �J���������Ƃɖ߂�
			//CameraController::Instance().Set_owner(&Player::Instance());
			//CameraController::Instance().SetRangeMode(RangeMode::Range_First, 0.0f);

			GameManager::Instance().ReturnTitle();
		}

		// �J���[�o�����X
		PostEffect::Instance().LerpColorBalance(angryColor, normalColor, lerpRate);
	}

	return ActionState::Run;
}
#pragma endregion
#pragma endregion

#endif