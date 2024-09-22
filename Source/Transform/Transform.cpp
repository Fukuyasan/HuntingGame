#include "Transform.h"

void Transform::UpdateTransform()
{
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
        m_scale.x,
        m_scale.y,
        m_scale.z
    );

    // 回転行列を作成
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(m_angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(m_angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(m_angle.z);
    
    // R = Roll * Pitch * yaw
    DirectX::XMMATRIX R = Y * Z * X;

    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
        m_position.x,
        m_position.y,
        m_position.z
    );

    // 3つの行列を組み合わせ、ワールド行列を作成    
    DirectX::XMMATRIX W = S * R * T;  // ※この順番で

    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&m_transform, W);  

    // 前ベクトル
    m_forward.x = sinf(m_angle.y);
    m_forward.y = 0;
    m_forward.z = cosf(m_angle.y);

    // 上ベクトル
    m_up.x = m_transform._21;
    m_up.y = m_transform._22;
    m_up.z = m_transform._23;
    
    // 右ベクトル
    m_right.x = m_transform._11;
    m_right.y = m_transform._12;
    m_right.z = m_transform._13;
}