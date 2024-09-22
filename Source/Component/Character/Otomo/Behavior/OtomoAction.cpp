#include "OtomoAction.h"
#include "GameObject/GameObjectManager.h"

bool A_OtomoKnockDown::Start()
{
    owner->animator->PlayAnimation(static_cast<int>(Otomo::Animation::KnockDown), false);

    return false;
}

ActionState A_OtomoKnockDown::Run(const float& elapsedTime)
{
    return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_OtomoGetUp::Start()
{
    owner->animator->PlayAnimation(static_cast<int>(Otomo::Animation::GetUp), false);

    return false;
}

ActionState A_OtomoGetUp::Run(const float& elapsedTime)
{
    return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_OtomoWander::Start()
{
    owner->animator->PlayAnimation(static_cast<int>(Otomo::Animation::Run), true);

    return false;
}

ActionState A_OtomoWander::Run(const float& elapsedTime)
{
    auto target = GameObjectManager::Instance().Find("Player");

    // 対象との距離を算出
    const DirectX::XMFLOAT3& position = owner->GetGameObject()->GetTransform().GetPosition();
    const DirectX::XMFLOAT3& targetPos = target->GetTransform().GetPosition();

    // ターゲット方向への進行ベクトルを算出
    float vx = targetPos.x - position.x;
    float vz = targetPos.z - position.z;
    float dist = sqrtf(vx * vx + vz * vz);
    vx /= dist;
    vz /= dist;

    // 移動処理
    owner->movement->Move(vx, vz, 5.0f);
    owner->movement->Turn(elapsedTime, vx, vz, DirectX::XMConvertToRadians(360));

    return (dist < 2.5f) ? ActionState::Complete : ActionState::Run;
}

bool A_OtomoIdle::Start()
{
    owner->animator->PlayAnimation(static_cast<int>(Otomo::Animation::Idle), true);

    return false;
}

ActionState A_OtomoIdle::Run(const float& elapsedTime)
{
    auto target = GameObjectManager::Instance().Find("Player");

    // 対象との距離を算出
    const DirectX::XMFLOAT3& position  = owner->GetGameObject()->GetTransform().GetPosition();
    const DirectX::XMFLOAT3& targetPos = target->GetTransform().GetPosition();

    float dist = GameMath::LengthSqFromXZ(position, targetPos);

    return (dist > 5.0f * 5.0f) ? ActionState::Complete : ActionState::Run;
}

bool A_OtomoAttack::Start()
{
    owner->animator->PlayAnimation(static_cast<int>(Otomo::Animation::Attack_A), false);

    return false;
}

ActionState A_OtomoAttack::Run(const float& elapsedTime)
{
    return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_OtomoSkill_A::Start()
{
    owner->animator->PlayAnimation(static_cast<int>(Otomo::Animation::Skill_A), false);

    return false;
}

ActionState A_OtomoSkill_A::Run(const float& elapsedTime)
{
    return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

bool A_OtomoSkill_B::Start()
{
    owner->animator->PlayAnimation(static_cast<int>(Otomo::Animation::Skill_B), false);

    return false;
}

ActionState A_OtomoSkill_B::Run(const float& elapsedTime)
{
    return (!owner->animator->IsPlayAnimation()) ? ActionState::Complete : ActionState::Run;
}

