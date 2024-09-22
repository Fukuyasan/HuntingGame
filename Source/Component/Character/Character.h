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

// キャラクター
class Character : public Component, DamageObject
{
public:
    // ダメージ処理
    bool ApplyDamage(int damage, float invincibleTime);

    // 半径取得
    float GetRadius() const { return radius; } 
    
    // 高さ取得
    float GetHeight() const { return height; }

    // 体力
    void SetHealth(int h) { this->health = h; }
    const int& GetHealth() const { return health; }
    const int& GetMaxHealth() const { return maxHealth; }

    // 生存判定取得
    bool IsAlive() { return isAlive; }

public:
    // 無敵時間更新
    void UpdateInvincibleTimer(const float& elapsedTime);

    // クールタイム   
    bool CoolTimer();
    void SetCoolTimer(float time) { this->coolTime = time; }
    void UpdateCoolTimer(const float& elapsedTime);

    // エリア番号
    int GetAreaNum() { return areaNum; }
    void SetAreaNum(int num) { areaNum = num; }

    // 各データ更新
    bool SwitchEventData(const char* eventType);
    const AttackData& GetAttackData();
    const MoveData& GetMoveData();

public:
    EventInfo eventInfo;
    std::shared_ptr<Movement> movement;
    std::shared_ptr<Animator> animator;

protected:
    // キャラクターの高さ
    float height = 2.0f;     
    float radius = 0.5f;
    float penentrationRadius = 0.5f;
    float weight = 0.5f;

    // 体力
    int health    = 5;
    int maxHealth = 5;

    // クールタイム
    float coolTime    = 1.0f;
    float maxCoolTime = 1.0f;

    // 生存判定
    bool isAlive = true;  // 生存 : true, 死亡 : false

    // エリアの番号
    int areaNum = 0;
};
