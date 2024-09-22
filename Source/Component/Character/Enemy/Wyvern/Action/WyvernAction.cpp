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
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::TailAttack), false);

	// 攻撃範囲設定
	owner->SetAttackRadius(1.5f);
	owner->SetAttackPower(15.0f * owner->GetMagnification());

	return true;
}

ActionState A_WyvernClaw::Run(const float& elapsedTime)
{
	// 実行処理
	const float animationTime = owner->animator->GetAnimationSeconds();

	// 振り向き処理をする
	if (owner->SwitchEventData("Fetch"))
	{
		owner->FaceTarget(elapsedTime, Player::Instance().transform.GetPosition(), DirectX::XMConvertToRadians(180), 0.8f);
	}
	// 衝突判定処理をする
	if (owner->SwitchEventData("Collision"))
	{
		//owner->CollisionNodeVSPlayer(owner->headIndex);
	}
	// エフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play();
	}


	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_WyvernHorn::Start()
{
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::HornAttack), false);

	return true;
}

ActionState A_WyvernHorn::Run(const float& elapsedTime)
{
	// 振り向き処理をする
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

	// 成功を返す
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_WyvernBite::Start()
{
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::BiteAttack), false);

	return true;
}

ActionState A_WyvernBite::Run(const float& elapsedTime)
{
	// 振り向き処理をする
	if (owner->SwitchEventData("Fetch"))
	{
		owner->FaceTarget(elapsedTime, Player::Instance().transform.GetPosition(), DirectX::XMConvertToRadians(180), 0.8f);
	}
	if (owner->SwitchEventData("Collision"))
	{

	}

	// 成功を返す
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}


bool A_WyvernRoar::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Roar), false);

	// カメラの距離を設定
	if (owner->GetCondition() & Enemy::Condition::C_Angry)
	{
		CameraController::Instance().SetRangeMode(RangeMode::Range_Far, 4.0f);

		// 怒り咆哮は怯まないように
		owner->SetNotFlinch(true);
	}

	owner->SetAttackPower(roarPower);
	owner->SetAttackRadius(20.0f);

	return true;
}

ActionState A_WyvernRoar::Run(const float& elapsedTime)
{
	// ポストエフェクトの設定
	// ブラーの位置設定
	const DirectX::XMFLOAT3& headPos = owner->GetModel()->GetNodePos(owner->headIndex);
	PostEffect::Instance().SetBlurPos(headPos);

	// パーティクル、エフェクトの出現
	if (owner->SwitchEventData("Effect"))
	{
		owner->bossSEs[Wyvern::SE::Roar]->Play(false);

		owner->smokeEffect->Play(owner->transform.GetPosition());

		// 怒り状態時のみパーティクル
		if (owner->GetCondition() & Enemy::Condition::C_Angry)
		{
			//owner->breath->Initialize(owner->transform.GetPosition(), 0.1f);
			//owner->eruption->Initialize(owner->transform.GetPosition(), 0.1f);
		}
	}
	if (owner->SwitchEventData("Collision") && roarPower != 0.0f)
	{
		// 当たり判定
		//owner->CollisionNodeVSPlayer(headPos);
	}
	if (owner->SwitchEventData("Roar"))
	{
		// ブラー設定
		PostEffect::Instance().SetBlurParam(30.0f, 1.0f);

		// カメラシェイク
		CameraController::Instance().SetShakeMode(1.0f, 2.5f, 5.0f);

		if (owner->GetCondition() & Enemy::Condition::C_Angry)
		{
			lerpTime += elapsedTime;
			lerpTime = (std::min)(lerpTime, 1.0f);

			// 通常ー＞暖色に変更
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


#pragma region プレイヤーと戦ってない時の行動
#pragma region 徘徊処理
bool A_WyvernWander::Start()
{
	// 徘徊モーション設定
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Walk), true);

	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_WyvernWander::Run(const float& elapsedTime)
{
	// 目的地点までのXZ平面での距離判定
	const DirectX::XMFLOAT3& position = owner->transform.GetPosition();
	const DirectX::XMFLOAT3& targetPosition = owner->GetTargetPosition();

	// 目的地点へ移動
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

	// 目的地へ着いた || プレイヤー発見
	const float range  = owner->GetRadius();
	const float distSq = GameMath::LengthSqFromXZ(position, targetPosition);

	bool playerFind = owner->GetCondition() & Wyvern::Condition::C_Find;

	// 実行中
	return (distSq < range * range || playerFind) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 待機行動
bool A_WyvernIdle::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Idle), true);

	owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
	
	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_WyvernIdle::Run(const float& elapsedTime)
{
	// タイマー更新
	owner->SubRunTimer(elapsedTime);
	
	// プレイヤー索敵成功
	bool find = owner->GetCondition() & Wyvern::Condition::C_Find;

	// 待機時間が過ぎた時	|| 発見時
	return (owner->GetRunTimer() < 0.0f || find) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 攻撃をくらったときの行動
#pragma region 怯み行動
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
		// 怯みリセット
		owner->DropCondition(Wyvern::Condition::C_Flinch);

		return ActionState::Complete;
	}

	return ActionState::Run;
}
#pragma endregion

#pragma region 死亡行動
bool A_WyvernDie::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Wyvern::EnemyAnimation::Die), false);

	// カメラ演出
	CameraController::Instance().SetClearPerform(6.0f);
	CameraController::Instance().SetOwner(owner);
	CameraController::Instance().SetRangeMode(RangeMode::Range_Perform);

	return true;
}
ActionState A_WyvernDie::Run(const float& elapsedTime)
{
	// 砂埃のエフェクト
	if (owner->SwitchEventData("Effect"))
	{
		owner->bossSEs[Wyvern::SE::Land]->Play(false);
		owner->smokeEffect->Play(owner->transform.GetPosition());
	}
	if (owner->SwitchEventData("Sound"))
	{
		owner->bossSEs[Wyvern::SE::Die]->Play(false);
	}

	// モンスターが死亡した時のカメラ制御
	if (CameraController::Instance().ClearPerform(elapsedTime))
	{
		// ポストエフェクト
		lerpRate += elapsedTime * 0.3f;

		if (lerpRate >= 1.0f)
		{
			//// カメラをもとに戻す
			//CameraController::Instance().Set_owner(&Player::Instance());
			//CameraController::Instance().SetRangeMode(RangeMode::Range_First, 0.0f);

			GameManager::Instance().ReturnTitle();
		}

		// カラーバランス
		PostEffect::Instance().LerpColorBalance(angryColor, normalColor, lerpRate);
	}

	return ActionState::Run;
}
#pragma endregion
#pragma endregion

#endif