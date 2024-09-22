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

    // �X�e�[�g
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

    // �q�b�g�G�t�F�N�g
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

    // �f�o�b�O�p
    void OnDebugGUI() override;
    void OnDebugPrimitive() override;

    const char* GetName() const override { return "Player"; }

public:
    void ChangeState(State state) { stateMachine->ChangeState(static_cast<int>(state)); }

    // �ړ����͏���
    void InputMove(const float& elapsedTime);

    // �_�b�V��
    bool InputDash();

    // ���
    bool InputAvoid(int frame = 20);

    void InputTurn();

public:
#pragma region �Z�b�^�[ �Q�b�^�[
    // �X�e�[�g�}�V�[��
    StateMachine<Player>* GetStateMachine() { return stateMachine.get(); }

    // ���
    void SetAvoidVec();
    void PlayAvoid(const float& elapsedTime);

    // �X�^�~�i
    float GetStamina() const { return stamina; }
    float GetMaxStamina() const { return maxStamina; }

#pragma endregion

#pragma region �Փˏ���
private:
    // �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();
    void CollisionNodeVSEnemies(float power, int hitStop, float invisible);

    // �X�e�[�g�}�V���o�^
    void RegisterStateMachine();

public:
    // ���ƓG�̏Փˏ���
    void CollisionNodeVSEnemies(const AttackData& data);
#pragma endregion

private:
    // ���n�������ɌĂ΂��
    //void OnLanding() override;

    // �_���[�W���󂯂����ɌĂ΂��
    void OnDamaged() override;

    // ���S�������ɌĂ΂��
    void OnDead() override;

private:
    float turnSpeed = DirectX::XMConvertToRadians(1080);

    // �q�b�g�X�g�b�v
    float hitStopFrame = 0;
    bool HitStop();

    // ���G����
    float invincibleFrame = 0;
    bool invincible = false;

    // ���
    DirectX::XMFLOAT3 avoidVec = {};
    float avoidSpeed = 0.0f;
    const float maxAvoidSpeed = 10.0f;

    // �X�^�~�i
    float stamina = 0.0f;
    float maxStamina = 150.0f;
    float recoveryStamina = 20.0f;  // �X�^�~�i���񕜂���l

    // �����X�^�~�i
    float consumeDash = 30.0f;
    float consumeAvoid = 50.0f;


public:
    bool firstAttack = false;  // �������ǂ����̔���

    // ���G�֌W
    void SetInvincibleFrame(float time) { invincibleFrame = time; }
    void SetInvincible(bool flg) { invincible = flg; }
    bool Invincible();

    // ������΂�����
    DirectX::XMFLOAT3 impulseVec;
    void SetImpulseVec(const DirectX::XMFLOAT3& vec) { impulseVec = vec; }

    bool standUp = false;

public:
    // �R���g���[���[
    std::unique_ptr<PlayerContorller> controller = nullptr;

    // �v���C���[�̃X�e�[�g�}�V�[��
    std::unique_ptr<StateMachine<Player>> stateMachine = nullptr;

    // UI
    //PlayerUI ui;
};