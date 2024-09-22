#pragma once
#include "Graphics/Model/Model.h"
#include "Stage.h"

// ステージのためのベクター
// ※ unityから持ってきた情報はDirectXだと使用できないから
struct Vec3
{
    float x;
    float y;
    float z;
};

class StageMain : public Stage
{
public:
    // ステージのパラメータ
    struct StageParam
    {
        std::string name;
        Vec3 position;
        Vec3 rotation;
        Vec3 scale;

        template<class Archive>
        void serialize(Archive& archive);
    };

    // ステージのデータ
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

    // Jsonにあるステージ情報をロード
    void LoadStageObjects(const char* filename);

    // ステージの情報
    void LoadAreaData();

    // レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end, HitResult& hit) override;

    // デバッグ表示
    void OnDebugGUI() override;
    void OnDebugPrimitive() override;

    void EditAreaData();
    void EditCollisionData();

private:
    std::vector<StageParam> stageParams;

    // デバッグ
    std::vector<DirectX::XMFLOAT3> m_modelPositions;

    int deleteArea = -1;
};
