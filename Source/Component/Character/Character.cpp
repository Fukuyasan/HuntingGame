#include "Character.h"

// ダメージを与える
bool Character::ApplyDamage(int damage, float invincibleTime)
{
    // 無敵時間が残っていればダメージ処理を行わない
    // ダメージが0の場合は健康状態を変更する必要がない
    // 死亡している場合は健康状態を変更しない
    if (invincibleTimer > 0.0f || health <= 0) return false;

    // ダメージ処理
    health -= damage;

    // 無敵時間を設定
    invincibleTimer = invincibleTime;

    // 死亡通知
    if (health <= 0) { OnDead(); }
    // ダメージ通知   
    else { OnDamaged(); }

    // 健康状態が変更された場合はtrueを返す
    return true;
}

// 無敵時間更新
void Character::UpdateInvincibleTimer(const float& elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

bool Character::CoolTimer()
{
    return coolTime < 0.0f;
}

void Character::UpdateCoolTimer(const float& elapsedTime)
{
    if (coolTime > 0.0f)
    {
        coolTime -= elapsedTime;
    }
}

bool Character::SwitchEventData(const char* eventType)
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

    const float animSeconds = animator->GetAnimationSeconds();
    const int animIndex     = animator->GetAnimationIndex();

    return eventInfo.possibleEvent(animIndex, eventType, animSeconds);
}

const AttackData& Character::GetAttackData()
{
    return eventInfo.GetAttackData();
}

const MoveData& Character::GetMoveData()
{
    return eventInfo.GetMoveData();
}
