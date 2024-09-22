#include "DragonJudgment.h"
#include "System/Math/Mathf.h"
#include "GameObject/GameObjectManager.h"

bool J_DragonTurn::Judgment()
{
	auto target = GameObjectManager::Instance().Find("Player");
	return !owner->SearchTarget(owner->GetSearchRange(), target->GetTransform().GetPosition());
}

bool J_DragonAngry::Judgment()
{
	// ��x�ʂ�Ǝ��͒ʂ��Ȃ�
	if (passible) return false;
	
	// �{����
	if((owner->GetCondition() & Dragon::Condition::C_Angry) != 0) 
	{
		passible = true;
		return true;
	}

	return false;
}

bool J_DragonHealth::Judgment()
{
	return owner->GetHealth() < owner->GetMaxHealth() * (healthPercent * 0.01f);
}

bool J_DragonCondition::Judgment()
{
	return owner->GetCondition() & condition;
}

bool J_DragonFar::Judgment()
{
	auto target = GameObjectManager::Instance().Find("Player");

	// �ΏۂƂ̋������Z�o
	const DirectX::XMFLOAT3& position  = owner->GetGameObject()->GetTransform().GetPosition();
	const DirectX::XMFLOAT3& targetPos = target->GetTransform().GetPosition();

	float dist = GameMath::LengthSqFromXZ(position, targetPos);

	return dist > range * range;
}

bool J_DragonWander::Judgment()
{
	// �ΏۂƂ̋������Z�o
	const DirectX::XMFLOAT3& position = owner->GetGameObject()->GetTransform().GetPosition();
	const DirectX::XMFLOAT3& target   = owner->GetTargetPosition();

	float dist = GameMath::LengthSqFromXZ(position, target);

	return dist > range * range;
}
