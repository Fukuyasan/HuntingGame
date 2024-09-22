#pragma once
#include "Graphics/Model/Model.h"
#include "Stage.h"

// �X�e�[�W�̂��߂̃x�N�^�[
// �� unity���玝���Ă�������DirectX���Ǝg�p�ł��Ȃ�����
struct Vec3
{
    float x;
    float y;
    float z;
};

class StageMain : public Stage
{
public:
    // �X�e�[�W�̃p�����[�^
    struct StageParam
    {
        std::string name;
        Vec3 position;
        Vec3 rotation;
        Vec3 scale;

        template<class Archive>
        void serialize(Archive& archive);
    };

    // �X�e�[�W�̃f�[�^
    struct StageData
    {
        std::vector<StageParam> stageParam;

        template<class Archive>
        void serialize(Archive& archive);
    };

public:
    StageMain() = default;
    ~StageMain() override;

    void Awake() override;
    void Start() override;
    void Update() override;
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    const char* GetName() const override { return "Stage"; }

    // Json�ɂ���X�e�[�W�������[�h
    void LoadStageObjects(const char* filename);

    // �X�e�[�W�̏��
    void LoadAreaData();

    // ���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end, HitResult& hit) override;

    // �f�o�b�O�\��
    void OnDebugGUI() override;
    void OnDebugPrimitive() override;

    void EditAreaData();
    void EditCollisionData();

private:
    std::vector<StageParam> stageParams;

    // �f�o�b�O
    std::vector<DirectX::XMFLOAT3> m_modelPositions;

    int deleteArea = -1;
};
