#include "Transform.h"

void Transform::UpdateTransform()
{
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
        m_scale.x,
        m_scale.y,
        m_scale.z
    );

    // ��]�s����쐬
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(m_angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(m_angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(m_angle.z);
    
    // R = Roll * Pitch * yaw
    DirectX::XMMATRIX R = Y * Z * X;

    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
        m_position.x,
        m_position.y,
        m_position.z
    );

    // 3�̍s���g�ݍ��킹�A���[���h�s����쐬    
    DirectX::XMMATRIX W = S * R * T;  // �����̏��Ԃ�

    // �v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&m_transform, W);  

    // �O�x�N�g��
    m_forward.x = sinf(m_angle.y);
    m_forward.y = 0;
    m_forward.z = cosf(m_angle.y);

    // ��x�N�g��
    m_up.x = m_transform._21;
    m_up.y = m_transform._22;
    m_up.z = m_transform._23;
    
    // �E�x�N�g��
    m_right.x = m_transform._11;
    m_right.y = m_transform._12;
    m_right.z = m_transform._13;
}