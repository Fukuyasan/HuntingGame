#pragma once
#include <DirectXMath.h>
#include "Component/Component.h"

class Movement : public Component
{
public:
    Movement() {}
    virtual ~Movement() {}

    const char* GetName() const override { return "Movement"; }

public:
    // �ړ�����
    void Move(float vx, float vz, float speed);
    void MoveDirection(const DirectX::XMFLOAT3& direction, float speed);

    // ���񏈗�
    void Turn(const float& elapsedTime, float vx, float vz, float speed);

    // ������΂�
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

public:
    void UpdateVelocity(const float& elapsedTime);

private:
    // �������́A�����ړ�
    void UpdateVerticalVelocity(float elapsedFrame);
    void UpdateVerticalMove(float elapsedTime);

    // �������́A�����ړ�
    void UpdateHorizontalVelocity(float elapsedFrame);
    void UpdateHorizontalMove(float elapsedTime);

public:
    void SetMaxMoveSpeed(float speed) { m_maxMoveSpeed = speed; }
    float GetMaxMoveSpeed() { return m_maxMoveSpeed; }

    // �n�ʂɐڐG���Ă��邩
    bool IsGround()  const { return m_isGround; }

public:
    // ���n���Ă��邩�ǂ���
    bool m_isGround  = false;  // ���Ă� : true, ���ĂȂ� : false

    DirectX::XMFLOAT3 m_velocity = { 0,0,0 };
    float m_gravity              = -1.0f;
    float m_friction             = 3.0f;
    float m_acceleration         = 6.0f;
    float m_maxMoveSpeed         = 20.0f;
    
    // �i�s����
    float m_moveVecX = 0.0f;
    float m_moveVecZ = 0.0f;

    float m_airControl = 0.1f;

    // ���C�L���X�g�p
    float m_stepOffset = 1.0f;
    float m_slopeRate  = 1.0f;
};