#pragma once
#include "Component/Character/Character.h"

#include "AI/StateMachine/StateMachine.h"

#include "PlayerController.h"
#include "PlayerUI.h"

class Player final : public Character
{
public:
    enum class Animation
    {
        ComboA_1,
        ComboA_2,
        ComboA_3,

        ComboB_1,
        ComboB_2,
        ComboB_3,
        ComboB_4,

        Counter_Attack,
        Counter_End,
        Counter_Start,

        Dash_Attack,

        Douge,
        Idle,
        Jogging,
        Run,
        Dash,
        Walk,

        Damage,
        Death,
        KnockDown_Back,
        KnockDown_Front,
        StandUp_Back,
        StandUp_Front,
    };

    // ステート
    enum class State
    {
        State_Idle,
        State_Walk,
        State_Run,
        State_Dash,
        State_Avoid,

        State_ComboA_1,
        State_ComboA_2,
        State_ComboA_3,

        State_ComboB_1,
        State_ComboB_2,
        State_ComboB_3,
        State_ComboB_4,

        State_Attack_Dash,

        State_Counter_Start,
        State_Counter_Attack,
        State_Counter_End,

        State_Damage,
        State_StandUp,
        State_Death,

        State_Non
    };

    // ヒットエフェクト
    enum HitEffect
    {
        Weak,
        Hard,
        Clack,
        Max
    };

public:
    void Awake() override;
    void Start() override;
    void Update() override;

    // デバッグ用
    void OnDebugGUI() override;
    void OnDebugPrimitive() override;

    const char* GetName() const override { return "Player"; }

public:
    void ChangeState(State state) { stateMachine->ChangeState(static_cast<int>(state)); }

    // 移動入力処理
    void InputMove(const float& elapsedTime);

    // ダッシュ
    bool InputDash();

    // 回避
    bool InputAvoid(int frame = 20);

    void InputTurn();

public:
#pragma region セッター ゲッター
    // ステートマシーン
    StateMachine<Player>* GetStateMachine() { return stateMachine.get(); }

    // 回避
    void SetAvoidVec();
    void PlayAvoid(const float& elapsedTime);

    // スタミナ
    float GetStamina() const { return stamina; }
    float GetMaxStamina() const { return maxStamina; }

#pragma endregion

#pragma region 衝突処理
private:
    // プレイヤーとエネミーとの衝突処理
    void CollisionPlayerVsEnemies();
    void CollisionNodeVSEnemies(float power, int hitStop, float invisible);

    // ステートマシン登録
    void RegisterStateMachine();

public:
    // 剣と敵の衝突処理
    void CollisionNodeVSEnemies(const AttackData& data);
#pragma endregion

private:
    // 着地した時に呼ばれる
    //void OnLanding() override;

    // ダメージを受けた時に呼ばれる
    void OnDamaged() override;

    // 死亡した時に呼ばれる
    void OnDead() override;

private:
    float turnSpeed = DirectX::XMConvertToRadians(1080);

    // ヒットストップ
    float hitStopFrame = 0;
    bool HitStop();

    // 無敵時間
    float invincibleFrame = 0;
    bool invincible = false;

    // 回避
    DirectX::XMFLOAT3 avoidVec = {};
    float avoidSpeed = 0.0f;
    const float maxAvoidSpeed = 10.0f;

    // スタミナ
    float stamina = 0.0f;
    float maxStamina = 150.0f;
    float recoveryStamina = 20.0f;  // スタミナが回復する値

    // 消費するスタミナ
    float consumeDash = 30.0f;
    float consumeAvoid = 50.0f;


public:
    bool firstAttack = false;  // 初撃かどうかの判定

    // 無敵関係
    void SetInvincibleFrame(float time) { invincibleFrame = time; }
    void SetInvincible(bool flg) { invincible = flg; }
    bool Invincible();

    // 吹き飛ばす方向
    DirectX::XMFLOAT3 impulseVec;
    void SetImpulseVec(const DirectX::XMFLOAT3& vec) { impulseVec = vec; }

    bool standUp = false;

public:
    // コントローラー
    std::unique_ptr<PlayerContorller> controller = nullptr;

    // プレイヤーのステートマシーン
    std::unique_ptr<StateMachine<Player>> stateMachine = nullptr;

    // UI
    //PlayerUI ui;
};