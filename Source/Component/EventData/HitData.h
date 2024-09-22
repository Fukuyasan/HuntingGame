#pragma once
#include "System/Collision/Collision.h"
#include "Graphics/Model/Model.h"

#include <map>

// �q�b�g���
class HitInformation
{
public:
    // ���ʔj��
    enum ClackPart
    {
        Non      = 0,         // ����
        Head     = (1 << 0),  // ��
        LeftArm  = (1 << 1),  // ���r
        RightArm = (1 << 2),  // �E�r
        Belly    = (1 << 3)   // ��
    };

    // ����
    enum class Part
    {
        Head,   // ��
        Neck,   // ��
        L_Arm,  // ���r
        R_Arm,  // �E�r
        Torso,  // ����
        R_Leg,  // �E��
        L_Leg,  // ����
        Tail    // �K��
    };

public:
    // ��炢����f�[�^
    struct HitData
    {
        DirectX::XMFLOAT3 position;  // ����ʒu(���[�J�����W)
        float             radius;    // ���蔼�a
        std::string       nodeName;  // �m�[�h��(���[���h���W�̊�ɂȂ�)
        int               hitPart;   // �q�b�g�������ʂ̔ԍ�
        template<class Archive>
        void serialize(Archive& _archive);
    };

    // ���ʃf�[�^
    struct PartData
    {
        int hitZone;                     // ����
        int durability;                  // �ϋv�l
        int clackPart = ClackPart::Non;  // ���� (���ʔj��p)
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