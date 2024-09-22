#pragma once

#include "Scene.h"

// シーンマネージャー
class SceneManager
{
private:
    SceneManager( ) = default;  // コンストラクタ
    ~SceneManager() = default;  // デストラクタ

public:
    // 唯一のインスタンス取得
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    // 更新処理
    void Update(const float& elapsedTime);

    // 描画処理
    void Render();

    // シーンクリア
    void Clear();

    // ※ 管理しているシーンの
    //    終了処理を行う関数

    // シーン切り替え
    void ChangeScene(Scene* scene);

private:
    Scene* currentScene = nullptr;
    Scene* nextScene    = nullptr;
};