#pragma once

#include "Graphics/Model/Model.h"
#include "Projectile.h"

struct StraiteParam
{
    DirectX::XMFLOAT3 direction;
    DirectX::XMFLOAT3 position;
    float             speed;
    float             radius;
    Type              type;
};

// ���i�e��
class ProjectileStraite : public Projectile
{
public:
    ProjectileStraite(std::shared_ptr<ModelResource> resource);  // �R���X�g���N�^
    ~ProjectileStraite()override = default;  // �f�X�g���N�^

    // �X�V����
    void Update(const float& elapsedTime) override;
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // ����
    void Launch(const StraiteParam& param);

    // GUI�`��
    void DrawDebugGUI();

private:
    const float gravity = 0.15f;
};