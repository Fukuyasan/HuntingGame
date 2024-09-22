#pragma once
#include "System/Math/Mathf.h"
#include "System/Time/TimeManager.h"
#include "System/Collision/Collision.h"

#include "Audio/Audio.h"

#include "Component/Component.h"
#include "Component/Animator/Animator.h"
#include "Component/Movement/Movement.h"
#include "Component/EventData/EventData.h"
#include "Component/DamageObject/DamageObject.h"

// �L�����N�^�[
class Character : public Component, DamageObject
{
public:
    // �_���[�W����
    bool ApplyDamage(int damage, float invincibleTime);

    // ���a�擾
    float GetRadius() const { return radius; } 
    
    // �����擾
    float GetHeight() const { return height; }

    // �̗�
    void SetHealth(int h) { this->health = h; }
    const int& GetHealth() const { return health; }
    const int& GetMaxHealth() const { return maxHealth; }

    // ��������擾
    bool IsAlive() { return isAlive; }

public:
    // ���G���ԍX�V
    void UpdateInvincibleTimer(const float& elapsedTime);

    // �N�[���^�C��   
    bool CoolTimer();
    void SetCoolTimer(float time) { this->coolTime = time; }
    void UpdateCoolTimer(const float& elapsedTime);

    // �G���A�ԍ�
    int GetAreaNum() { return areaNum; }
    void SetAreaNum(int num) { areaNum = num; }

    // �e�f�[�^�X�V
    bool SwitchEventData(const char* eventType);
    const AttackData& GetAttackData();
    const MoveData& GetMoveData();

public:
    EventInfo eventInfo;
    std::shared_ptr<Movement> movement;
    std::shared_ptr<Animator> animator;

protected:
    // �L�����N�^�[�̍���
    float height = 2.0f;     
    float radius = 0.5f;
    float penentrationRadius = 0.5f;
    float weight = 0.5f;

    // �̗�
    int health    = 5;
    int maxHealth = 5;

    // �N�[���^�C��
    float coolTime    = 1.0f;
    float maxCoolTime = 1.0f;

    // ��������
    bool isAlive = true;  // ���� : true, ���S : false

    // �G���A�̔ԍ�
    int areaNum = 0;
};
