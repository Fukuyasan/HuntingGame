#include "Graphics/Graphics.h"
#include "Graphics/RenderState.h"
#include "Graphics/ShaderState.h"
#include "Graphics/CreateShader.h"

#include "SceneTitle.h"
#include "SceneLoding.h"
#include "SceneGame.h"
#include "SceneManager.h"

#include "System/Input/Input.h"

#include "Component/Stage/StageMain.h"

#include "Camera/Camera.h"
#include "Graphics/PostEffect.h"

#include "UI/Fade/Fade.h"

#include "Component/Character/Enemy/Stego/Stego.h"
#include "Component/Character/Enemy/EnemyManager.h"
#include "AI/MetaAI/MetaAI.h"

#include "System/Effect/EffectManager.h"

#include <imgui.h>

#define DEBUG_FLG 0

#define USEPOSTEFFECT 0

#ifdef UseTitle

// 初期化
void SceneTitle::Initialize()
{
    Graphics&     graphics     = Graphics::Instance();
    StageManager& stageManager = StageManager::Instance();
    EnemyManager& enemyManager = EnemyManager::Instance();
    UIManager&    uiManager    = UIManager::Instance();

#pragma region ステージ
    StageMain* stage = new StageMain();
    stageManager.Register(stage);
#pragma endregion

#pragma region カメラ
    Camera& camera = Camera::Instance();
    camera.SetLookAt(
        eye,
        focus,
        up
    );
    camera.SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),
        graphics.GetScreenWidth() / graphics.GetScreenHeight(),
        0.1f,
        10000.0f
    );

    //controller = std::make_unique<CameraController>(true);
#pragma endregion

#pragma region MetaAI
    MetaAI::Instance().Initialize();
#pragma endregion

#pragma region ナビメッシュ
    // ナビメッシュ初期化
    navMesh = std::make_unique<SoloMesh>("navMeshData.bin");

    // エージェント初期化
    agent = std::make_unique<NavMeshAgent>(navMesh.get());
#pragma endregion

#pragma region 草食動物
    const int stegoCount = 16;
    for (int i = 0; i < stegoCount; ++i)
    {
        Stego* stego = new Stego();

        // 円状に出現させる
        float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
        float range = i;

        stego->transform.SetPositionX(stego->transform.GetPosition().x + sinf(theta) * range);
        stego->transform.SetPositionY(stego->transform.GetPosition().y);
        stego->transform.SetPositionZ(stego->transform.GetPosition().z + cosf(theta) * range);

        // エージェント登録
        AgentParams ap{};
        ap.height = 5.0f;
        ap.radius = 2.0f;
        ap.maxSpeed = 50.0f;
        ap.maxAcceleration = ap.maxSpeed;
        agent->AddAgent(stego->transform.GetPosition(), ap);

        // エージェントセット
        stego->SetAgent(agent, i);

        enemyManager.Register(stego);
    }
#pragma endregion

#pragma region タイトルロゴ   
    const char* titleTex[] = {
       "Data/Sprite/TitleLogo.DDS",
       "Data/Sprite/key.DDS",
    };

    titleLogo = std::make_unique<UIButton>(titleTex[0]);
    titleLogo->SetPosition({ 1920 / 2 - 864 / 2, 1080 / 10 });
    uiManager.RegisterUI(titleLogo.get());


    key = std::make_unique<UIButton>(titleTex[1]);
    key->SetPosition({ 1920 / 2 - 640 / 2, 1080 - 265 });
    uiManager.RegisterUI(key.get());

#pragma endregion

    // シャドウマップ
    dsvShadowmap = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);

    // スカイマップテクスチャ
    skymap = std::make_unique<Sprite>("Data/Sprite/drakensberg_solitary_mountain_4k.DDS");

    const char* maskTex[] = {
        "Data/Sprite/Dissolve/dissolve_animation.png",
        "Data/Sprite/Dissolve/Dissolve_01_01.png",
        "Data/Sprite/Dissolve/Dissolve_01_04.png",
        "Data/Sprite/Dissolve/Dissolve_01_07.png",
        "Data/Sprite/Dissolve/Dissolve_01_08.png",
        "Data/Sprite/Dissolve/Dissolve_01_11.png",
        "Data/Sprite/Dissolve/Dissolve_01_14.png",
        "Data/Sprite/Dissolve/Dissolve_01_19.png",
    };

    Texture::LoadTexture(maskTex[1], graphics.GetDevice(), maskSRV.GetAddressOf());

    // オフスクリーン
    bitBlockTransfer = std::make_unique<FullscreeQuad>(graphics.GetDevice());

    PostEffect::Instance().Initialize();

    // 定数バッファ
    CreateShader::CreateCB(graphics.GetDevice(), sizeof(CBMask), cbMask.ReleaseAndGetAddressOf());

    // フェード
    Fade::Instance().Initialize(1.0f);
}

// 終了化
void SceneTitle::Finalize()
{
    StageManager::Instance().Clear();

    // エネミー終了化
    EnemyManager::Instance().Clear();

    UIManager::Instance().Clear();
}

// 更新処理
void SceneTitle::Update(const float& elapsedTime)
{
    float a = sizeof(elapsedTime);

    GamePad& gamePad = Input::Instance().GetGamePad();

    PostEffect::Instance().Update(elapsedTime);
    UIManager::Instance().Update(elapsedTime);

    //controller->Update(elapsedTime);

    // マスクの更新(タイトルを出す)
    mask.dissolveThreshold += elapsedTime * 0.5f;

    // エネミー更新処理
    EnemyManager::Instance().Update(elapsedTime);

    // エフェクト更新処理
    EffectManager::Instance().Update(elapsedTime);

    agent->Update(elapsedTime);

#pragma region 遷移
    // なにかボタンを押したらフェード開始
    const GamePadButton anyButton =
          GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_X
        | GamePad::BTN_Y;
    if (gamePad.GetButtonDown() & anyButton) {
        Fade::Instance().SetFade();
    }

    // フェードを終了すると遷移
    if (Fade::Instance().PlayFade(elapsedTime))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
    }    
#pragma endregion
}

// 描画処理
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    RenderState& rs    = RenderState::Instance();
    ShaderState& ss    = ShaderState::Instance();

    ID3D11DeviceContext* dc     = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

#if 1
    // オフスクリーンの警告をなくす
    ID3D11RenderTargetView* rtvsNull[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
    dc->OMSetRenderTargets(_countof(rtvsNull), rtvsNull, 0);
    ID3D11ShaderResourceView* srvsNull[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    dc->VSSetShaderResources(0, _countof(srvsNull), srvsNull);
    dc->PSSetShaderResources(0, _countof(srvsNull), srvsNull);
#endif

    // サンプラーステート設定
    rs.SetPSSampler(dc, 0, 1, SAMPLER_MODE::POINT);
    rs.SetPSSampler(dc, 1, 1, SAMPLER_MODE::LINEAR);
    rs.SetPSSampler(dc, 2, 1, SAMPLER_MODE::ANISOTROPIC);
    rs.SetPSSampler(dc, 3, 1, SAMPLER_MODE::LINER_BORDER_BLACK);
    rs.SetPSSampler(dc, 4, 1, SAMPLER_MODE::LINER_BORDER_WHITE);
    rs.SetPSSampler(dc, 5, 1, SAMPLER_MODE::LINEAR_MIRROR);

    // シャドウマップの描画
    RenderShadowmap(dc);

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 1.0f,1.0f,1.0f,1.0f };  // RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);
    
    RenderContext rc;
    SetRenderContext(rc);

    // ゲームシーンのビューポートの設定
    D3D11_VIEWPORT vp = {};
    vp.Width          = graphics.GetScreenWidth();
    vp.Height         = graphics.GetScreenHeight();
    vp.MinDepth       = 0.0f;
    vp.MaxDepth       = 1.0f;
    dc->RSSetViewports(1, &vp);

#if USEPOSTEFFECT
    // オフスクリーン描画
    RenderOffscreen(dc, rc);

#else
    Render3D(dc, rc);
#endif
    // 2D描画
    //Render2D(dc, rc);

#if DEBUG_FLG
    DrawDebugGUI();
#endif
}

void SceneTitle::RenderShadowmap(ID3D11DeviceContext* dc)
{
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11DepthStencilView* dsv = dsvShadowmap->GetDSV().Get();

    // 画面クリア
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // レンダーターゲット設定
    dc->OMSetRenderTargets(0, &rtv, dsv);

    // ビューポートの設定
    D3D11_VIEWPORT	vp = {};
    vp.Width           = static_cast<float>(dsvShadowmap->GetWidth());
    vp.Height          = static_cast<float>(dsvShadowmap->GetHeight());
    vp.MinDepth        = 0.0f;
    vp.MaxDepth        = 1.0f;
    dc->RSSetViewports(1, &vp);

    RenderContext rc{};

    DirectX::XMFLOAT3 LightDirection = { lightDir.x,lightDir.y,lightDir.z };

    //カメラパラメータ設定
    {
        // 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を作成
        DirectX::XMVECTOR lightPosition = DirectX::XMLoadFloat3(&LightDirection);
        lightPosition = DirectX::XMVectorScale(lightPosition, lightBias);

        // シャドウマップに描画したい範囲の射影行列を生成
        DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
            lightPosition,
            DirectX::XMVectorSet(0, 0, 0, 0),
            DirectX::XMVectorSet(0, 1, 0, 0)
        );
        DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowRect, shadowRect, 0.1f, 1000.0f);

        DirectX::XMStoreFloat4x4(&rc.view, V);
        DirectX::XMStoreFloat4x4(&rc.projection, P);

        DirectX::XMStoreFloat4x4(&lightVP, V * P);
    }

    RenderState& rs = RenderState::Instance();
    ShaderState& ss = ShaderState::Instance();
   
    rs.SetBlend(dc, BLEND_MODE::ALPHA);
    rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

    // 影描画
    Shader* shadow = ss.GetShadowmapShader();
    shadow->Begin(dc, rc);
    StageManager::Instance().Render(dc, shadow);
    EnemyManager::Instance().Render(dc, shadow);
    shadow->End(dc);
}

void SceneTitle::Render3D(ID3D11DeviceContext* dc, const RenderContext& rc)
{
    Graphics& graphics = Graphics::Instance();
    RenderState& rs    = RenderState::Instance();
    ShaderState& ss    = ShaderState::Instance();

    // ブレンド設定
    rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_OFF);
    rs.SetBlend(dc, BLEND_MODE::ALPHA);
    {
        SkymapShader* shader = ss.GetSkymapShader();

        shader->Begin(dc, rc);
        shader->Draw(dc, skymap.get());
        shader->End(dc);
    }

    // ブレンド設定
    rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
    rs.SetBlend(dc, BLEND_MODE::ALPHA);

    Shader* shader = ss.GetModelShader(MODEL::DefaultPBR);

    shader->Begin(dc, rc);
    StageManager::Instance().Render(dc, shader);
    EnemyManager::Instance().Render(dc, shader);
    shader->End(dc);

    // エフェクト描画処理
    rs.SetBlend(dc, BLEND_MODE::ADD);
    rs.SetDepth(dc, DEPTH_MODE::ZT_OFF_ZW_OFF);
    rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_BACK);

    EffectManager::Instance().Render(rc.view, rc.projection);
}

void SceneTitle::Render2D(ID3D11DeviceContext* dc, const RenderContext& rc)
{
    Graphics& graphics = Graphics::Instance();
    RenderState& rs    = RenderState::Instance();
    ShaderState& ss    = ShaderState::Instance();

    // ブレンド設定
    rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
    rs.SetBlend(dc, BLEND_MODE::ALPHA);

    dc->PSSetConstantBuffers(0, 1, cbMask.GetAddressOf());
    dc->UpdateSubresource(cbMask.Get(), 0, 0, &mask, 0, 0);

    dc->PSSetShaderResources(22, 1, maskSRV.GetAddressOf());

    // UI描画
    SpriteShader* shader = ss.GetSpriteShader(SPRITE::Mask);
    shader->Begin(dc);
    UIManager::Instance().Render(dc, shader);
    shader->End(dc);
}

void SceneTitle::DrawDebugGUI()
{
    Camera& camera = Camera::Instance();

    ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Title", nullptr, ImGuiWindowFlags_None))
    {
        // カメラ情報
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            eye   = camera.GetEye();
            focus = camera.GetFocus();
            up    = camera.GetUp();

            ImGui::InputFloat3("Eye",   &eye.x);
            ImGui::InputFloat3("Focus", &focus.x);
            ImGui::InputFloat3("Up",    &up.x);
        }

        // マスク
        if (ImGui::CollapsingHeader("Mask", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat("Dissolve", &mask.dissolveThreshold, 0.0f, 1.0f);
            ImGui::SliderFloat("Edge",     &mask.edgeThreshold,     0.0f, 1.0f);
            ImGui::ColorEdit3("colorBalance",   &mask.edgeColor.x);
            ImGui::SliderFloat("scorchingHeat", &mask.edgeColor.w, 0.1f, 100.0f);

        }
    }

    ImGui::End();
}

void SceneTitle::RenderOffscreen(ID3D11DeviceContext* dc, const RenderContext& rc)
{
    Graphics& graphics = Graphics::Instance();
    RenderState& rs    = RenderState::Instance();
    ShaderState& ss    = ShaderState::Instance();

    // オフスクリーン
    FrameBuffer* frameBuffer = ss.GetOffscreenBuffer(OFFSCREEN::OFFSCREEN_SET);

    // ゲームシーンをオフスクリーンにする。
    frameBuffer->Clear(dc);
    frameBuffer->Activate(dc);
    Render3D(dc, rc);
    frameBuffer->DeActivate(dc);

    rs.SetBlend(dc, BLEND_MODE::ALPHA);
    rs.SetDepth(dc, DEPTH_MODE::ZT_OFF_ZW_OFF);

#pragma region 川瀬式ブラー
    ID3D11PixelShader* luminance = ss.GetOffscreenShader(OFFSCREEN::LUMINANCE);

    // 川瀬式ブラーやってみる
    for (int i = 0; i < 4; ++i)
    {
        ss.GetBlurBuffer(i)->Clear(dc);
        ss.GetBlurBuffer(i)->Activate(dc);
        bitBlockTransfer->Blit(dc, frameBuffer->shaderResourceViews[0].GetAddressOf(), 6 + i, 1, luminance);
        ss.GetBlurBuffer(i)->DeActivate(dc);
    }
#pragma endregion

#pragma region 陽炎

    PostEffect::Instance().UpdateHeatHaze(dc);

    // オフスクリーンのシェーダーリソースビューをGPU側に送る
    ID3D11ShaderResourceView* srvs[2]
    {
        // 輝度抽出したシーン
        frameBuffer->shaderResourceViews[0].Get(),
        // 深度
        frameBuffer->shaderResourceViews[1].Get()
    };

    FrameBuffer* heatHazeBuffer = ss.GetOffscreenBuffer(OFFSCREEN::HEATHAZE);
    ID3D11PixelShader* heatHaze = ss.GetOffscreenShader(OFFSCREEN::HEATHAZE);

    // オフスクリーンにした画面に輝度抽出を行う
    heatHazeBuffer->Clear(dc);
    heatHazeBuffer->Activate(dc);
    bitBlockTransfer->Blit(dc, srvs, 6, _countof(srvs), heatHaze);
    heatHazeBuffer->DeActivate(dc);
#pragma endregion

    PostEffect::Instance().UpdateBloom(dc);

    // オフスクリーンのシェーダーリソースビューをGPU側に送る
    ID3D11ShaderResourceView* finalSrvs[7]
    {
        // ゲームシーン
        frameBuffer->shaderResourceViews[0].Get(),
        // 陽炎
        heatHazeBuffer->shaderResourceViews[0].Get(),
        // 川瀬式ブラー
        ss.GetBlurBuffer(0)->shaderResourceViews[0].Get(),
        ss.GetBlurBuffer(1)->shaderResourceViews[0].Get(),
        ss.GetBlurBuffer(2)->shaderResourceViews[0].Get(),
        ss.GetBlurBuffer(3)->shaderResourceViews[0].Get(),

    };
    ID3D11PixelShader* Final = ss.GetOffscreenShader(OFFSCREEN::FINAL);
    bitBlockTransfer->Blit(dc, finalSrvs, 6, _countof(finalSrvs), Final);

#pragma endregion
}

void SceneTitle::SetRenderContext(RenderContext& rc)
{
    rc.lightDirection = lightDir;    // ライト方向（下方向）
    rc.lightColor     = lightColor;  // ライト色

    Camera& camera = Camera::Instance();

    // シャドウマップの設定
    rc.shadowmapData.shadowmap   = dsvShadowmap->GetSRV().Get();
    rc.shadowmapData.lightVP     = lightVP;
    rc.shadowmapData.shadowColor = shadowColor;
    rc.shadowmapData.shadowBias  = shadowBias;

    // カメラ位置
    rc.viewPosition.x = camera.GetEye().x;
    rc.viewPosition.y = camera.GetEye().y;
    rc.viewPosition.z = camera.GetEye().z;
    rc.viewPosition.w = 1;

    rc.view       = camera.GetView();
    rc.projection = camera.GetProjection();
}

#endif