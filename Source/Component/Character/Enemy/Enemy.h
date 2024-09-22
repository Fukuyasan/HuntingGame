#pragma once
#include "Component/Character/Character.h"

#include "System/Math/Mathf.h"

#include "Graphics/Renderer/DebugRenderer.h"

#include "AI/NavMesh/NavMeshAgent.h"

#include "Component/EventData/HitData.h"

#define PARTICLE_FLG 0
#define AREAMOVE_FLG 1

// �G�l�~�[
class Enemy : public Character
{
#pragma region ���
public:
    enum Condition
    {
        C_Find      = (1 << 0),	 // ����
        C_Battle    = (1 << 1),	 // �퓬
        C_AreaMove  = (1 << 2),  // �G���A�ړ�
        C_Angry     = (1 << 3),  // �{��
        C_Flinch    = (1 << 4),  // ����
        C_Die       = (1 << 5),  // ���S
        C_Movie     = (1 << 6),  // ���[�r�[
        C_ALL       = C_Find | C_Battle | C_AreaMove | C_Angry | C_Flinch
    };
#pragma endregion

public:
#pragma region �f�o�b�O
    // �G�̃J�v�Z���̕`��
    void DrawDebugHitData(DebugRenderer* renderer, const HitInformation::HitData&  hit);
    void DrawDebugCapsule(DebugRenderer* renderer, const Capsule& capsule);
    void DrawDebugSphere(DebugRenderer* renderer,  const Sphere& sphere);

#pragma endregion
    // �j��
    void Destroy();

    // �꒣��ݒ�
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range = 0.0f);

    // �^�[�Q�b�g�ʒu�������_���ݒ�
    void SetRandomTargetPosition();

    // �ڕW�n�_�ֈړ�
    void MoveToTarget(const float& elapsedTime, float speedRate);

    // �v���C���[���G
    bool SearchTarget(const float range, const DirectX::XMFLOAT3& target);

    // �v���C���[�̕����֌���
    bool FaceTarget(const float& elapsedTime, const DirectX::XMFLOAT3& target, float value = 1.0f, const float angle = 0.0f);

#pragma region �Z�b�^�[ �Q�b�^�[
public:
    // �͈͎擾
    float GetAttackRange()    { return attackRange; }
    float GetTerritoryRange() { return territoryRange; }
    float GetSearchRange()    { return searchRange; }

    // �^�[�Q�b�g�|�W�V����
    void SetTargetPosition(DirectX::XMFLOAT3 position) { this->targetPosition = position; }
    const DirectX::XMFLOAT3& GetTargetPosition()       { return targetPosition; }

    // �p�j�s���ȂǂŎg�����s�^�C�}�[�ݒ�
    void SetRunTimer(float timer) { this->runTimer  = timer; }
    void SubRunTimer(float timer) { this->runTimer -= timer; }
    float GetRunTimer() { return runTimer; }

    // �U���֌W
    void SetAttackPower(float power)   { this->attackPower = power; }
    void SetAttackRadius(float radius) { this->attackRadius = radius; }
    float GetAttackRadius() { return attackRadius; }

    // �G���A�ړ�
    void  SetStayTime(float time) { this->stayTime = time; }
    float GetStayTime()           { return stayTime; }

    // �G�[�W�F���g
    void SetAgent(std::shared_ptr<NavMeshAgent> agent, UINT id) { this->agent = agent; this->agentID = id; }
    std::shared_ptr<NavMeshAgent> GetAgent() { return agent; }
    UINT GetAgentID() { return agentID; }

    // �{��
    float GetMagnification() { return magnification; }  // �U���{��
    float GetDefenceRate()   { return defenseRate; }    // �h��{��

    // ��Ԃ̐ݒ�
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
    // �Q�O�I�u�W�F�N�g
    std::shared_ptr<NavMeshAgent> agent;
    UINT agentID = 0;

#pragma region �G�̃X�e�[�^�X
    Character* target = nullptr;

    DirectX::XMFLOAT3 targetPosition  = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3 territoryOrigin = { 0.0f,0.0f,0.0f };

    float territoryRange = 20.0f; 
    float searchRange    = 20.0f;
    float attackRange    = 15.0f;

    float moveSpeed = 12.0f;
    float turnSpeed = DirectX::XMConvertToRadians(360);
        
    float attackRadius = 3.0f;  // �U���͈�
    float attackPower  = 1.0f;  // �U����
    float runTimer     = 0.0f;

    bool notFlinch     = false;  // �Ђ�܂Ȃ�

public:
    // ���
    UINT condition = 0;
    // ���ʔj��
    UINT clackFlg  = 0;

protected:
    // �G���A�̑؍ݎ���
    // 0�ɂȂ�ƃG���A�ړ�
    float stayTime = 0.0f;

    // �U���̔{�� : �{�莞���Əオ��
    float magnification = 1.0f;
    float defenseRate   = 0.1f;
#pragma endregion
public:
    HitInformation hitinformation;
};