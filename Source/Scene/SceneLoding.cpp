#include "Graphics/Graphics.h"
#include "Graphics/RenderState.h"
#include "Graphics/ShaderState.h"
#include "System/Input/Input.h"
#include "SceneLoding.h"
#include "SceneManager.h"

#include "UI/UIManager.h"

// 初期化
void SceneLoading::Initialize()
{
    // スレッド開始   
    thread = new std::thread(LoadingThread, this);

    for (int i = 0; i < _countof(nowLoading); ++i)
    {
        nowLoading[i] = std::make_unique<NowLoading>(i, 0.1f * (i + 1));
    }
}

// 終了化
void SceneLoading::Finalize()
{
    // スレッド終了化
    if (thread != nullptr)
    {
        thread->join();

        delete thread;
        thread = nullptr;
    }
}

// 更新処理
void SceneLoading::Update(const float& elapsedTime)
{
    // NOWLOADINGの更新処理
    for (int i = 0; i < _countof(nowLoading); ++i)
    {
        nowLoading[i]->Update(elapsedTime);
    }

    // 次のシーンの準備が完了したらシーンを切り替える
    if (nextScene->IsReady())
    {
        SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
    }
}

// 描画処理
void SceneLoading::Render()
{
    Graphics& graphics          = Graphics::Instance();
    RenderState& bs             = RenderState::Instance();
    ShaderState& ss             = ShaderState::Instance();
    ID3D11DeviceContext* dc     = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };  // RGBA(0.0f～1.0f)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // ブレンド設定
    bs.SetBlend(dc, BLEND_MODE::ALPHA);
    bs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

    // サンプラーステート設定
    bs.SetPSSampler(dc, 0, SAMPLER_MODE::POINT);
    bs.SetPSSampler(dc, 1, SAMPLER_MODE::LINEAR);
    bs.SetPSSampler(dc, 2, SAMPLER_MODE::ANISOTROPIC);
    bs.SetPSSampler(dc, 3, SAMPLER_MODE::LINER_BORDER_BLACK);
    bs.SetPSSampler(dc, 4, SAMPLER_MODE::LINER_BORDER_WHITE);
    bs.SetPSSampler(dc, 5, SAMPLER_MODE::LINEAR_MIRROR);

    SpriteShader* shader = ss.GetSpriteShader(SPRITE::Default);
    shader->Begin(dc);

    // NOWLOADING描画
    for (int i = 0; i < _countof(nowLoading); ++i)
    {
        nowLoading[i]->Render(dc, shader);
    }

    shader->End(dc);
}

// ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    // COM関連の初期化でスレッド毎に呼ぶ必要がある
    CoInitialize(nullptr);

    // 次のシーンの初期化を行う
    scene->nextScene->Initialize();    
    
    // スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    // 次のシーンの準備完了設定
    scene->nextScene->SetReady();
}
