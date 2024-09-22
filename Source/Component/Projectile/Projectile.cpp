#include "Projectile.h"
#include "ProjectileManager.h"

#include "Graphics/ShaderState.h"

// �f�o�b�O�v���~�e�B�u�`��
void Projectile::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�O����`��
    debugRenderer->AddSphere(
        position,
        radius,
        DirectX::XMFLOAT4(0, 0, 0, 1)
    );  
}

// �s��X�V����
void Projectile::UpdateTransform()
{
    DirectX::XMVECTOR Front, Up, Right;
    // �O�x�N�g�����Z�o
    Front = DirectX::XMLoadFloat3(&direction);

    // ���̏�x�N�g�����Z�o
    DirectX::XMFLOAT3 provisionalUp = { 0,1,0 };
    Up = DirectX::XMLoadFloat3(&provisionalUp);

    // �E�x�N�g�����Z�o(�O��)
    Right = DirectX::XMVector3Cross(Up, Front);

    // ��x�N�g�����Z�o(�O��)
    Up = DirectX::XMVector3Cross(Front, Right);

    // ���K��
    Front = DirectX::XMVector3Normalize(Front);
    Up    = DirectX::XMVector3Normalize(Up);
    Right = DirectX::XMVector3Normalize(Right);

    // scale��������
    Front = DirectX::XMVectorScale(Front,scale.z);
    Up    = DirectX::XMVectorScale(Up, scale.y);
    Right = DirectX::XMVectorScale(Right, scale.x);

    // �v�Z���ʂ����o��
    DirectX::XMFLOAT3 front, up, right;
    DirectX::XMStoreFloat3(&front, Front);  // �O����
    DirectX::XMStoreFloat3(&up, Up);        // �����
    DirectX::XMStoreFloat3(&right, Right);  // �E����

    // �Z�o�������x�N�g������s����쐬 
    // x��   
    transform._11 = right.x;
    transform._12 = right.y;
    transform._13 = right.z;
    transform._14 = 0.0f;
    // y��
    transform._21 = up.x;
    transform._22 = up.y;
    transform._23 = up.z;
    transform._24 = 0.0f;
    // z��
    transform._31 = front.x;
    transform._32 = front.y;
    transform._33 = front.z;
    transform._34 = 0.0f;
    // �ʒu
    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    // ���˕���
    this->direction = front;
}

// �j��
void Projectile::Destroy()
{
    // �}�l�[�W���[���玩����j������
    ProjectileManager::Instance().Remove(this);
}

// ��������
void Projectile::EndLife(const float& elapsedTime)
{
    // �������s�����玩���͔j������B
    lifeTimer -= elapsedTime;

   // �������폜 
   if (lifeTimer < 0) Destroy();    
}
