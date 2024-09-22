#pragma once

#include "Scene.h"
#include "UI/Button/UIButton.h"
#include "UI/NowLoading/NowLoading.h"

#include <thread>

// ローディングシーン
class SceneLoading : public Scene
{
public:
    // コンストラクタの初期化子リストで次のシーンを設定
    SceneLoading(Scene* nextScene) : nextScene(nextScene) {}  // コンストラクタ
    ~SceneLoading() override = default;                       // デストラクタ

    // 初期化
    void Initialize() override;

    // 終了化
    void Finalize() override;

    // 更新処理
    void Update(const float& elapsedTime) override;

    // 描画処理
    void Render() override;
    
private:
    // ローディングスレッド
    static void LoadingThread(SceneLoading* scene);

private:
    float angle    = 0.0f;

    Scene* nextScene    = nullptr;
    std::thread* thread = nullptr;

    // NOWLOADING ->10文字
    std::unique_ptr<NowLoading> nowLoading[10] = {};
};