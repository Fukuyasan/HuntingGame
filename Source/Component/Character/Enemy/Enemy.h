#pragma once
#include "Component/Character/Character.h"

#include "System/Math/Mathf.h"

#include "Graphics/Renderer/DebugRenderer.h"

#include "AI/NavMesh/NavMeshAgent.h"

#include "Component/EventData/HitData.h"

#define PARTICLE_FLG 0
#define AREAMOVE_FLG 1

// エネミー
class Enemy : public Character
{
#pragma region 状態
public:
    enum Condition
    {
        C_Find      = (1 << 0),	 // 発見
        C_Battle    = (1 << 1),	 // 戦闘
        C_AreaMove  = (1 << 2),  // エリア移動
        C_Angry     = (1 << 3),  // 怒り
        C_Flinch    = (1 << 4),  // 怯み
        C_Die       = (1 << 5),  // 死亡
        C_Movie     = (1 << 6),  // ムービー
        C_ALL       = C_Find | C_Battle | C_AreaMove | C_Angry | C_Flinch
    };
#pragma endregion

public:
#pragma region デバッグ
    // 敵のカプセルの描画
    void DrawDebugHitData(DebugRenderer* renderer, const HitInformation::HitData&  hit);
    void DrawDebugCapsule(DebugRenderer* renderer, const Capsule& capsule);
    void DrawDebugSphere(DebugRenderer* renderer,  const Sphere& sphere);

#pragma endregion
    // 破棄
    void Destroy();

    // 縄張り設定
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range = 0.0f);

    // ターゲット位置をランダム設定
    void SetRandomTargetPosition();

    // 目標地点へ移動
    void MoveToTarget(const float& elapsedTime, float speedRate);

    // プレイヤー索敵
    bool SearchTarget(const float range, const DirectX::XMFLOAT3& target);

    // プレイヤーの方向へ向く
    bool FaceTarget(const float& elapsedTime, const DirectX::XMFLOAT3& target, float value = 1.0f, const float angle = 0.0f);

#pragma region セッター ゲッター
public:
    // 範囲取得
    float GetAttackRange()    { return attackRange; }
    float GetTerritoryRange() { return territoryRange; }
    float GetSearchRange()    { return searchRange; }

    // ターゲットポジション
    void SetTargetPosition(DirectX::XMFLOAT3 position) { this->targetPosition = position; }
    const DirectX::XMFLOAT3& GetTargetPosition()       { return targetPosition; }

    // 徘徊行動などで使う実行タイマー設定
    void SetRunTimer(float timer) { this->runTimer  = timer; }
    void SubRunTimer(float timer) { this->runTimer -= timer; }
    float GetRunTimer() { return runTimer; }

    // 攻撃関係
    void SetAttackPower(float power)   { this->attackPower = power; }
    void SetAttackRadius(float radius) { this->attackRadius = radius; }
    float GetAttackRadius() { return attackRadius; }

    // エリア移動
    void  SetStayTime(float time) { this->stayTime = time; }
    float GetStayTime()           { return stayTime; }

    // エージェント
    void SetAgent(std::shared_ptr<NavMeshAgent> agent, UINT id) { this->agent = agent; this->agentID = id; }
    std::shared_ptr<NavMeshAgent> GetAgent() { return agent; }
    UINT GetAgentID() { return agentID; }

    // 倍率
    float GetMagnification() { return magnification; }  // 攻撃倍率
    float GetDefenceRate()   { return defenseRate; }    // 防御倍率

    // 状態の設定
    virtual void SetCondition() {}

    UINT GetCondition() { return condition; }

    void MakeCondition(Condition condition) { this->condition |= static_cast<UINT>(condition); }
    void DropCondition(Condition condition) { this->condition &= static_cast<UINT>(~condition); }

    void MakeClack(UINT clack) { this->clackFlg |= clack; }

    void SetNotFlinch(bool flg) { this->notFlinch = flg; }
    bool IsNotFlinch() { return !this->notFlinch; }

    static inline int IDCount = 0;
    int ID = ++IDCount;

protected:
    // 群衆オブジェクト
    std::shared_ptr<NavMeshAgent> agent;
    UINT agentID = 0;

#pragma region 敵のステータス
    Character* target = nullptr;

    DirectX::XMFLOAT3 targetPosition  = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3 territoryOrigin = { 0.0f,0.0f,0.0f };

    float territoryRange = 20.0f; 
    float searchRange    = 20.0f;
    float attackRange    = 15.0f;

    float moveSpeed = 12.0f;
    float turnSpeed = DirectX::XMConvertToRadians(360);
        
    float attackRadius = 3.0f;  // 攻撃範囲
    float attackPower  = 1.0f;  // 攻撃力
    float runTimer     = 0.0f;

    bool notFlinch     = false;  // ひるまない

public:
    // 状態
    UINT condition = 0;
    // 部位破壊
    UINT clackFlg  = 0;

protected:
    // エリアの滞在時間
    // 0になるとエリア移動
    float stayTime = 0.0f;

    // 攻撃の倍率 : 怒り時だと上がる
    float magnification = 1.0f;
    float defenseRate   = 0.1f;
#pragma endregion
public:
    HitInformation hitinformation;
};