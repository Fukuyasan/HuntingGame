#include "Sword.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Graphics/ShaderState.h"
#include "GameObject/GameObjectManager.h"

#include "Camera/Camera.h"


#define TRAIL_FLG 1

void Sword::Start()
{
    std::shared_ptr<GameObject> object = gameObject.lock();

    // �p�x�ݒ�
    angle.y = DirectX::XMConvertToRadians(90);
    angle.z = DirectX::XMConvertToRadians(90);

    // ���̓����蔻��ʒu�擾
    swordNodeIndex[0] = object->GetModel()->FindNodeIndex("Ancient__Warrior_Blade");
    swordNodeIndex[1] = object->GetModel()->FindNodeIndex("GameObject");

#if TRAIL_FLG
    // �g���C��
    trail = std::make_unique<TrailShader>("Data/Sprite/Trail.png");
#endif
}

void Sword::Update()
{
    std::shared_ptr<GameObject> object = gameObject.lock();

    // �g�����X�t�H�[���v�Z
	ComputeTransform();

    // ���f���̃g�����X�t�H�[���v�Z
    object->GetModel()->UpdateTransform(transform);

#if TRAIL_FLG
    SetTrail();
    trail->Update();
#endif
}

void Sword::TrailRender()
{
#if TRAIL_FLG
    if (!isTrail) return;
    
    Camera& camera = Camera::Instance();
    trail->Render(&camera.GetView(), &camera.GetProjection(), 0);
#endif
}

void Sword::ComputeTransform()
{
    auto parent = gameObject.lock()->m_parent;

	// �ڒ��������m�[�h�̃��[���h�s����擾
    int nodeIndex             = parent.lock()->GetModel()->FindNodeIndex("mixamorig:RightHand");
	DirectX::XMFLOAT4X4 world = parent.lock()->GetModel()->GetNodes().at(nodeIndex).worldTransform;
	DirectX::XMMATRIX W       = DirectX::XMLoadFloat4x4(&world);

	// ���̃��[�J���s��
    
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // ��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);

    DirectX::XMMATRIX L = S * R * T;

	// ���̃g�����X�t�H�[���v�Z
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixMultiply(L, W));
}

void Sword::OnDebugGUI()
{ 
    if (ImGui::CollapsingHeader("Sword"))
    {
        ImGui::InputFloat3("Scale", &scale.x);

        // ��]
        DirectX::XMFLOAT3 a;
        a.x = DirectX::XMConvertToDegrees(angle.x);
        a.y = DirectX::XMConvertToDegrees(angle.y);
        a.z = DirectX::XMConvertToDegrees(angle.z);
        ImGui::SliderFloat3("Angle", &a.x, -180, 180);
        angle.x = DirectX::XMConvertToRadians(a.x);
        angle.y = DirectX::XMConvertToRadians(a.y);
        angle.z = DirectX::XMConvertToRadians(a.z);

        ImGui::DragFloat3("Translate", &translate.x, 0.01f, 0.0f, 10.0f, "%.3f");

        ImGui::DragFloat("root", &trailOffset[0], 0.01f, -200.0f, 200.0f, "%.3f");
        ImGui::DragFloat("tip",  &trailOffset[1], 0.01f, -200.0f, 200.0f, "%.3f");

        ImGui::Checkbox("Trail", &isTrail);

    }
}

void Sword::OnDebugPrimitive()
{
    DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();
#if 0

    const Model::Node& startNode = model->GetNodes().at(swordNodeIndex[0]);
    const Model::Node& goalNode  = model->GetNodes().at(swordNodeIndex[1]);

    DirectX::XMFLOAT3 swordStartPos = GameMath::GetPositionFrom4x4(startNode.worldTransform);
    DirectX::XMFLOAT3 swordGoalPos  = GameMath::GetPositionFrom4x4(goalNode.worldTransform);
    // �J�v�Z���`��
    debugRenderer->AddCapsule(
        swordStartPos,
        swordGoalPos,
        radius,
        DirectX::XMFLOAT4(1, 0, 0, 1)
    );
#else

#endif
}

void Sword::SetTrail()
{
    DirectX::XMFLOAT3 root, tip;

    DirectX::XMMATRIX T    = DirectX::XMLoadFloat4x4(&transform);
    DirectX::XMVECTOR Root = DirectX::XMVectorSet(0, 0, trailOffset[0], 0);
    DirectX::XMVECTOR Tip  = DirectX::XMVectorSet(0, 0, trailOffset[1], 0);

    Root = DirectX::XMVector3Transform(Root, T);
    Tip  = DirectX::XMVector3Transform(Tip, T);

    DirectX::XMStoreFloat3(&root, Root);
    DirectX::XMStoreFloat3(&tip, Tip);

    trail->Set(root, tip);
}
