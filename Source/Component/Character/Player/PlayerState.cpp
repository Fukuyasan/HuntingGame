#include "PlayerState.h"
#include "PlayerController.h"
#include "System/Math/Easing.h"

constexpr inline float effectScale = 0.3f;

// ************************************************************
//		�ҋ@�X�e�[�g
// ************************************************************

void IdleState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Idle), true);
	owner->firstAttack = true;
}

void IdleState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	// ����X�e�[�g�ɑJ��
	if (lStickValue != 0)
	{
		owner->ChangeState(Player::State::State_Run);
	}
	// ����X�e�[�g�ɑJ��
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// �J�E���^�[�U��
	if (owner->controller->GetKeyPrecede(Key::KeyX))
	{
		owner->ChangeState(Player::State::State_Counter_Start);
	}
	// ��U��
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_1);
	}
	// ���U��
	if (owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_1);
	}
	// �_�b�V���U��
	if (owner->controller->GetKeyPrecede(Key::KeyYB))
	{
		owner->ChangeState(Player::State::State_Attack_Dash);
	}
}

// ************************************************************
//		�����X�e�[�g
// ************************************************************

void WalkState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Walk), true);
	owner->movement->SetMaxMoveSpeed(3.0f);
	owner->firstAttack = true;
}

void WalkState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	owner->InputMove(elapsedTime);
		
	// ����X�e�[�g
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// ��U��
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_1);
	}
	// ���U��
	if (owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_1);
	}
	// �ҋ@�X�e�[�g
	if (lStickValue == 0.0f)
	{
		owner->ChangeState(Player::State::State_Idle);
	}
	// ����X�e�[�g
	if (lStickValue >= 0.5f)
	{
		owner->ChangeState(Player::State::State_Run);
	}	

	// ���̃G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}
}

void WalkState::Exit()
{
}

// ************************************************************
//		����X�e�[�g
// ************************************************************

void RunState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Jogging), true);
	owner->movement->SetMaxMoveSpeed(5.0f);
	owner->firstAttack = true;
}

void RunState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();
	owner->InputMove(elapsedTime);

	// �����X�e�[�g�ɑJ��
	if (lStickValue < 0.5f)
	{
		owner->ChangeState(Player::State::State_Walk);
	}
	// ����X�e�[�g�ɑJ��
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// ��U��
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_1);
	}
	// ���U��
	if (owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_1);
	}
	// �_�b�V���X�e�[�g�ɑJ��
	if (lStickValue > 0.9f && owner->InputDash())
	{
		owner->ChangeState(Player::State::State_Dash);
	}
	
	// ���̃G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}
}

void RunState::Exit()
{
}

// ************************************************************
//		�_�b�V���X�e�[�g
// ************************************************************

void DashState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Run), true);
	owner->movement->SetMaxMoveSpeed(8.0f);
}

void DashState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();
	//owner->stamina -= owner->consumeDash * elapsedTime;

	owner->InputMove(elapsedTime);

	// ����X�e�[�g�ɑJ��
	if (!owner->InputDash() || lStickValue < 0.5f)
	{
		// �A�ł���Ȃ��悤�ɂ���
		owner->SetCoolTimer(0.5f);
		owner->ChangeState(Player::State::State_Run);
	}
	// ����X�e�[�g�ɑJ��
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// �_�b�V���U���X�e�[�g�ɑJ��
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_Attack_Dash);
	}
	
	// ���̃G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}
}

void DashState::Exit()
{
}

// ************************************************************
//		����X�e�[�g
// ************************************************************

void AvoidState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Douge), false);

	// ����̕�����ݒ�
	owner->SetAvoidVec();
	owner->firstAttack = true;
}

void AvoidState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	// �������
	owner->PlayAvoid(elapsedTime);
	
	// �ҋ@�X�e�[�g�J��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		event�t�@�C������J�ڂ̐؂�ւ�
	// 
	// -------------------------------------------

	// ���r���ňړ����͂�����Α���X�e�[�g�J��
	if (owner->SwitchEventData("Move") && lStickValue != 0.0f)
	{
		owner->ChangeState(Player::State::State_Run);
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_2);
	}
	// ���̃G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}	
}

// ************************************************************
// 
//		��U���X�e�[�g
// 
// ************************************************************

void ComboA_1State::Enter()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboA_1), false);
}

void ComboA_1State::Execute(const float& elapsedTime)
{
	// �A�j���[�V�����I���őҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		event�t�@�C������J�ڂ̐؂�ւ�
	// 
	// -------------------------------------------

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_2);
	}
	if (owner->SwitchEventData("Avoid") && owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->InputTurn();
	}
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack1);
	}
}

void ComboA_1State::Exit()
{
	owner->firstAttack = false;
}

void ComboA_2State::Enter()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboA_2), false);
}

void ComboA_2State::Execute(const float& elapsedTime)
{
	// �ҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		event�t�@�C������J�ڂ̐؂�ւ�
	// 
	// -------------------------------------------

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_3);
	}
	if (owner->SwitchEventData("Avoid") && owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->InputTurn();
	}
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack2);
	}
}

void ComboA_2State::Exit()
{
	owner->firstAttack = false;
}

void ComboA_3State::Enter()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboA_3), false);
}

void ComboA_3State::Execute(const float& elapsedTime)
{
	// �ҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		event�t�@�C������J�ڂ̐؂�ւ�
	// 
	// -------------------------------------------

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_2);
	}
	if (owner->SwitchEventData("Avoid") && owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->InputTurn();
	}
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack3);
	}
}

// ************************************************************
// 
//		���U���X�e�[�g
// 
// ************************************************************

void ComboB_1State::Enter()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_1), false);
}

void ComboB_1State::Execute(const float& elapsedTime)
{
	// �ҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		event�t�@�C������J�ڂ̐؂�ւ�
	// 
	// -------------------------------------------

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_2);
	}
	if (owner->SwitchEventData("Avoid") && owner->controller->GetKeyPrecede(Key::KeyA))
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->InputTurn();
	}
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack1);
	}
}

void ComboB_1State::Exit()
{
	owner->firstAttack = false;
}

void ComboB_2State::Enter()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_2), false);
}

void ComboB_2State::Execute(const float& elapsedTime)
{
	// �ҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_3);
	}
	if (owner->SwitchEventData("Avoid") && owner->controller->GetKeyPrecede(Key::KeyA))
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->InputTurn();
	}
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack2);
	}
}

void ComboB_3State::Enter()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_3), false);
}

void ComboB_3State::Execute(const float& elapsedTime)
{
	// �ҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_4);
	}
	if (owner->SwitchEventData("Avoid") && owner->controller->GetKeyPrecede(Key::KeyA))
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->InputTurn();
	}
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack3);
	}
}

void ComboB_4State::Enter()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_4), false);
}

void ComboB_4State::Execute(const float& elapsedTime)
{
	// �ҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	if (owner->SwitchEventData("Avoid") && owner->controller->GetKeyPrecede(Key::KeyA))
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->InputTurn();
	}
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack4);
	}
}

// ************************************************************
// 
//		�_�b�V���U���X�e�[�g
// 
// ************************************************************

void Attack_DashState::Enter()
{	
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Dash_Attack), false);
}

void Attack_DashState::Execute(const float& elapsedTime)
{
	// �A�j���[�V�����I���őҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	if (owner->SwitchEventData("Collision"))
	{
		owner->CollisionNodeVSEnemies(owner->GetAttackData());
	}
	// SE
	if (owner->SwitchEventData("To"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerAttack2);
	}
	if (owner->SwitchEventData("Atare"))
	{
		Audio::Instance().PlayAudio(AudioList::PlayerInVail);
		Audio::Instance().PlayAudio(AudioList::PlayerAttack4);
	}
}

// ************************************************************
// 
//		�_���[�W�X�e�[�g
// 
// ************************************************************

void DamageState::Enter()
{
	standUpTimer = 5.0f;

	animDot = ComputeDot();

	// ���ʂ���U������������ꍇ
	if (animDot > 0.5f)
	{		
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::KnockDown_Back), false);
	}
	// �w�ʂ���U������������ꍇ
	else
	{
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::KnockDown_Front), false);
	}

	// �N���オ��̃A�j���[�V���������߂�
	owner->standUp = animDot > 0.5f;

	// ���G�ݒ�
	owner->SetInvincible(true);
}

void DamageState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	// �N���オ�莞�ԍX�V
	standUpTimer -= elapsedTime;

	if (owner->animator->IsPlayAnimation()) return;

	// �N���オ�莞�Ԃ��O�ɂȂ邩�������͂��ꂽ��N���オ��
	if (standUpTimer < 0.0f || lStickValue != 0.0f)
	{
		owner->ChangeState(Player::State::State_StandUp);
	}
}

// �v���C���[�̑O�����Ɛ�����΂������̓��ς���
// �A�j���[�V������ݒ�
float DamageState::ComputeDot()
{
	DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&owner->GetGameObject()->transform.GetForward()));
	DirectX::XMVECTOR Impulse = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&owner->impulseVec));

	// ����
	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Impulse, Forward);
	float dot = DirectX::XMVectorGetX(Dot);

	return dot;
}

// ************************************************************
// 
//		�N���オ��X�e�[�g
// 
// ************************************************************

void StandUpState::Enter()
{
	// �O��̃X�e�[�g�擾
	const int beforeState = owner->GetStateMachine()->GetBeforState();

	// �A�j���[�V�����ݒ�
	// ���S���邩���ʂ���U������������ꍇ
	if (owner->standUp || beforeState == static_cast<int>(Player::State::State_Death))
	{	
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::StandUp_Back), false);
	}
	// �w�ʂ���U������������ꍇ
	else
	{		
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::StandUp_Front), false);
	}
}

void StandUpState::Execute(const float& elapsedTime)
{
	// �A�j���[�V�����I���őҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->SetInvincible(false);
		owner->ChangeState(Player::State::State_Idle);
	}
}

// ************************************************************
//		���S�X�e�[�g
// ************************************************************

void DeathState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Death), false);
}

void DeathState::Execute(const float& elapsedTime)
{
	// �A�j���[�V�����I�����N���オ����͂őҋ@�X�e�[�g�ɑJ��
	if (!owner->animator->IsPlayAnimation() && owner->controller->GetKeyPrecede(Key::KeyA))
	{
		owner->SetHealth(owner->GetMaxHealth());
		owner->ChangeState(Player::State::State_StandUp);
	}
}

void Counter_StartState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Counter_Start), false);
}

void Counter_StartState::Execute(const float& elapsedTime)
{
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Counter_End);
	}
}

void Counter_AttackState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Counter_Attack), false);
}

void Counter_AttackState::Execute(const float& elapsedTime)
{
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}
}

void Counter_EndState::Enter()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Counter_End), false);
}

void Counter_EndState::Execute(const float& elapsedTime)
{
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}
}