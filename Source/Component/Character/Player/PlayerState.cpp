#include "PlayerState.h"
#include "PlayerController.h"
#include "System/Math/Easing.h"

constexpr inline float effectScale = 0.3f;

// ************************************************************
//		待機ステート
// ************************************************************

void IdleState::Enter()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Idle), true);
	owner->firstAttack = true;
}

void IdleState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	// 走りステートに遷移
	if (lStickValue != 0)
	{
		owner->ChangeState(Player::State::State_Run);
	}
	// 回避ステートに遷移
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// カウンター攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyX))
	{
		owner->ChangeState(Player::State::State_Counter_Start);
	}
	// 弱攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_1);
	}
	// 強攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_1);
	}
	// ダッシュ攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyYB))
	{
		owner->ChangeState(Player::State::State_Attack_Dash);
	}
}

// ************************************************************
//		歩きステート
// ************************************************************

void WalkState::Enter()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Walk), true);
	owner->movement->SetMaxMoveSpeed(3.0f);
	owner->firstAttack = true;
}

void WalkState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	owner->InputMove(elapsedTime);
		
	// 回避ステート
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// 弱攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_1);
	}
	// 強攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_1);
	}
	// 待機ステート
	if (lStickValue == 0.0f)
	{
		owner->ChangeState(Player::State::State_Idle);
	}
	// 走りステート
	if (lStickValue >= 0.5f)
	{
		owner->ChangeState(Player::State::State_Run);
	}	

	// 砂のエフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}
}

void WalkState::Exit()
{
}

// ************************************************************
//		走りステート
// ************************************************************

void RunState::Enter()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Jogging), true);
	owner->movement->SetMaxMoveSpeed(5.0f);
	owner->firstAttack = true;
}

void RunState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();
	owner->InputMove(elapsedTime);

	// 歩きステートに遷移
	if (lStickValue < 0.5f)
	{
		owner->ChangeState(Player::State::State_Walk);
	}
	// 回避ステートに遷移
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// 弱攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_1);
	}
	// 強攻撃
	if (owner->controller->GetKeyPrecede(Key::KeyB))
	{
		owner->ChangeState(Player::State::State_ComboB_1);
	}
	// ダッシュステートに遷移
	if (lStickValue > 0.9f && owner->InputDash())
	{
		owner->ChangeState(Player::State::State_Dash);
	}
	
	// 砂のエフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}
}

void RunState::Exit()
{
}

// ************************************************************
//		ダッシュステート
// ************************************************************

void DashState::Enter()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Run), true);
	owner->movement->SetMaxMoveSpeed(8.0f);
}

void DashState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();
	//owner->stamina -= owner->consumeDash * elapsedTime;

	owner->InputMove(elapsedTime);

	// 走りステートに遷移
	if (!owner->InputDash() || lStickValue < 0.5f)
	{
		// 連打されないようにする
		owner->SetCoolTimer(0.5f);
		owner->ChangeState(Player::State::State_Run);
	}
	// 回避ステートに遷移
	if (owner->InputAvoid())
	{
		owner->ChangeState(Player::State::State_Avoid);
	}
	// ダッシュ攻撃ステートに遷移
	if (owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_Attack_Dash);
	}
	
	// 砂のエフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}
}

void DashState::Exit()
{
}

// ************************************************************
//		回避ステート
// ************************************************************

void AvoidState::Enter()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Douge), false);

	// 回避の方向を設定
	owner->SetAvoidVec();
	owner->firstAttack = true;
}

void AvoidState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	// 回避処理
	owner->PlayAvoid(elapsedTime);
	
	// 待機ステート遷移
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		eventファイルから遷移の切り替え
	// 
	// -------------------------------------------

	// 回避途中で移動入力があれば走りステート遷移
	if (owner->SwitchEventData("Move") && lStickValue != 0.0f)
	{
		owner->ChangeState(Player::State::State_Run);
	}
	if (owner->SwitchEventData("Switch") && owner->controller->GetKeyPrecede(Key::KeyY))
	{
		owner->ChangeState(Player::State::State_ComboA_2);
	}
	// 砂のエフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->transform.GetPosition(), effectScale);
	}	
}

// ************************************************************
// 
//		弱攻撃ステート
// 
// ************************************************************

void ComboA_1State::Enter()
{
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboA_1), false);
}

void ComboA_1State::Execute(const float& elapsedTime)
{
	// アニメーション終了で待機ステートに遷移
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		eventファイルから遷移の切り替え
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
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboA_2), false);
}

void ComboA_2State::Execute(const float& elapsedTime)
{
	// 待機ステートに遷移
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		eventファイルから遷移の切り替え
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
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboA_3), false);
}

void ComboA_3State::Execute(const float& elapsedTime)
{
	// 待機ステートに遷移
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		eventファイルから遷移の切り替え
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
//		強攻撃ステート
// 
// ************************************************************

void ComboB_1State::Enter()
{
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_1), false);
}

void ComboB_1State::Execute(const float& elapsedTime)
{
	// 待機ステートに遷移
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}

	// -------------------------------------------
	// 
	//		eventファイルから遷移の切り替え
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
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_2), false);
}

void ComboB_2State::Execute(const float& elapsedTime)
{
	// 待機ステートに遷移
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
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_3), false);
}

void ComboB_3State::Execute(const float& elapsedTime)
{
	// 待機ステートに遷移
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
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::ComboB_4), false);
}

void ComboB_4State::Execute(const float& elapsedTime)
{
	// 待機ステートに遷移
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
//		ダッシュ攻撃ステート
// 
// ************************************************************

void Attack_DashState::Enter()
{	
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Dash_Attack), false);
}

void Attack_DashState::Execute(const float& elapsedTime)
{
	// アニメーション終了で待機ステートに遷移
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
//		ダメージステート
// 
// ************************************************************

void DamageState::Enter()
{
	standUpTimer = 5.0f;

	animDot = ComputeDot();

	// 正面から攻撃をくらった場合
	if (animDot > 0.5f)
	{		
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::KnockDown_Back), false);
	}
	// 背面から攻撃をくらった場合
	else
	{
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::KnockDown_Front), false);
	}

	// 起き上がりのアニメーションを決める
	owner->standUp = animDot > 0.5f;

	// 無敵設定
	owner->SetInvincible(true);
}

void DamageState::Execute(const float& elapsedTime)
{
	const float lStickValue = owner->controller->GetGamePad().GetAxisLStick();

	// 起き上がり時間更新
	standUpTimer -= elapsedTime;

	if (owner->animator->IsPlayAnimation()) return;

	// 起き上がり時間が０になるか何か入力されたら起き上がる
	if (standUpTimer < 0.0f || lStickValue != 0.0f)
	{
		owner->ChangeState(Player::State::State_StandUp);
	}
}

// プレイヤーの前方向と吹き飛ばす方向の内積から
// アニメーションを設定
float DamageState::ComputeDot()
{
	DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&owner->GetGameObject()->transform.GetForward()));
	DirectX::XMVECTOR Impulse = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&owner->impulseVec));

	// 内積
	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Impulse, Forward);
	float dot = DirectX::XMVectorGetX(Dot);

	return dot;
}

// ************************************************************
// 
//		起き上がりステート
// 
// ************************************************************

void StandUpState::Enter()
{
	// 前回のステート取得
	const int beforeState = owner->GetStateMachine()->GetBeforState();

	// アニメーション設定
	// 死亡するか正面から攻撃をくらった場合
	if (owner->standUp || beforeState == static_cast<int>(Player::State::State_Death))
	{	
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::StandUp_Back), false);
	}
	// 背面から攻撃をくらった場合
	else
	{		
		owner->animator->PlayAnimation(static_cast<int>(Player::Animation::StandUp_Front), false);
	}
}

void StandUpState::Execute(const float& elapsedTime)
{
	// アニメーション終了で待機ステートに遷移
	if (!owner->animator->IsPlayAnimation())
	{
		owner->SetInvincible(false);
		owner->ChangeState(Player::State::State_Idle);
	}
}

// ************************************************************
//		死亡ステート
// ************************************************************

void DeathState::Enter()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Death), false);
}

void DeathState::Execute(const float& elapsedTime)
{
	// アニメーション終了かつ起き上がり入力で待機ステートに遷移
	if (!owner->animator->IsPlayAnimation() && owner->controller->GetKeyPrecede(Key::KeyA))
	{
		owner->SetHealth(owner->GetMaxHealth());
		owner->ChangeState(Player::State::State_StandUp);
	}
}

void Counter_StartState::Enter()
{
	// アニメーション設定
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
	// アニメーション設定
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
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Player::Animation::Counter_End), false);
}

void Counter_EndState::Execute(const float& elapsedTime)
{
	if (!owner->animator->IsPlayAnimation())
	{
		owner->ChangeState(Player::State::State_Idle);
	}
}