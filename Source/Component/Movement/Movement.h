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
    // 移動処理
    void Move(float vx, float vz, float speed);
    void MoveDirection(const DirectX::XMFLOAT3& direction, float speed);

    // 旋回処理
    void Turn(const float& elapsedTime, float vx, float vz, float speed);

    // 吹き飛ばす
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

public:
    void UpdateVelocity(const float& elapsedTime);

private:
    // 垂直速力、垂直移動
    void UpdateVerticalVelocity(float elapsedFrame);
    void UpdateVerticalMove(float elapsedTime);

    // 水平速力、水平移動
    void UpdateHorizontalVelocity(float elapsedFrame);
    void UpdateHorizontalMove(float elapsedTime);

public:
    void SetMaxMoveSpeed(float speed) { m_maxMoveSpeed = speed; }
    float GetMaxMoveSpeed() { return m_maxMoveSpeed; }

    // 地面に接触しているか
    bool IsGround()  const { return m_isGround; }

public:
    // 着地しているかどうか
    bool m_isGround  = false;  // してる : true, してない : false

    DirectX::XMFLOAT3 m_velocity = { 0,0,0 };
    float m_gravity              = -1.0f;
    float m_friction             = 3.0f;
    float m_acceleration         = 6.0f;
    float m_maxMoveSpeed         = 20.0f;
    
    // 進行方向
    float m_moveVecX = 0.0f;
    float m_moveVecZ = 0.0f;

    float m_airControl = 0.1f;

    // レイキャスト用
    float m_stepOffset = 1.0f;
    float m_slopeRate  = 1.0f;
};