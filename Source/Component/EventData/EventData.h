#pragma once
#include <memory>
#include <vector>
#include <map>

#include "System/Math/Mathf.h"

// 攻撃のデータ
struct AttackData
{
    float attackPower    = 0;     // 攻撃力
    int   hitStopFrame   = 0;     // ヒットストップ
    float invincibleTime = 0.5f;  // 無敵時間

    template<class Archive>
    void serialize(Archive& archive);
};

// 攻撃の移動値
struct MoveData
{
    // 移動値
    float lerpRate = 0.0f;

    template<class Archive>
    void serialize(Archive& archive);
};

// この区間で行動を発生させる
struct EventData
{
    int beginFrame;
    int endFrame;
    std::string eventType;

    AttackData attackData;
    MoveData   moveData;

    template<class Archive>
    void serialize(Archive& archive);
};

class EventInfo
{
public:
    // 特定のアニメーション再生時間内で切り替える
    bool possibleEvent(int animNo, const char* eventType, float second);

    void Deserialize(const char* filename);

    const AttackData& GetAttackData() { return saveAttackData; }
    const MoveData& GetMoveData() { return saveMoveData; }

public:
    // アニメーションの番号、アニメーションのイベント
    std::map<int, std::vector<EventData>> eventDatas = {};

    // 情報の取得用に保存しておくための構造体
private:
    AttackData saveAttackData;
    MoveData saveMoveData;
};