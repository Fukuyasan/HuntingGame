#pragma once
#include "Graphics/Shader.h"
#include "System/Collision/Collision.h"
#include "Component/Component.h"

class Stage : public Component
{
public:
    // �G���A�̃p�����[�^
    struct AreaData
    {
        int areaNum;
        DirectX::XMFLOAT3 areaPos;
        float areaRange;

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct CollisionData
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 size;

        template<class Archive>
        void serialize(Archive& archive);
    };
public:
    Stage()          = default;
    virtual ~Stage() = default;

    // ���C�L���X�g
    virtual bool RayCast(const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

    // �t�@�C������f�[�^�����[�h
    //LoadDataFromFile(const char* filename)

public:
    // ���f���擾
    Model GetModel(int index) { return *models.at(index); }

    const std::vector<AreaData> GetAreaData() { return areaDatas; }
    const std::vector<CollisionData> GetCollisionData() { return collisionDatas; }

protected:
    std::vector<std::unique_ptr<Model>> models;
    
    std::vector<AreaData> areaDatas;
    std::vector<CollisionData> collisionDatas;
};
