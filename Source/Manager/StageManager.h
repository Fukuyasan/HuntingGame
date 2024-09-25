#pragma once
#include <string>
#include <vector>

// ステージのためのベクター
// ※ unityから持ってきた情報はDirectXだと使用できないから
struct Vec3
{
    float x;
    float y;
    float z;
};

class StageManager
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
    StageManager() {}
    ~StageManager() {}

    static StageManager& Instance()
    {
        static StageManager instance;
        return instance;
    }

    // Jsonにあるステージ情報をロード
    void LoadStageObjects(const char* filename);

public:
    //std::vector<std::string> namePool;
};