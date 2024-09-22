#pragma once

#include "Graphics/Model/Model.h"
#include "Graphics/shader.h"

enum class Type
{
    Player,
    Dragon
};

//�e��
class Projectile
{
public:
    Projectile()          = default;  // �R���X�g���N�^
    virtual ~Projectile() = default;  // �f�X�g���N�^

    // �X�V����
    virtual void Update(const float& elapsedTime) = 0;

    // �`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive();
    virtual void DrawDebugGUI() {}

    void SetScale(float _scale) { this->scale.x = this->scale.y = this->scale.z = _scale; }

    // �ʒu�擾
    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    // �����擾
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }
    // �X�P�[���擾
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // �j��
    void Destroy();

    // ����
    void EndLife(const float& elapsedTime);

    // ���a�擾
    float GetRadius() const { return radius; }

    // �^�C�v�擾
    Type GetType() const { return type; }

    // ���f���擾
    Model* GetModel() { return model.get(); }
protected:
    // �s��X�V����
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3 position    = { 0,0,0 };  // �ʒu
    DirectX::XMFLOAT3 direction   = { 0,0,1 };  // ����
    DirectX::XMFLOAT3 scale       = { 1,1,1 };  // �X�P�[��
    DirectX::XMFLOAT4X4 transform = {           // �s��    
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    float radius    = 0.5f;  // ���a
    float speed     = 1.0f;  // �X�s�[�h
    float lifeTimer = 3.0f;  // ����  
    Type  type;              // �^�C�v

    std::unique_ptr<Model> model = nullptr;
};