#include "Hammer.h"
#include "GameObject/GameObjectManager.h"

void Hammer::Start()
{
    std::shared_ptr<GameObject> object = gameObject.lock();

    object->transform.SetPosition({ 0, 2, 2 });
    object->transform.SetAngleZ(180);
}

void Hammer::Update()
{
    std::shared_ptr<GameObject> object = gameObject.lock();

    // �g�����X�t�H�[���v�Z
    ComputeTransform();

    // ���f���̃g�����X�t�H�[���v�Z
    object->GetModel()->UpdateTransform(object->transform.GetTransform());

}

void Hammer::ComputeTransform()
{
    std::shared_ptr<GameObject> object = gameObject.lock();
    auto player = GameObjectManager::Instance().Find("Otomo");

    // �ڒ��������m�[�h�̃��[���h�s����擾
    int nodeIndex = player->GetModel()->FindNodeIndex("Character1_RightHand");
    DirectX::XMFLOAT4X4 world = player->GetModel()->GetNodes().at(nodeIndex).worldTransform;
    DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&world);

    // ���̃��[�J���s��
    DirectX::XMFLOAT3 scale     = object->GetTransform().GetScale();
    DirectX::XMFLOAT3 angle     = object->GetTransform().GetAngle();
    DirectX::XMFLOAT3 translate = object->GetTransform().GetPosition();

    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // ��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);

    DirectX::XMMATRIX L = S * R * T;

    // ���̃g�����X�t�H�[���v�Z
    DirectX::XMStoreFloat4x4(&object->transform.m_transform, DirectX::XMMatrixMultiply(L, W));
}
