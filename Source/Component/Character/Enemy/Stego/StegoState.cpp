#include "StegoState.h"
#include "AI/MetaAI/MetaAI.h"
#include "Component/Character/Player/player.h"

#include "Camera/Camera.h"
#include "Camera/CameraController.h"

#ifdef Stego

// ************************************************************
//		待機ステート
// ************************************************************

void StegoIdleState::Enter()
{
	owner->animator->PlayAnimation(static_cast<int>(Stego::Animation::A_Idle), true);
}

void StegoIdleState::Execute(const float& elapsedTime)
{
	// 敵を移動させる	
	if (owner->GetCondition() & Stego::Condition::C_AreaMove)
	{
		owner->ChangeState(Stego::State::S_Run);
	}
}

// ************************************************************
//		走りステート
// ************************************************************

void StegoRunState::Enter()
{
	owner->animator->PlayAnimation(static_cast<int>(Stego::Animation::A_Run), true);
	owner->animator->SetAnimSpeed(3.0f);

	// 移動場所セット3
	int num = owner->GetAreaNum();
	num = ++num % 5;
	owner->SetAreaNum(num);
	
	// エリア移動
	areaPos = MetaAI::Instance().GetAreaData(num).areaPos;
	owner->GetAgent()->StartMove(owner->GetAgentID(), owner->transform.GetPosition(), areaPos);
}

void StegoRunState::Execute(const float& elapsedTime)
{
	owner->UpdateCoolTimer(elapsedTime);

	//owner->GetAgent()->Target(owner->GetAgentID(), Player::Instance().transform.GetPosition());

	// 敵を移動させる
	dtCrowd* crowd = owner->GetAgent()->GetCrowd();
	owner->transform.SetPositionX(crowd->getAgent(owner->GetAgentID())->npos[0]);
	owner->transform.SetPositionY(crowd->getAgent(owner->GetAgentID())->npos[1]);
	owner->transform.SetPositionZ(crowd->getAgent(owner->GetAgentID())->npos[2]);

	DirectX::XMFLOAT3 nextPos{};
	nextPos.x = owner->transform.GetPosition().x + crowd->getAgent(owner->GetAgentID())->vel[0];
	nextPos.z = owner->transform.GetPosition().z + crowd->getAgent(owner->GetAgentID())->vel[2];

	// 敵は移動する方向に向く
	owner->FaceTarget(elapsedTime, nextPos, DirectX::XMConvertToRadians(1080), 1.0f);

	// 目的地についたら実行成功
	const float lengthSq = GameMath::LengthSqFromXZ(owner->transform.GetPosition(), areaPos);
	const float radius   = 20.0f;
	if (lengthSq < radius * radius)
	{
		// 経路探索をリセット
		crowd->resetMoveTarget(owner->GetAgentID());

		// 待機ステートに遷移
		owner->ChangeState(Stego::State::S_Idle);
	}

	// エフェクト
	if (owner->SwitchEventData("Effect"))
	{		
		owner->smokeEffect->Play(owner->transform.GetPosition());
	}
}


// ************************************************************
//		死亡ステート
// ************************************************************

void StegoDeathState::Enter()
{
	owner->animator->PlayAnimation(static_cast<int>(Stego::Animation::A_Death), false);
}

void StegoDeathState::Execute(const float& elapsedTime)
{
	
}

#endif