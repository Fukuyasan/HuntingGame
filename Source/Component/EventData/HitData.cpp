#include "HitData.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <types/vector.hpp>
#include <types/map.hpp>
#include <types/string.hpp>
#include <archives/json.hpp>

#include <imgui.h>

#include "Cereal/Cereal.h"

CEREAL_CLASS_VERSION(HitInformation::HitData,  1)
CEREAL_CLASS_VERSION(HitInformation::PartData, 1)

template<class Archive>
inline void HitInformation::HitData::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(position),
        CEREAL_NVP(radius),
        CEREAL_NVP(nodeName),
        CEREAL_NVP(hitPart)
    );
}

template<class Archive>
inline void HitInformation::PartData::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(hitZone),
        CEREAL_NVP(durability),
        CEREAL_NVP(clackPart)
    );
}

void HitInformation::DrawDebugGUI()
{     
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    // �m�[�h�̊K�w
    if (ImGui::Begin("Hitinformation", nullptr, ImGuiWindowFlags_None))
    {
        if (ImGui::CollapsingHeader("HitData"))
        {
            int i = 0;
            for (auto& hit : hitDatas)
            {
                std::string I        = std::to_string(i);
                std::string position = I + " : Position";
                std::string radius   = I + " : Radius";
                std::string part     = I + " : Part";

                ImGui::Text(hit.nodeName.c_str());
                ImGui::DragFloat3(position.c_str(), &hit.position.x, 0.1f, 0.0f, 1000.0f, "%.2f");
                ImGui::DragFloat(radius.c_str(), &hit.radius, 0.1f, 0.0f, 10.0f, "%.2f");
                ImGui::Combo(part.c_str(), &hit.hitPart, "Head\0R_Arm\0L_Arm\0Torso\0R_Leg\0L_Leg\0Tail");
                ++i;
            }
        }

        if (ImGui::CollapsingHeader("PartData"))
        {
            int i = 0;
            for (auto& part : partDatas)
            {
                int partNo             = part.first;
                std::string I          = std::to_string(partNo);
                std::string Part       = I + " : Part";
                std::string hitZone    = I + " : HitZone";
                std::string durability = I + " : Durability";
                std::string clackPart  = I + " : ClackPart";

                PartData& data = part.second;

                ImGui::Combo(Part.c_str(), &partNo, "Head\0R_Arm\0L_Arm\0Torso\0R_Leg\0L_Leg\0Tail");
                ImGui::DragInt(hitZone.c_str(), &data.hitZone, 1, 0, 100);
                ImGui::DragInt(durability.c_str(), &data.durability, 1, 0, 1000);
                ImGui::Combo(clackPart.c_str(), &data.clackPart, "Non\0Head\0LeftArm\0RightArm\0Belly");
            }
        }
    }
    ImGui::End();
}

void HitInformation::EditHitData(Model* model)
{
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    // �m�[�h�̊K�w
    if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_None))
    {
        for (auto& node : model->GetNodes())
        {
            if (node.parent == nullptr)
            {
                DrawNodeGUI(&node);
            }
        }
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    // �m�[�h�̊K�w
    if (ImGui::Begin("EditHitData", nullptr, ImGuiWindowFlags_None))
    {
        if (ImGui::Button("SetNode"))
        {
            HitData& hit = hitDatas.back();
            hit.nodeName = selectNode->name;
        }

        if (ImGui::Button("AddHitData"))
        {
            // ��炢����
            HitData& hit = hitDatas.emplace_back();
            if (selectNode) hit.nodeName = selectNode->name;
            hit.position                 = DirectX::XMFLOAT3(0, 0, 0);
            hit.radius                   = 1.0f; 
        }

        if (ImGui::Button("AddPushData"))
        {
            // ���ʏ��
            PartData part{};
            part.hitZone    = 45;
            part.durability = 300;
            part.clackPart  = ClackPart::Non;

            HitData& hit = hitDatas.back();
            partDatas[hit.hitPart] = part;
        }
        if (ImGui::Button("Delete"))
        {
            hitDatas.erase(hitDatas.end() - 1);
        }
    }
    ImGui::End();
}

void HitInformation::DrawNodeGUI(Model::Node* node)
{
    // �����N���b�N�A�܂��̓m�[�h���_�u���N���b�N�ŊK�w���J��
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // �I���t���O
    if (selectNode == node)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }
    // �q�����Ȃ��ꍇ�͖������Ȃ�
    size_t child_count = node->children.size();
    if (child_count <= 0)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // �c���[�m�[�h��\��
    bool opened = ImGui::TreeNodeEx(node, nodeFlags, node->name);

    // �A�N�e�B�u�����ꂽ�m�[�h�͑I������
    if (ImGui::IsItemClicked())
    {
        selectNode = node;
    }

    // �J����Ă���ꍇ�A�q�K�w�������������s��
    if (opened && child_count > 0)
    {
        for (Model::Node* child : node->children)
        {
            DrawNodeGUI(child);
        }
        ImGui::TreePop();
    }
}

void HitInformation::Decerialize(const char* filepath)
{
    {
        // �t�@�C���p�X�擾
        std::string Filepath    = filepath;
        std::string Filename    = "HitData.json";
        std::string hitDataFile = filepath + Filename;

        // �t�@�C���ǂݍ���
        std::ifstream ifs(hitDataFile.c_str(), std::ios::in);
        cereal::JSONInputArchive archive(ifs);

        if (ifs.is_open())
        {
            archive(
                CEREAL_NVP(hitDatas)
            );
        }
    }

    {
        // �t�@�C���p�X�擾
        std::string Filepath     = filepath;
        std::string Filename     = "PartData.json";
        std::string partDataFile = filepath + Filename;

        // �t�@�C���ǂݍ���
        std::ifstream ifs(partDataFile.c_str(), std::ios::in);
        cereal::JSONInputArchive archive(ifs);

        if (ifs.is_open())
        {
            archive(
                CEREAL_NVP(partDatas)
            );
        }
    }
}

void HitInformation::Serialize(const char* filepath)
{
    {
        // �t�@�C���p�X�擾
        std::string Filepath    = filepath;
        std::string Filename    = "HitData.json";
        std::string hitDataFile = filepath + Filename;

        // �t�@�C���o��
        std::ofstream ofs(hitDataFile.c_str(), std::ios::out);
        cereal::JSONOutputArchive archive(ofs);

        if (ofs.is_open())
        {
            archive(
                CEREAL_NVP(hitDatas)
            );
        }
    }

    {
        // �t�@�C���p�X�擾
        std::string Filepath     = filepath;
        std::string Filename     = "PartData.json";
        std::string partDataFile = filepath + Filename;

        // �t�@�C���o��
        std::ofstream ofs(partDataFile, std::ios::out);
        cereal::JSONOutputArchive archive(ofs);

        if (ofs.is_open())
        {
            archive(
                CEREAL_NVP(partDatas)
            );
        }
    }
}
