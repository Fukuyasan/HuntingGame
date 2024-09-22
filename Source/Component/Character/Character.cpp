#include "Character.h"

// �_���[�W��^����
bool Character::ApplyDamage(int damage, float invincibleTime)
{
    // ���G���Ԃ��c���Ă���΃_���[�W�������s��Ȃ�
    // �_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
    // ���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
    if (invincibleTimer > 0.0f || health <= 0) return false;

    // �_���[�W����
    health -= damage;

    // ���G���Ԃ�ݒ�
    invincibleTimer = invincibleTime;

    // ���S�ʒm
    if (health <= 0) { OnDead(); }
    // �_���[�W�ʒm   
    else { OnDamaged(); }

    // ���N��Ԃ��ύX���ꂽ�ꍇ��true��Ԃ�
    return true;
}

// ���G���ԍX�V
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
