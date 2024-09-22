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

    // トランスフォーム計算
    ComputeTransform();

    // モデルのトランスフォーム計算
    object->GetModel()->UpdateTransform(object->transform.GetTransform());

}

void Hammer::ComputeTransform()
{
    std::shared_ptr<GameObject> object = gameObject.lock();
    auto player = GameObjectManager::Instance().Find("Otomo");

    // 接着したいノードのワールド行列を取得
    int nodeIndex = player->GetModel()->FindNodeIndex("Character1_RightHand");
    DirectX::XMFLOAT4X4 world = player->GetModel()->GetNodes().at(nodeIndex).worldTransform;
    DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&world);

    // 剣のローカル行列
    DirectX::XMFLOAT3 scale     = object->GetTransform().GetScale();
    DirectX::XMFLOAT3 angle     = object->GetTransform().GetAngle();
    DirectX::XMFLOAT3 translate = object->GetTransform().GetPosition();

    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // 回転行列を作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);

    DirectX::XMMATRIX L = S * R * T;

    // 剣のトランスフォーム計算
    DirectX::XMStoreFloat4x4(&object->transform.m_transform, DirectX::XMMatrixMultiply(L, W));
}
