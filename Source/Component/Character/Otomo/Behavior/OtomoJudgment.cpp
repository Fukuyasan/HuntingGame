#include "OtomoJudgment.h"

#include "System/Math/Mathf.h"
#include "GameObject/GameObjectManager.h"


bool J_OtomoCondition::Judgment()
{
    return owner->condition & condition;
}

bool J_SkillGauge::Judgment()
{
    return false;
}

bool J_OtomoWander::Judgment()
{
	auto target = GameObjectManager::Instance().Find("Player");

	// ‘ÎÛ‚Æ‚Ì‹——£‚ğZo
	const DirectX::XMFLOAT3& position  = owner->GetGameObject()->GetTransform().GetPosition();
	const DirectX::XMFLOAT3& targetPos = target->GetTransform().GetPosition();

	float dist = GameMath::LengthSqFromXZ(position, targetPos);

	return dist > range * range;
}
