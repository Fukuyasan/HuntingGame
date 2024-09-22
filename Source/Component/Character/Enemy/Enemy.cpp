#include "Enemy.h"
#include "Graphics/ShaderState.h"
#include "EnemyManager.h"
#include "GameObject/GameObjectManager.h""

void Enemy::DrawDebugHitData(DebugRenderer* renderer, const HitInformation::HitData& hit)
{
	// ローカル座標からワールド座標に変換
	const int nodeIndex     = gameObject.lock()->GetModel()->FindNodeIndex(hit.nodeName);
	const Model::Node& node = gameObject.lock()->GetModel()->GetNodes()[nodeIndex];

	DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(&hit.position);
	DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
	DirectX::XMVECTOR worldPosition = DirectX::XMVector3Transform(localPosition, worldTransform);

	DirectX::XMFLOAT3 hitPosition{};
	DirectX::XMStoreFloat3(&hitPosition, worldPosition);

	renderer->AddSphere(
		hitPosition,
		hit.radius,
		DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)
	);
}

// カプセル描画
void Enemy::DrawDebugCapsule(DebugRenderer* renderer, const Capsule& capsule)
{
	// ローカル座標からワールド座標に変換
	DirectX::XMFLOAT3 startPosition{}, goalPosition{};
	GameMath::ConvertWorldCoordinates(capsule.startPosition, gameObject.lock()->GetTransform().GetTransform(), startPosition);
	GameMath::ConvertWorldCoordinates(capsule.goalPosition,  gameObject.lock()->GetTransform().GetTransform(), goalPosition);

	renderer->AddCapsule(
		startPosition,
		goalPosition,
		capsule.radius,
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
	);
}

void Enemy::DrawDebugSphere(DebugRenderer* renderer, const Sphere& sphere)
{
	DirectX::XMFLOAT3 Position{};
	GameMath::ConvertWorldCoordinates(sphere.position, gameObject.lock()->GetTransform().GetTransform(), Position);

	renderer->AddSphere(
		Position,
		sphere.radius,
		DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)
	);
}

// 縄張り設定
void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;

	// 範囲の変更がなかった場合元の値を代入
	territoryRange = range > 0.0f ? range : territoryRange;
}

// ターゲット位置をランダム設定
void Enemy::SetRandomTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

// 目的地点へ移動
void Enemy::MoveToTarget(const float& elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - gameObject.lock()->GetTransform().GetPosition().x;
	float vz = targetPosition.z - gameObject.lock()->GetTransform().GetPosition().z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	movement->Move(vx, vz, moveSpeed * speedRate);
	movement->Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// プレイヤー索敵
bool Enemy::SearchTarget(const float range, const DirectX::XMFLOAT3& target)
{
	// 発見状態でなければ
	if (!(condition & Condition::C_Find)) {

		// プレイヤーとの高低差を考慮して3Dで距離判定をする
		const float dist = GameMath::LengthSq(gameObject.lock()->GetTransform().GetPosition(), target);

		// 範囲外なら false
		if (dist > range * range) return false;
	}

	// XZ方面のベクトル取得
	DirectX::XMVECTOR PosXZ = DirectX::XMVectorSet(gameObject.lock()->GetTransform().GetPosition().x, 0, gameObject.lock()->GetTransform().GetPosition().z, 0);
	DirectX::XMVECTOR TargetXZ = DirectX::XMVectorSet(target.x, 0, target.z, 0);

	// ターゲットへのベクトル取得
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetXZ, PosXZ));

	// 方向ベクトル取得
	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMVectorSet(gameObject.lock()->GetTransform().GetForward().x, 0, gameObject.lock()->GetTransform().GetForward().z, 0));

	// 2つのベクトルの内積値で前後判定
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Direction, Front));

	return dot > 0.5f;
}

// オブジェクトの位置の方向へ向く
bool Enemy::FaceTarget(const float& elapsedTime, const DirectX::XMFLOAT3& target, float value, const float angle)
{
	// ベクトルを取得
	DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&gameObject.lock()->GetTransform().GetForward()));

	// オブジェクトとのベクトルを求める
	DirectX::XMVECTOR Target   = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&gameObject.lock()->GetTransform().GetPosition());
	DirectX::XMVECTOR Vec      = DirectX::XMVectorSubtract(Target, Position);
	DirectX::XMVECTOR N_Vec    = DirectX::XMVector3Normalize(Vec);

	// 前後判定
	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Forward, N_Vec);

	float dot = DirectX::XMVectorGetX(Dot);
	// プレイヤーが後ろにいたら
	if (dot < angle) {
		DirectX::XMFLOAT3 vec;
		DirectX::XMStoreFloat3(&vec, (Vec));
		movement->Turn(elapsedTime, vec.x, vec.z, value);

		return false;
	}

	return true;
}

// 破棄
void Enemy::Destroy()
{
	// 破棄リストに追加
	GameObjectManager::Instance().Remove(this->GetGameObject());
}
