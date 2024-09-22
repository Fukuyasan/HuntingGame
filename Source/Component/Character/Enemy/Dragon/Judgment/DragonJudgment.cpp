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
	// ˆê“x’Ê‚é‚ÆŸ‚Í’Ê‚³‚È‚¢
	if (passible) return false;
	
	// “{‚èó‘Ô
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

	// ‘ÎÛ‚Æ‚Ì‹——£‚ğZo
	const DirectX::XMFLOAT3& position  = owner->GetGameObject()->GetTransform().GetPosition();
	const DirectX::XMFLOAT3& targetPos = target->GetTransform().GetPosition();

	float dist = GameMath::LengthSqFromXZ(position, targetPos);

	return dist > range * range;
}

bool J_DragonWander::Judgment()
{
	// ‘ÎÛ‚Æ‚Ì‹——£‚ğZo
	const DirectX::XMFLOAT3& position = owner->GetGameObject()->GetTransform().GetPosition();
	const DirectX::XMFLOAT3& target   = owner->GetTargetPosition();

	float dist = GameMath::LengthSqFromXZ(position, target);

	return dist > range * range;
}
