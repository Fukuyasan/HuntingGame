#include <imgui.h>
#include "ProjectileStraite.h"

ProjectileStraite::ProjectileStraite(std::shared_ptr<ModelResource> resource)
{
    model = std::make_unique<Model>(resource);
    SetScale(0.015f);
}

// �X�V����
void ProjectileStraite::Update(const float& elapsedTime)
{
   // ��������
    EndLife(elapsedTime);

    // �ړ�
    float Speed = this->speed * elapsedTime;

    // ���������߂�
    DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
    dir = DirectX::XMVector3Normalize(dir);
    DirectX::XMStoreFloat3(&direction, dir);

    position.x += direction.x * Speed;
    position.y += direction.y * Speed;
    position.z += direction.z * Speed;

    // �d�͂�������
    //direction.y -= m_gravity * elapsedTime;

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    model->UpdateTransform(transform);
}

void ProjectileStraite::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model.get());
}

void ProjectileStraite::Launch(const StraiteParam& param)
{
    this->direction = param.direction;
    this->position  = param.position;
    this->speed     = param.speed;
    this->radius    = param.radius;
    this->type      = param.type;
}

void ProjectileStraite::DrawDebugGUI()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // �ʒu
        ImGui::InputFloat("speed", &speed);
    }
}
