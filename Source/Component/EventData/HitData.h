#pragma once
#include "System/Collision/Collision.h"
#include "Graphics/Model/Model.h"

#include <map>

// ヒット情報
class HitInformation
{
public:
    // 部位破壊
    enum ClackPart
    {
        Non      = 0,         // 無し
        Head     = (1 << 0),  // 頭
        LeftArm  = (1 << 1),  // 左腕
        RightArm = (1 << 2),  // 右腕
        Belly    = (1 << 3)   // 腹
    };

    // 部位
    enum class Part
    {
        Head,   // 頭
        Neck,   // 首
        L_Arm,  // 左腕
        R_Arm,  // 右腕
        Torso,  // 胴体
        R_Leg,  // 右足
        L_Leg,  // 左足
        Tail    // 尻尾
    };

public:
    // 喰らい判定データ
    struct HitData
    {
        DirectX::XMFLOAT3 position;  // 判定位置(ローカル座標)
        float             radius;    // 判定半径
        std::string       nodeName;  // ノード名(ワールド座標の基準になる)
        int               hitPart;   // ヒットした部位の番号
        template<class Archive>
        void serialize(Archive& _archive);
    };

    // 部位データ
    struct PartData
    {
        int hitZone;                     // 肉質
        int durability;                  // 耐久値
        int clackPart = ClackPart::Non;  // 部位 (部位破壊用)
        template<class Archive>
        void serialize(Archive& _archive);
    };

public:
    void DrawDebugGUI();
    void EditHitData(Model* model);
    void DrawNodeGUI(Model::Node* node);

    void Decerialize(const char* filepath);
    void Serialize(const char* filepath);

public:
	std::vector<HitData>     hitDatas;
    std::map<int, PartData>  partDatas;

private:
    Model::Node* selectNode;
    char setNodeName;
};