#pragma once
#include <DirectXMath.h>

class Transform
{
public:
#pragma region トランスフォーム
    // 行列更新処理
    void UpdateTransform();

    // 位置
    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }
    void SetPosition(const DirectX::XMFLOAT3& position)
    { 
        this->m_position = position;
    }
    void SetPositionX(const float position) { this->m_position.x = position; }
    void SetPositionY(const float position) { this->m_position.y = position; }
    void SetPositionZ(const float position) { this->m_position.z = position; }

    void AddPositionX(const float position) { this->m_position.x = position; }
    void AddPositionY(const float position) { this->m_position.y = position; }
    void AddPositionZ(const float position) { this->m_position.z = position; }

    // 回転
    const DirectX::XMFLOAT3& GetAngle() const { return m_angle; }
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->m_angle = angle; }
    void SetAngleX(const float angle) { this->m_angle.x = angle; }
    void SetAngleY(const float angle) { this->m_angle.y = angle; }
    void SetAngleZ(const float angle) { this->m_angle.z = angle; }

    void AddAngleX(const float angle) { this->m_angle.x += angle; }
    void AddAngleY(const float angle) { this->m_angle.y += angle; }
    void AddAngleZ(const float angle) { this->m_angle.z += angle; }

    // スケール
    const DirectX::XMFLOAT3& GetScale() const { return m_scale; }
    void SetScale(const DirectX::XMFLOAT3& scale) { this->m_scale = scale; }
    void SetScale(const float scale) { this->m_scale.x = this->m_scale.y = this->m_scale.z = scale; }

    // 各ベクトル取得
    const DirectX::XMFLOAT3& GetForward() const { return m_forward; };
    const DirectX::XMFLOAT3& GetUp()      const { return m_up; };
    const DirectX::XMFLOAT3& GetRight()   const { return m_right; };

    // 行列
    const DirectX::XMFLOAT4X4& GetTransform() const { return m_transform; }
#pragma endregion

public:
#pragma region トランスフォーム
    DirectX::XMFLOAT3 m_position = { 0,0,0 };
    DirectX::XMFLOAT3 m_angle    = { 0,0,0 };
    DirectX::XMFLOAT3 m_scale    = { 1,1,1 };

    DirectX::XMFLOAT4X4 m_transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    DirectX::XMFLOAT3 m_forward = { 0,0,0 };
    DirectX::XMFLOAT3 m_up      = { 0,0,0 };
    DirectX::XMFLOAT3 m_right   = { 0,0,0 };
#pragma endregion
};