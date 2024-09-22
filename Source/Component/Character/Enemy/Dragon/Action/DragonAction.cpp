#include "DragonAction.h"

#include "System/Math/Mathf.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectManager.h"

#include "AI/NavMesh/NavMesh.h"
#include "AI/NavMesh/NavMeshAgent.h"
#include "AI/MetaAI/MetaAI.h"

#include "Camera/Camera.h"
#include "Camera/CameraController.h"

#include "Graphics/PostEffect.h"
#include "Graphics/Model/ResourceManager.h"

//#include "Manager/GameManager.h"

#pragma region �U���s��
#pragma region �p�ōU��
bool A_Horn::Start()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::HornAttack), false);
	owner->SetAnimationSpeed(0.5f, 0.7f);

	// �U���ݒ�
	owner->SetAttackRadius(1.5f);

	// SE
	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	return true;
}
ActionState A_Horn::Run(const float& elapsedTime)
{
	// �U���������������
	if (owner->SwitchEventData("Fetch"))
	{
		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(540), 1.0f);
	}
	// �Փ˔��菈��������
	if (owner->SwitchEventData("Collision"))
	{
		owner->animator->SetAnimSpeed(0.8f);
		owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), 5.0f * owner->GetMagnification());
		//owner->CollisionNodeVSPlayer(owner->headIndex);
	}

	// ���s���� : ���s��
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region �܍U��
bool A_Claw::Start()
{
	// �A�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::ClawAttack), false);
	owner->animator->SetAnimSpeed(0.7f);

	// �U���ݒ�
	owner->SetAttackRadius(8.0f);

	// SE
	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	// �J�����̋��� : ��
	CameraController::Instance().SetRangeMode(RangeMode::Range_Far, 5.0f);

	// ��̃f�[�^�ǂݍ���
	filenames[0] = "Data/Model/Rock/RockDesert/RockDesert.mdl";
	filenames[1] = "Data/Model/Rock/Rock.mdl";
	random = Mathf::RandomProbability(0.3f);
	owner->stopRockShot = false;

	return true;
}
ActionState A_Claw::Run(const float& elapsedTime)
{
	// �U���������������
	if (owner->SwitchEventData("Fetch"))
	{				
		// ���\�[�X�ǂݍ��� : 3���Ŋ�̂�����ɂȂ�悤��
		resource = ResourceManager::Instance().LoadModelResourceAsync(filenames[random]);

		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(360), 1.0f);
	}
	// ����
	if (owner->SwitchEventData("Strength"))
	{
		owner->SetAnimationSpeed(0.2f, 0.8f);
	}
	// �ړ�����
	if (owner->SwitchEventData("Move"))
	{
		owner->animator->SetAnimSpeed(0.8f);
		owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), 10.0f * owner->GetMagnification());
	}
	// �U��
	if (owner->SwitchEventData("Collision"))
	{
		// �E��ʒu
		const DirectX::XMFLOAT3 HandPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->handIndex);

		// �����蔻��
		//owner->CollisionNodeVSPlayer(HandPos);

		// ����΂�
		//owner->RockShot(resource, HandPos, 3);
	}
	if (owner->SwitchEventData("Effect"))
	{
		// �E��ʒu
		const DirectX::XMFLOAT3 HandPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->handIndex);

		// �J�����V�F�C�N
		CameraController::Instance().SetShakeMode(2.0f, 5.0f, 30.0f);

		//// �G�t�F�N�g
		//owner->fileEffect->Play(HandPos, 2.0f);
		//owner->smokeEffect->Play(HandPos, 0.7f);

		// �{�莞�Ȃ�p�[�e�B�N��
		if (owner->GetCondition() & Dragon::Condition::C_Angry)
			owner->eruption->Initialize(HandPos, 0.1f);

		// ���W�A���u���[
		PostEffect::Instance().SetBlurPos(HandPos);
		PostEffect::Instance().SetBlurParam(30.0f, 5.0f);
	}
	// ���s���� : ���s��
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region ���݂��U��
bool A_Bite::Start()
{
	// �A�j���[�V�����Đ�
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::BasicAttack), false);

	// �A�j���[�V�������x�ݒ�
	owner->animator->SetAnimSpeed(animSpeed);

	// �U���͈͐ݒ�
	owner->SetAttackRadius(1.3f);

	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	return true;
}
ActionState A_Bite::Run(const float& elapsedTime)
{	
	// �U���������������
	if (owner->SwitchEventData("Fetch") && fatchFlg)
	{
		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(360), 1.0f);
	}
	// �Փ˔��菈��������
	if (owner->SwitchEventData("Collision"))
	{
		owner->animator->SetAnimSpeed(1.0f);
		//owner->CollisionNodeVSPlayer(owner->tongueIndex);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), 15.0f * owner->GetMagnification());
	}

	// ���s���� : ���s��
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region �ːi�U��
bool A_Rush::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Run), true);
	owner->animator->SetAnimSpeed(2.0f);

	owner->SetRunTimer(runTimer);

	// �U���͈͐ݒ�
	owner->SetAttackRadius(3.0f);
	owner->SetAttackPower(25.0f * owner->GetMagnification());

	// ����񐔂̐ݒ�
	turnCount = maxTurnCount;

	return true;
}
ActionState A_Rush::Run(const float& elapsedTime)
{
	// �v���C���[�Ƃ̓����蔻��
	//owner->CollisionNodeVSPlayer(owner->headIndex);

	// ���Ԍo�߂őJ��
	owner->SubRunTimer(elapsedTime);
	if (owner->GetRunTimer() < 0.0f)
	{
		// ����̉񐔂��Ȃ��Ȃ�I��
		if (turnCount <= 0)
		{
			Audio::Instance().PlayAudio(AudioList::DragonFootStep);

			return ActionState::Complete;
		}

		// ����
		auto target = GameObjectManager::Instance().Find("Player");
		if (owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(270), 0.99f))
		{
			rushSpeed = maxRushSpeed;

			--turnCount;
			owner->SetRunTimer(runTimer);
		}

		// ���X�Ɍ���������
		rushSpeed *= 0.99f;
	}

	// �v���C���[�Ƃ̈ʒu�őJ��
	if (turnCount <= 0)
	{
		
		auto target = GameObjectManager::Instance().Find("Player");
		float lengthSq = GameMath::LengthSqFromXZ(owner->GetGameObject()->GetTransform().GetPosition(), target->GetTransform().GetPosition());
		const float range = 10.0f;
		if (lengthSq < range * range)
		{
			return ActionState::Complete;
		}
		
	}

	// ����������
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// �ړ�����
	const float moveSpeed = rushSpeed * owner->GetMagnification();
	owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), moveSpeed);

	return ActionState::Run;
}
#pragma endregion

#pragma region �W�����v
bool A_Jump::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Jump), false);
	owner->animator->SetAnimSpeed(1.0f);

	return true;
}
ActionState A_Jump::Run(const float& elapsedTime)
{
	const float animationTime = owner->animator->GetAnimationSeconds();

	// �U���������������
	if (owner->SwitchEventData("Fetch"))
	{
		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(540), 1.0f);
	}
	// �W�����v
	if (owner->SwitchEventData("Jump"))
	{
		// �W�����v����������v�Z
		DirectX::XMFLOAT3 direction = owner->GetGameObject()->GetTransform().GetForward();
		direction.y += (power > 0.0f) ? 0.1f : -0.1f;
		DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction));

		Direction = DirectX::XMVectorScale(Direction, power);
		DirectX::XMStoreFloat3(&direction, Direction);

		owner->movement->AddImpulse(direction);
	}
	// �G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonLand);
	}

	// ���s���� : ���s��
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region �ǐ�
bool A_Pursuit::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Run), true);
	owner->animator->SetAnimSpeed(1.5f);

	auto target = GameObjectManager::Instance().Find("Player");
	owner->SetTargetPosition(target->GetTransform().GetPosition());

	return true;
}
ActionState A_Pursuit::Run(const float& elapsedTime)
{
	// �ǂ�������
	owner->MoveToTarget(elapsedTime, 2.0f);

	// �G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// �^�[�Q�b�g�Ƃ̋������v�Z
	const float distSq = GameMath::LengthSqFromXZ(owner->GetGameObject()->GetTransform().GetPosition(), owner->GetTargetPosition());
	const float range = owner->GetAttackRange() * 0.5f;

	return (distSq <= range * range) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion
#pragma endregion

#pragma region �v���C���[�Ɛ���ĂȂ����̍s��
#pragma region �p�j����
bool A_Wander::Start()
{
	// �p�j���[�V�����ݒ�
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Run), true);
	owner->animator->SetAnimSpeed(1.0f);

	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_Wander::Run(const float& elapsedTime)
{
	// �ړI�n�_�܂ł�XZ���ʂł̋�������
	const DirectX::XMFLOAT3& position       = owner->GetGameObject()->GetTransform().GetPosition();
	const DirectX::XMFLOAT3& targetPosition = owner->GetTargetPosition();

	// �ړI�n�_�ֈړ�
	owner->MoveToTarget(elapsedTime, 1.0f);

	// �G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(position, 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// �ړI�n�֒����� || �v���C���[����
	const float range  = owner->GetRadius();
	const float distSq = GameMath::LengthSqFromXZ(position, targetPosition);
	bool playerFind    = owner->GetCondition() & Dragon::Condition::C_Find;

	// ���s��
	return (distSq <= range * range || playerFind) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region �ҋ@�s��
bool A_Idle::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::IdleBattle), true);
	owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));

	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_Idle::Run(const float& elapsedTime)
{
	// �^�C�}�[�X�V
	owner->SubRunTimer(elapsedTime);

	// �ҋ@���Ԃ��߂�����
	// �v���C���[���G����
	bool find = owner->GetCondition() & Dragon::Condition::C_Find;

	return (owner->GetRunTimer() < 0.0f || find) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion
#pragma endregion

#pragma region �U������������Ƃ��̍s��
#pragma region ���ݍs��
bool A_Flinch::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::GetHit), false);
	owner->animator->SetAnimSpeed(0.7f);

	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	owner->SetLookFlg(false);
	
	// ���݃��Z�b�g
	owner->DropCondition(Dragon::Condition::C_Flinch);

	return true;
}
ActionState A_Flinch::Run(const float& elapsedTime)
{
	return  (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region ���S�s��
bool A_Die::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Die), false);
	owner->animator->SetAnimSpeed(0.5f);

	// �J�������o
	CameraController::Instance().SetClearPerform(6.0f);
	CameraController::Instance().SetOwner(owner->GetGameObject());
	CameraController::Instance().SetRangeMode(RangeMode::Range_Perform);

	Audio::Instance().PlayAudio(AudioList::DragonRoar);

	owner->SetLookFlg(false);

	return true;
}
ActionState A_Die::Run(const float& elapsedTime)
{
	const float animationTime = owner->animator->GetAnimationSeconds();

	// �����̃G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonLand);
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());
	}

	// �����X�^�[�����S�������̃J��������
	if (CameraController::Instance().ClearPerform(elapsedTime))
	{
		// �|�X�g�G�t�F�N�g
		lerpRate += elapsedTime * 0.3f;
		
		// �J���[�o�����X
		PostEffect::Instance().LerpColorBalance(angryColor, normalColor, lerpRate);

		if (lerpRate < 1.0f) return ActionState::Run;
		
		//GameManager::Instance().ReturnTitle();
	}
	return ActionState::Run;
}
#pragma endregion
#pragma endregion

#pragma region �G���A�ړ�����
bool A_AreaMove::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Run), true);
	owner->animator->SetAnimSpeed(1.4f);

	// ���̃G���A�ԍ���ݒ�
	areaNum = owner->GetAreaNum();
	areaNum = ++areaNum % 5;  // ���̃G���A % �G���A��
	owner->SetAreaNum(areaNum);

	// �G���A���擾
	MetaAI::UseAreaData areaData = MetaAI::Instance().GetAreaData(areaNum);
	areaPos   = areaData.areaPos;
	areaRange = areaData.areaRange;

	// �ړ��J�n
	owner->GetAgent()->StartMove(owner->GetAgentID(), owner->GetGameObject()->GetTransform().GetPosition(), areaPos);

	return true;
}
ActionState A_AreaMove::Run(const float& elapsedTime)
{
	dtCrowd* crowd = owner->GetAgent()->GetCrowd();
	Transform transform = owner->GetGameObject()->GetTransform();
		
	// �G���ړ�������
	transform.SetPositionX(crowd->getAgent(0)->npos[0]);
	owner->GetGameObject()->GetTransform().SetPositionZ(crowd->getAgent(0)->npos[2]);

	DirectX::XMFLOAT3 nextPos{};
	nextPos.x = transform.GetPosition().x + crowd->getAgent(0)->vel[0];
	nextPos.z = transform.GetPosition().z + crowd->getAgent(0)->vel[2];

	// �G�͈ړ���������Ɍ���
	owner->FaceTarget(elapsedTime, nextPos, DirectX::XMConvertToRadians(540), 1.0f);

	// �ړI�n�ɂ�������s����
	const float lengthSq = GameMath::LengthSqFromXZ(transform.GetPosition(), areaPos);
	const float radius   = owner->GetAttackRange();
	if (lengthSq < radius * radius)
	{
		// �G���A�ړ��Ɣ�����Ԃ�����
		owner->DropCondition(Dragon::Condition::C_AreaMove);
		owner->DropCondition(Dragon::Condition::C_Find);

		// �؍ݎ��ԃ��Z�b�g
		owner->SetStayTime(300.0f);

		// �o�H�T�������Z�b�g
		crowd->resetMoveTarget(0);

		// �p�j����͈͐ݒ�
		owner->SetTerritory(transform.GetPosition(), areaRange);

		return ActionState::Complete;
	}
	// �G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(transform.GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	return ActionState::Run;
}
#pragma endregion

#pragma region ���K
bool A_Roar::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::BigScream), false);
	owner->animator->SetAnimSpeed(1.0f);
	
	owner->SetAttackPower(roarPower);
	owner->SetAttackRadius(20.0f);
	
	// ���܂Ȃ��悤��
	owner->SetNotFlinch(true);

	// �J�����̋�����ݒ�
	if (!(owner->GetCondition() & Enemy::Condition::C_Angry)) return true;

	CameraController::Instance().SetRangeMode(RangeMode::Range_Far, 4.0f);
	
	return true;
}
ActionState A_Roar::Run(const float& elapsedTime)
{
	// �|�X�g�G�t�F�N�g�̐ݒ�
	// �u���[�̈ʒu�ݒ�
	const DirectX::XMFLOAT3 headPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->headIndex);
	PostEffect::Instance().SetBlurPos(headPos);

	// �p�[�e�B�N���A�G�t�F�N�g�̏o��
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonRoar);

		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());

		// �{���Ԏ��̂݃p�[�e�B�N��
		if (owner->GetCondition() & Enemy::Condition::C_Angry)
		{
			owner->breath->Initialize(owner->GetGameObject()->GetTransform().GetPosition(),   0.1f);
			owner->eruption->Initialize(owner->GetGameObject()->GetTransform().GetPosition(), 0.1f);
		}
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
	if (owner->SwitchEventData("Collision") && roarPower != 0.0f)
	{
		//owner->CollisionNodeVSPlayer(owner->headIndex);
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
#pragma endregion

#pragma region �Њd
bool A_Intimidate::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Scream), false);
	owner->animator->SetAnimSpeed(0.7f);

	return true;
}
ActionState A_Intimidate::Run(const float& elapsedTime)
{
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonVoice);
	}

	// ���s���� : ���s��
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region ������
bool A_Back::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Walk_Back), true);
	owner->animator->SetAnimSpeed(1.0f);

	owner->SetRunTimer(backTime);

	return true;
}
ActionState A_Back::Run(const float& elapsedTime)
{
	// �^�C�}�[�X�V
	owner->SubRunTimer(elapsedTime);	

	// �v���C���[�̕���������
	auto target = GameObjectManager::Instance().Find("Player");
	owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(540), 1.0f);

	// �������ɕ���
	DirectX::XMFLOAT3 direction{};
	direction.x = owner->GetGameObject()->GetTransform().GetForward().x * -1.0f;
	direction.z = owner->GetGameObject()->GetTransform().GetForward().z * -1.0f;
	owner->movement->MoveDirection(direction, 5.0f);

	// SE
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// �ҋ@���Ԃ��߂�����
	return (owner->GetRunTimer() < 0.0f) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region �U�����
bool A_Turn::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Walk_Right), false);
	owner->animator->SetAnimSpeed(1.5f);

	Audio::Instance().PlayAudio(AudioList::DragonFootStep);

	return false;
}
ActionState A_Turn::Run(const float& elapsedTime)
{
	auto target = GameObjectManager::Instance().Find("Player");
	owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(180), 0.8f);

	// ���s���� : ���s��
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;	
}
#pragma endregion

#pragma region �W�����v�U��
bool A_JumpAttack::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Jump), false);
	owner->animator->SetAnimSpeed(0.5f);

	// �U���͈͐ݒ�
	owner->SetAttackRadius(5.0f);
	owner->SetAttackPower(15.0f * owner->GetMagnification());

	return true;
}
ActionState A_JumpAttack::Run(const float& elapsedTime)
{
	const float animationTime = owner->animator->GetAnimationSeconds();

	if (owner->SwitchEventData("Jump"))
	{
		owner->animator->SetAnimSpeed(1.0f);
	}
	// ���n�U��
	if (owner->SwitchEventData("Collision"))
	{
		//owner->CollisionNodeVSPlayer(owner->transform.GetPosition());
	}
	// �G�t�F�N�g
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonLand);
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());
	}

	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region ��ꂽ�Ƃ��̍s��
bool A_Tired::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::IdleBattle), true);
	owner->animator->SetAnimSpeed(1.0f);

	owner->SetRunTimer(tiredTime);

	owner->SetLookFlg(true);

	return true;
}
ActionState A_Tired::Run(const float& elapsedTime)
{
	// �^�C�}�[�X�V
	owner->SubRunTimer(elapsedTime);

	// �ҋ@���Ԃ��߂�����
	if (owner->GetRunTimer() < 0.0f)
	{
		owner->SetLookFlg(false);
		return ActionState::Complete;
	}

	return ActionState::Run;
}
#pragma endregion

#pragma region ���[�r�[�s��
bool A_MovieClaw::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::ClawAttack), false);
	owner->animator->SetAnimSpeed(0.7f);

	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	// �J�������o
	CameraController::Instance().SetEye({ 70, 9, 85 });
	CameraController::Instance().SetOwner(owner->GetGameObject());
	CameraController::Instance().SetRangeMode(RangeMode::Range_Perform);

	// UI���\��
	//GameManager::Instance().ShowUI(false);

	return true;
}
ActionState A_MovieClaw::Run(const float& elapsedTime)
{
	const float animationTime = owner->animator->GetAnimationSeconds();

	if (owner->SwitchEventData("Effect"))
	{
		// �E��ʒu
		const DirectX::XMFLOAT3 HandPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->handIndex);

		// SE
		Audio::Instance().PlayAudio(AudioList::DragonExplosion);

		// �J�����V�F�C�N
		CameraController::Instance().SetShakeMode(2.0f, 5.0f, 30.0f);

		// �G�t�F�N�g
		/*owner->fileEffect->Play(HandPos, 2.0f);
		owner->smokeEffect->Play(HandPos, 0.7f);*/

		// ���W�A���u���[
		PostEffect::Instance().SetBlurPos(HandPos);
		PostEffect::Instance().SetBlurParam(30.0f, 5.0f);
	}

	// ���s���� : ���s��
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_MovieRoar::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::BigScream), false);
	owner->animator->SetAnimSpeed(0.7f);

	CameraController::Instance().SetRangeMode(RangeMode::Range_Far, 4.5f);

	return true;
}
ActionState A_MovieRoar::Run(const float& elapsedTime)
{
	// ���ʒu�𒆐S�Ƀu���[��ݒ�
	const DirectX::XMFLOAT3 headPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->headIndex);
	PostEffect::Instance().SetBlurPos(headPos);

	// �A�j���[�V�����I��
	if (!owner->animator->IsPlayAnimation())
	{
		owner->DropCondition(Dragon::Condition::C_Movie);

		// �J���������Ƃɖ߂�
		/*CameraController::Instance().SetOwner(&Player::Instance());
		CameraController::Instance().SetRangeMode(RangeMode::Range_First);*/

		// UI��\��
		//GameManager::Instance().ShowUI(true);
		
		// ���s������Ԃ�
		return ActionState::Complete;
	}

	// ���K
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonRoar);

		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());
	}
	if (owner->SwitchEventData("Roar"))
	{
		// �u���[�ݒ�
		PostEffect::Instance().SetBlurParam(30.0f, 1.0f);

		// �J�����V�F�C�N
		CameraController::Instance().SetShakeMode(1.0f, 2.5f, 5.0f);
	}

	// ���s����Ԃ�
	return ActionState::Run;
}
#pragma endregion