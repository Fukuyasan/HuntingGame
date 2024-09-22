#include "WyvernJudgment.h"
#include "System/Math/Mathf.h"

#ifdef DEBUG

bool J_WyvernCondition::Judgment()
{
	return (owner->condition & condition);
}

//bool J_Range::Judgment()
//{
//	// ‘ÎÛ‚Æ‚Ì‹——£‚ðŽZo
//	const DirectX::XMFLOAT3& position = owner->transform.GetPosition();
//	const DirectX::XMFLOAT3& target   = Player::Instance().transform.GetPosition();
//
//	float dist = GameMath::LengthSqFromXZ(position, target);
//
//	return dist > range * range;
//}

bool J_WyvernWander::Judgment()
{
	// ‘ÎÛ‚Æ‚Ì‹——£‚ðŽZo
	const DirectX::XMFLOAT3& position = owner->transform.GetPosition();
	const DirectX::XMFLOAT3& target   = owner->GetTargetPosition();

	float dist = GameMath::LengthSqFromXZ(position, target);

	return dist > range * range;
}
#endif // DEBUG
