#pragma once
#include <string>
#include <vector>

// �X�e�[�W�̂��߂̃x�N�^�[
// �� unity���玝���Ă�������DirectX���Ǝg�p�ł��Ȃ�����
struct Vec3
{
    float x;
    float y;
    float z;
};

class StageManager
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
    StageManager() {}
    ~StageManager() {}

    static StageManager& Instance()
    {
        static StageManager instance;
        return instance;
    }

    // Json�ɂ���X�e�[�W�������[�h
    void LoadStageObjects(const char* filename);

public:
    //std::vector<std::string> namePool;
};