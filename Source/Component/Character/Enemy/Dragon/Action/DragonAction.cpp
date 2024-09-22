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

#pragma region 攻撃行動
#pragma region 角で攻撃
bool A_Horn::Start()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::HornAttack), false);
	owner->SetAnimationSpeed(0.5f, 0.7f);

	// 攻撃設定
	owner->SetAttackRadius(1.5f);

	// SE
	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	return true;
}
ActionState A_Horn::Run(const float& elapsedTime)
{
	// 振り向き処理をする
	if (owner->SwitchEventData("Fetch"))
	{
		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(540), 1.0f);
	}
	// 衝突判定処理をする
	if (owner->SwitchEventData("Collision"))
	{
		owner->animator->SetAnimSpeed(0.8f);
		owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), 5.0f * owner->GetMagnification());
		//owner->CollisionNodeVSPlayer(owner->headIndex);
	}

	// 実行成功 : 実行中
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 爪攻撃
bool A_Claw::Start()
{
	// アニメーション設定
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::ClawAttack), false);
	owner->animator->SetAnimSpeed(0.7f);

	// 攻撃設定
	owner->SetAttackRadius(8.0f);

	// SE
	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	// カメラの距離 : 遠
	CameraController::Instance().SetRangeMode(RangeMode::Range_Far, 5.0f);

	// 岩のデータ読み込み
	filenames[0] = "Data/Model/Rock/RockDesert/RockDesert.mdl";
	filenames[1] = "Data/Model/Rock/Rock.mdl";
	random = Mathf::RandomProbability(0.3f);
	owner->stopRockShot = false;

	return true;
}
ActionState A_Claw::Run(const float& elapsedTime)
{
	// 振り向き処理をする
	if (owner->SwitchEventData("Fetch"))
	{				
		// リソース読み込み : 3割で顔のついた岩になるように
		resource = ResourceManager::Instance().LoadModelResourceAsync(filenames[random]);

		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(360), 1.0f);
	}
	// 溜め
	if (owner->SwitchEventData("Strength"))
	{
		owner->SetAnimationSpeed(0.2f, 0.8f);
	}
	// 移動処理
	if (owner->SwitchEventData("Move"))
	{
		owner->animator->SetAnimSpeed(0.8f);
		owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), 10.0f * owner->GetMagnification());
	}
	// 攻撃
	if (owner->SwitchEventData("Collision"))
	{
		// 右手位置
		const DirectX::XMFLOAT3 HandPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->handIndex);

		// 当たり判定
		//owner->CollisionNodeVSPlayer(HandPos);

		// 岩を飛ばす
		//owner->RockShot(resource, HandPos, 3);
	}
	if (owner->SwitchEventData("Effect"))
	{
		// 右手位置
		const DirectX::XMFLOAT3 HandPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->handIndex);

		// カメラシェイク
		CameraController::Instance().SetShakeMode(2.0f, 5.0f, 30.0f);

		//// エフェクト
		//owner->fileEffect->Play(HandPos, 2.0f);
		//owner->smokeEffect->Play(HandPos, 0.7f);

		// 怒り時ならパーティクル
		if (owner->GetCondition() & Dragon::Condition::C_Angry)
			owner->eruption->Initialize(HandPos, 0.1f);

		// ラジアルブラー
		PostEffect::Instance().SetBlurPos(HandPos);
		PostEffect::Instance().SetBlurParam(30.0f, 5.0f);
	}
	// 実行成功 : 実行中
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 噛みつき攻撃
bool A_Bite::Start()
{
	// アニメーション再生
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::BasicAttack), false);

	// アニメーション速度設定
	owner->animator->SetAnimSpeed(animSpeed);

	// 攻撃範囲設定
	owner->SetAttackRadius(1.3f);

	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	return true;
}
ActionState A_Bite::Run(const float& elapsedTime)
{	
	// 振り向き処理をする
	if (owner->SwitchEventData("Fetch") && fatchFlg)
	{
		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(360), 1.0f);
	}
	// 衝突判定処理をする
	if (owner->SwitchEventData("Collision"))
	{
		owner->animator->SetAnimSpeed(1.0f);
		//owner->CollisionNodeVSPlayer(owner->tongueIndex);
	}
	if (owner->SwitchEventData("Move"))
	{
		owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), 15.0f * owner->GetMagnification());
	}

	// 実行成功 : 実行中
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 突進攻撃
bool A_Rush::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Run), true);
	owner->animator->SetAnimSpeed(2.0f);

	owner->SetRunTimer(runTimer);

	// 攻撃範囲設定
	owner->SetAttackRadius(3.0f);
	owner->SetAttackPower(25.0f * owner->GetMagnification());

	// 旋回回数の設定
	turnCount = maxTurnCount;

	return true;
}
ActionState A_Rush::Run(const float& elapsedTime)
{
	// プレイヤーとの当たり判定
	//owner->CollisionNodeVSPlayer(owner->headIndex);

	// 時間経過で遷移
	owner->SubRunTimer(elapsedTime);
	if (owner->GetRunTimer() < 0.0f)
	{
		// 旋回の回数がないなら終了
		if (turnCount <= 0)
		{
			Audio::Instance().PlayAudio(AudioList::DragonFootStep);

			return ActionState::Complete;
		}

		// 旋回
		auto target = GameObjectManager::Instance().Find("Player");
		if (owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(270), 0.99f))
		{
			rushSpeed = maxRushSpeed;

			--turnCount;
			owner->SetRunTimer(runTimer);
		}

		// 徐々に原則させる
		rushSpeed *= 0.99f;
	}

	// プレイヤーとの位置で遷移
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

	// 砂煙をだす
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// 移動処理
	const float moveSpeed = rushSpeed * owner->GetMagnification();
	owner->movement->MoveDirection(owner->GetGameObject()->GetTransform().GetForward(), moveSpeed);

	return ActionState::Run;
}
#pragma endregion

#pragma region ジャンプ
bool A_Jump::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Jump), false);
	owner->animator->SetAnimSpeed(1.0f);

	return true;
}
ActionState A_Jump::Run(const float& elapsedTime)
{
	const float animationTime = owner->animator->GetAnimationSeconds();

	// 振り向き処理をする
	if (owner->SwitchEventData("Fetch"))
	{
		auto target = GameObjectManager::Instance().Find("Player");
		owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(540), 1.0f);
	}
	// ジャンプ
	if (owner->SwitchEventData("Jump"))
	{
		// ジャンプする方向を計算
		DirectX::XMFLOAT3 direction = owner->GetGameObject()->GetTransform().GetForward();
		direction.y += (power > 0.0f) ? 0.1f : -0.1f;
		DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction));

		Direction = DirectX::XMVectorScale(Direction, power);
		DirectX::XMStoreFloat3(&direction, Direction);

		owner->movement->AddImpulse(direction);
	}
	// エフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonLand);
	}

	// 実行成功 : 実行中
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 追跡
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
	// 追いかける
	owner->MoveToTarget(elapsedTime, 2.0f);

	// エフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// ターゲットとの距離を計算
	const float distSq = GameMath::LengthSqFromXZ(owner->GetGameObject()->GetTransform().GetPosition(), owner->GetTargetPosition());
	const float range = owner->GetAttackRange() * 0.5f;

	return (distSq <= range * range) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion
#pragma endregion

#pragma region プレイヤーと戦ってない時の行動
#pragma region 徘徊処理
bool A_Wander::Start()
{
	// 徘徊モーション設定
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Run), true);
	owner->animator->SetAnimSpeed(1.0f);

	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_Wander::Run(const float& elapsedTime)
{
	// 目的地点までのXZ平面での距離判定
	const DirectX::XMFLOAT3& position       = owner->GetGameObject()->GetTransform().GetPosition();
	const DirectX::XMFLOAT3& targetPosition = owner->GetTargetPosition();

	// 目的地点へ移動
	owner->MoveToTarget(elapsedTime, 1.0f);

	// エフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(position, 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// 目的地へ着いた || プレイヤー発見
	const float range  = owner->GetRadius();
	const float distSq = GameMath::LengthSqFromXZ(position, targetPosition);
	bool playerFind    = owner->GetCondition() & Dragon::Condition::C_Find;

	// 実行中
	return (distSq <= range * range || playerFind) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 待機行動
bool A_Idle::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::IdleBattle), true);
	owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));

	owner->SetRandomTargetPosition();

	return true;
}
ActionState A_Idle::Run(const float& elapsedTime)
{
	// タイマー更新
	owner->SubRunTimer(elapsedTime);

	// 待機時間が過ぎた時
	// プレイヤー索敵成功
	bool find = owner->GetCondition() & Dragon::Condition::C_Find;

	return (owner->GetRunTimer() < 0.0f || find) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion
#pragma endregion

#pragma region 攻撃をくらったときの行動
#pragma region 怯み行動
bool A_Flinch::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::GetHit), false);
	owner->animator->SetAnimSpeed(0.7f);

	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	owner->SetLookFlg(false);
	
	// 怯みリセット
	owner->DropCondition(Dragon::Condition::C_Flinch);

	return true;
}
ActionState A_Flinch::Run(const float& elapsedTime)
{
	return  (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 死亡行動
bool A_Die::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Die), false);
	owner->animator->SetAnimSpeed(0.5f);

	// カメラ演出
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

	// 砂埃のエフェクト
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonLand);
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());
	}

	// モンスターが死亡した時のカメラ制御
	if (CameraController::Instance().ClearPerform(elapsedTime))
	{
		// ポストエフェクト
		lerpRate += elapsedTime * 0.3f;
		
		// カラーバランス
		PostEffect::Instance().LerpColorBalance(angryColor, normalColor, lerpRate);

		if (lerpRate < 1.0f) return ActionState::Run;
		
		//GameManager::Instance().ReturnTitle();
	}
	return ActionState::Run;
}
#pragma endregion
#pragma endregion

#pragma region エリア移動処理
bool A_AreaMove::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Run), true);
	owner->animator->SetAnimSpeed(1.4f);

	// 次のエリア番号を設定
	areaNum = owner->GetAreaNum();
	areaNum = ++areaNum % 5;  // 次のエリア % エリア数
	owner->SetAreaNum(areaNum);

	// エリア情報取得
	MetaAI::UseAreaData areaData = MetaAI::Instance().GetAreaData(areaNum);
	areaPos   = areaData.areaPos;
	areaRange = areaData.areaRange;

	// 移動開始
	owner->GetAgent()->StartMove(owner->GetAgentID(), owner->GetGameObject()->GetTransform().GetPosition(), areaPos);

	return true;
}
ActionState A_AreaMove::Run(const float& elapsedTime)
{
	dtCrowd* crowd = owner->GetAgent()->GetCrowd();
	Transform transform = owner->GetGameObject()->GetTransform();
		
	// 敵を移動させる
	transform.SetPositionX(crowd->getAgent(0)->npos[0]);
	owner->GetGameObject()->GetTransform().SetPositionZ(crowd->getAgent(0)->npos[2]);

	DirectX::XMFLOAT3 nextPos{};
	nextPos.x = transform.GetPosition().x + crowd->getAgent(0)->vel[0];
	nextPos.z = transform.GetPosition().z + crowd->getAgent(0)->vel[2];

	// 敵は移動する方向に向く
	owner->FaceTarget(elapsedTime, nextPos, DirectX::XMConvertToRadians(540), 1.0f);

	// 目的地についたら実行成功
	const float lengthSq = GameMath::LengthSqFromXZ(transform.GetPosition(), areaPos);
	const float radius   = owner->GetAttackRange();
	if (lengthSq < radius * radius)
	{
		// エリア移動と発見状態を解除
		owner->DropCondition(Dragon::Condition::C_AreaMove);
		owner->DropCondition(Dragon::Condition::C_Find);

		// 滞在時間リセット
		owner->SetStayTime(300.0f);

		// 経路探索をリセット
		crowd->resetMoveTarget(0);

		// 徘徊する範囲設定
		owner->SetTerritory(transform.GetPosition(), areaRange);

		return ActionState::Complete;
	}
	// エフェクト
	if (owner->SwitchEventData("Effect"))
	{
		//owner->smokeEffect->Play(transform.GetPosition(), 0.5f);
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	return ActionState::Run;
}
#pragma endregion

#pragma region 咆哮
bool A_Roar::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::BigScream), false);
	owner->animator->SetAnimSpeed(1.0f);
	
	owner->SetAttackPower(roarPower);
	owner->SetAttackRadius(20.0f);
	
	// 怯まないように
	owner->SetNotFlinch(true);

	// カメラの距離を設定
	if (!(owner->GetCondition() & Enemy::Condition::C_Angry)) return true;

	CameraController::Instance().SetRangeMode(RangeMode::Range_Far, 4.0f);
	
	return true;
}
ActionState A_Roar::Run(const float& elapsedTime)
{
	// ポストエフェクトの設定
	// ブラーの位置設定
	const DirectX::XMFLOAT3 headPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->headIndex);
	PostEffect::Instance().SetBlurPos(headPos);

	// パーティクル、エフェクトの出現
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonRoar);

		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());

		// 怒り状態時のみパーティクル
		if (owner->GetCondition() & Enemy::Condition::C_Angry)
		{
			owner->breath->Initialize(owner->GetGameObject()->GetTransform().GetPosition(),   0.1f);
			owner->eruption->Initialize(owner->GetGameObject()->GetTransform().GetPosition(), 0.1f);
		}
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

#pragma region 威嚇
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

	// 実行成功 : 実行中
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 後ろ歩き
bool A_Back::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Walk_Back), true);
	owner->animator->SetAnimSpeed(1.0f);

	owner->SetRunTimer(backTime);

	return true;
}
ActionState A_Back::Run(const float& elapsedTime)
{
	// タイマー更新
	owner->SubRunTimer(elapsedTime);	

	// プレイヤーの方向を向く
	auto target = GameObjectManager::Instance().Find("Player");
	owner->FaceTarget(elapsedTime, target->GetTransform().GetPosition(), DirectX::XMConvertToRadians(540), 1.0f);

	// 後ろ向きに歩く
	DirectX::XMFLOAT3 direction{};
	direction.x = owner->GetGameObject()->GetTransform().GetForward().x * -1.0f;
	direction.z = owner->GetGameObject()->GetTransform().GetForward().z * -1.0f;
	owner->movement->MoveDirection(direction, 5.0f);

	// SE
	if (owner->SwitchEventData("Sound"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonFootStep);
	}

	// 待機時間が過ぎた時
	return (owner->GetRunTimer() < 0.0f) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 振り向き
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

	// 実行成功 : 実行中
	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;	
}
#pragma endregion

#pragma region ジャンプ攻撃
bool A_JumpAttack::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::Jump), false);
	owner->animator->SetAnimSpeed(0.5f);

	// 攻撃範囲設定
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
	// 着地攻撃
	if (owner->SwitchEventData("Collision"))
	{
		//owner->CollisionNodeVSPlayer(owner->transform.GetPosition());
	}
	// エフェクト
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonLand);
		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());
	}

	return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}
#pragma endregion

#pragma region 疲れたときの行動
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
	// タイマー更新
	owner->SubRunTimer(elapsedTime);

	// 待機時間が過ぎた時
	if (owner->GetRunTimer() < 0.0f)
	{
		owner->SetLookFlg(false);
		return ActionState::Complete;
	}

	return ActionState::Run;
}
#pragma endregion

#pragma region ムービー行動
bool A_MovieClaw::Start()
{
	owner->animator->PlayAnimation(static_cast<int>(Dragon::EnemyAnimation::ClawAttack), false);
	owner->animator->SetAnimSpeed(0.7f);

	Audio::Instance().PlayAudio(AudioList::DragonVoice);

	// カメラ演出
	CameraController::Instance().SetEye({ 70, 9, 85 });
	CameraController::Instance().SetOwner(owner->GetGameObject());
	CameraController::Instance().SetRangeMode(RangeMode::Range_Perform);

	// UIを非表示
	//GameManager::Instance().ShowUI(false);

	return true;
}
ActionState A_MovieClaw::Run(const float& elapsedTime)
{
	const float animationTime = owner->animator->GetAnimationSeconds();

	if (owner->SwitchEventData("Effect"))
	{
		// 右手位置
		const DirectX::XMFLOAT3 HandPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->handIndex);

		// SE
		Audio::Instance().PlayAudio(AudioList::DragonExplosion);

		// カメラシェイク
		CameraController::Instance().SetShakeMode(2.0f, 5.0f, 30.0f);

		// エフェクト
		/*owner->fileEffect->Play(HandPos, 2.0f);
		owner->smokeEffect->Play(HandPos, 0.7f);*/

		// ラジアルブラー
		PostEffect::Instance().SetBlurPos(HandPos);
		PostEffect::Instance().SetBlurParam(30.0f, 5.0f);
	}

	// 実行成功 : 実行中
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
	// 頭位置を中心にブラーを設定
	const DirectX::XMFLOAT3 headPos = owner->GetGameObject()->GetModel()->GetNodePos(owner->headIndex);
	PostEffect::Instance().SetBlurPos(headPos);

	// アニメーション終了
	if (!owner->animator->IsPlayAnimation())
	{
		owner->DropCondition(Dragon::Condition::C_Movie);

		// カメラをもとに戻す
		/*CameraController::Instance().SetOwner(&Player::Instance());
		CameraController::Instance().SetRangeMode(RangeMode::Range_First);*/

		// UIを表示
		//GameManager::Instance().ShowUI(true);
		
		// 実行成功を返す
		return ActionState::Complete;
	}

	// 咆哮
	if (owner->SwitchEventData("Effect"))
	{
		Audio::Instance().PlayAudio(AudioList::DragonRoar);

		//owner->smokeEffect->Play(owner->GetGameObject()->GetTransform().GetPosition());
	}
	if (owner->SwitchEventData("Roar"))
	{
		// ブラー設定
		PostEffect::Instance().SetBlurParam(30.0f, 1.0f);

		// カメラシェイク
		CameraController::Instance().SetShakeMode(1.0f, 2.5f, 5.0f);
	}

	// 実行中を返す
	return ActionState::Run;
}
#pragma endregion