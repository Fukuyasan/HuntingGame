#include "Enemy.h"
#include "Graphics/ShaderState.h"
#include "EnemyManager.h"
#include "GameObject/GameObjectManager.h""

void Enemy::DrawDebugHitData(DebugRenderer* renderer, const HitInformation::HitData& hit)
{
	// ���[�J�����W���烏�[���h���W�ɕϊ�
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

// �J�v�Z���`��
void Enemy::DrawDebugCapsule(DebugRenderer* renderer, const Capsule& capsule)
{
	// ���[�J�����W���烏�[���h���W�ɕϊ�
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

// �꒣��ݒ�
void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;

	// �͈͂̕ύX���Ȃ������ꍇ���̒l����
	territoryRange = range > 0.0f ? range : territoryRange;
}

// �^�[�Q�b�g�ʒu�������_���ݒ�
void Enemy::SetRandomTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

// �ړI�n�_�ֈړ�
void Enemy::MoveToTarget(const float& elapsedTime, float speedRate)
{
	// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - gameObject.lock()->GetTransform().GetPosition().x;
	float vz = targetPosition.z - gameObject.lock()->GetTransform().GetPosition().z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// �ړ�����
	movement->Move(vx, vz, moveSpeed * speedRate);
	movement->Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// �v���C���[���G
bool Enemy::SearchTarget(const float range, const DirectX::XMFLOAT3& target)
{
	// ������ԂłȂ����
	if (!(condition & Condition::C_Find)) {

		// �v���C���[�Ƃ̍��፷���l������3D�ŋ������������
		const float dist = GameMath::LengthSq(gameObject.lock()->GetTransform().GetPosition(), target);

		// �͈͊O�Ȃ� false
		if (dist > range * range) return false;
	}

	// XZ���ʂ̃x�N�g���擾
	DirectX::XMVECTOR PosXZ = DirectX::XMVectorSet(gameObject.lock()->GetTransform().GetPosition().x, 0, gameObject.lock()->GetTransform().GetPosition().z, 0);
	DirectX::XMVECTOR TargetXZ = DirectX::XMVectorSet(target.x, 0, target.z, 0);

	// �^�[�Q�b�g�ւ̃x�N�g���擾
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetXZ, PosXZ));

	// �����x�N�g���擾
	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMVectorSet(gameObject.lock()->GetTransform().GetForward().x, 0, gameObject.lock()->GetTransform().GetForward().z, 0));

	// 2�̃x�N�g���̓��ϒl�őO�㔻��
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Direction, Front));

	return dot > 0.5f;
}

// �I�u�W�F�N�g�̈ʒu�̕����֌���
bool Enemy::FaceTarget(const float& elapsedTime, const DirectX::XMFLOAT3& target, float value, const float angle)
{
	// �x�N�g�����擾
	DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&gameObject.lock()->GetTransform().GetForward()));

	// �I�u�W�F�N�g�Ƃ̃x�N�g�������߂�
	DirectX::XMVECTOR Target   = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&gameObject.lock()->GetTransform().GetPosition());
	DirectX::XMVECTOR Vec      = DirectX::XMVectorSubtract(Target, Position);
	DirectX::XMVECTOR N_Vec    = DirectX::XMVector3Normalize(Vec);

	// �O�㔻��
	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Forward, N_Vec);

	float dot = DirectX::XMVectorGetX(Dot);
	// �v���C���[�����ɂ�����
	if (dot < angle) {
		DirectX::XMFLOAT3 vec;
		DirectX::XMStoreFloat3(&vec, (Vec));
		movement->Turn(elapsedTime, vec.x, vec.z, value);

		return false;
	}

	return true;
}

// �j��
void Enemy::Destroy()
{
	// �j�����X�g�ɒǉ�
	GameObjectManager::Instance().Remove(this->GetGameObject());
}
