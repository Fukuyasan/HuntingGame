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

// ������
void SceneTitle::Initialize()
{
    Graphics&     graphics     = Graphics::Instance();
    StageManager& stageManager = StageManager::Instance();
    EnemyManager& enemyManager = EnemyManager::Instance();
    UIManager&    uiManager    = UIManager::Instance();

#pragma region �X�e�[�W
    StageMain* stage = new StageMain();
    stageManager.Register(stage);
#pragma endregion

#pragma region �J����
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

#pragma region �i�r���b�V��
    // �i�r���b�V��������
    navMesh = std::make_unique<SoloMesh>("navMeshData.bin");

    // �G�[�W�F���g������
    agent = std::make_unique<NavMeshAgent>(navMesh.get());
#pragma endregion

#pragma region ���H����
    const int stegoCount = 16;
    for (int i = 0; i < stegoCount; ++i)
    {
        Stego* stego = new Stego();

        // �~��ɏo��������
        float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
        float range = i;

        stego->transform.SetPositionX(stego->transform.GetPosition().x + sinf(theta) * range);
        stego->transform.SetPositionY(stego->transform.GetPosition().y);
        stego->transform.SetPositionZ(stego->transform.GetPosition().z + cosf(theta) * range);

        // �G�[�W�F���g�o�^
        AgentParams ap{};
        ap.height = 5.0f;
        ap.radius = 2.0f;
        ap.maxSpeed = 50.0f;
        ap.maxAcceleration = ap.maxSpeed;
        agent->AddAgent(stego->transform.GetPosition(), ap);

        // �G�[�W�F���g�Z�b�g
        stego->SetAgent(agent, i);

        enemyManager.Register(stego);
    }
#pragma endregion

#pragma region �^�C�g�����S   
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

    // �V���h�E�}�b�v
    dsvShadowmap = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);

    // �X�J�C�}�b�v�e�N�X�`��
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

    // �I�t�X�N���[��
    bitBlockTransfer = std::make_unique<FullscreeQuad>(graphics.GetDevice());

    PostEffect::Instance().Initialize();

    // �萔�o�b�t�@
    CreateShader::CreateCB(graphics.GetDevice(), sizeof(CBMask), cbMask.ReleaseAndGetAddressOf());

    // �t�F�[�h
    Fade::Instance().Initialize(1.0f);
}

// �I����
void SceneTitle::Finalize()
{
    StageManager::Instance().Clear();

    // �G�l�~�[�I����
    EnemyManager::Instance().Clear();

    UIManager::Instance().Clear();
}

// �X�V����
void SceneTitle::Update(const float& elapsedTime)
{
    float a = sizeof(elapsedTime);

    GamePad& gamePad = Input::Instance().GetGamePad();

    PostEffect::Instance().Update(elapsedTime);
    UIManager::Instance().Update(elapsedTime);

    //controller->Update(elapsedTime);

    // �}�X�N�̍X�V(�^�C�g�����o��)
    mask.dissolveThreshold += elapsedTime * 0.5f;

    // �G�l�~�[�X�V����
    EnemyManager::Instance().Update(elapsedTime);

    // �G�t�F�N�g�X�V����
    EffectManager::Instance().Update(elapsedTime);

    agent->Update(elapsedTime);

#pragma region �J��
    // �Ȃɂ��{�^������������t�F�[�h�J�n
    const GamePadButton anyButton =
          GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_X
        | GamePad::BTN_Y;
    if (gamePad.GetButtonDown() & anyButton) {
        Fade::Instance().SetFade();
    }

    // �t�F�[�h���I������ƑJ��
    if (Fade::Instance().PlayFade(elapsedTime))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
    }    
#pragma endregion
}

// �`�揈��
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    RenderState& rs    = RenderState::Instance();
    ShaderState& ss    = ShaderState::Instance();

    ID3D11DeviceContext* dc     = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

#if 1
    // �I�t�X�N���[���̌x�����Ȃ���
    ID3D11RenderTargetView* rtvsNull[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
    dc->OMSetRenderTargets(_countof(rtvsNull), rtvsNull, 0);
    ID3D11ShaderResourceView* srvsNull[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    dc->VSSetShaderResources(0, _countof(srvsNull), srvsNull);
    dc->PSSetShaderResources(0, _countof(srvsNull), srvsNull);
#endif

    // �T���v���[�X�e�[�g�ݒ�
    rs.SetPSSampler(dc, 0, 1, SAMPLER_MODE::POINT);
    rs.SetPSSampler(dc, 1, 1, SAMPLER_MODE::LINEAR);
    rs.SetPSSampler(dc, 2, 1, SAMPLER_MODE::ANISOTROPIC);
    rs.SetPSSampler(dc, 3, 1, SAMPLER_MODE::LINER_BORDER_BLACK);
    rs.SetPSSampler(dc, 4, 1, SAMPLER_MODE::LINER_BORDER_WHITE);
    rs.SetPSSampler(dc, 5, 1, SAMPLER_MODE::LINEAR_MIRROR);

    // �V���h�E�}�b�v�̕`��
    RenderShadowmap(dc);

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 1.0f,1.0f,1.0f,1.0f };  // RGBA(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);
    
    RenderContext rc;
    SetRenderContext(rc);

    // �Q�[���V�[���̃r���[�|�[�g�̐ݒ�
    D3D11_VIEWPORT vp = {};
    vp.Width          = graphics.GetScreenWidth();
    vp.Height         = graphics.GetScreenHeight();
    vp.MinDepth       = 0.0f;
    vp.MaxDepth       = 1.0f;
    dc->RSSetViewports(1, &vp);

#if USEPOSTEFFECT
    // �I�t�X�N���[���`��
    RenderOffscreen(dc, rc);

#else
    Render3D(dc, rc);
#endif
    // 2D�`��
    //Render2D(dc, rc);

#if DEBUG_FLG
    DrawDebugGUI();
#endif
}

void SceneTitle::RenderShadowmap(ID3D11DeviceContext* dc)
{
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11DepthStencilView* dsv = dsvShadowmap->GetDSV().Get();

    // ��ʃN���A
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // �����_�[�^�[�Q�b�g�ݒ�
    dc->OMSetRenderTargets(0, &rtv, dsv);

    // �r���[�|�[�g�̐ݒ�
    D3D11_VIEWPORT	vp = {};
    vp.Width           = static_cast<float>(dsvShadowmap->GetWidth());
    vp.Height          = static_cast<float>(dsvShadowmap->GetHeight());
    vp.MinDepth        = 0.0f;
    vp.MaxDepth        = 1.0f;
    dc->RSSetViewports(1, &vp);

    RenderContext rc{};

    DirectX::XMFLOAT3 LightDirection = { lightDir.x,lightDir.y,lightDir.z };

    //�J�����p�����[�^�ݒ�
    {
        // ���s��������J�����ʒu���쐬���A�������猴�_�̈ʒu������悤�Ɏ����s����쐬
        DirectX::XMVECTOR lightPosition = DirectX::XMLoadFloat3(&LightDirection);
        lightPosition = DirectX::XMVectorScale(lightPosition, lightBias);

        // �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
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

    // �e�`��
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

    // �u�����h�ݒ�
    rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_OFF);
    rs.SetBlend(dc, BLEND_MODE::ALPHA);
    {
        SkymapShader* shader = ss.GetSkymapShader();

        shader->Begin(dc, rc);
        shader->Draw(dc, skymap.get());
        shader->End(dc);
    }

    // �u�����h�ݒ�
    rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
    rs.SetBlend(dc, BLEND_MODE::ALPHA);

    Shader* shader = ss.GetModelShader(MODEL::DefaultPBR);

    shader->Begin(dc, rc);
    StageManager::Instance().Render(dc, shader);
    EnemyManager::Instance().Render(dc, shader);
    shader->End(dc);

    // �G�t�F�N�g�`�揈��
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

    // �u�����h�ݒ�
    rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
    rs.SetBlend(dc, BLEND_MODE::ALPHA);

    dc->PSSetConstantBuffers(0, 1, cbMask.GetAddressOf());
    dc->UpdateSubresource(cbMask.Get(), 0, 0, &mask, 0, 0);

    dc->PSSetShaderResources(22, 1, maskSRV.GetAddressOf());

    // UI�`��
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
        // �J�������
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            eye   = camera.GetEye();
            focus = camera.GetFocus();
            up    = camera.GetUp();

            ImGui::InputFloat3("Eye",   &eye.x);
            ImGui::InputFloat3("Focus", &focus.x);
            ImGui::InputFloat3("Up",    &up.x);
        }

        // �}�X�N
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

    // �I�t�X�N���[��
    FrameBuffer* frameBuffer = ss.GetOffscreenBuffer(OFFSCREEN::OFFSCREEN_SET);

    // �Q�[���V�[�����I�t�X�N���[���ɂ���B
    frameBuffer->Clear(dc);
    frameBuffer->Activate(dc);
    Render3D(dc, rc);
    frameBuffer->DeActivate(dc);

    rs.SetBlend(dc, BLEND_MODE::ALPHA);
    rs.SetDepth(dc, DEPTH_MODE::ZT_OFF_ZW_OFF);

#pragma region �쐣���u���[
    ID3D11PixelShader* luminance = ss.GetOffscreenShader(OFFSCREEN::LUMINANCE);

    // �쐣���u���[����Ă݂�
    for (int i = 0; i < 4; ++i)
    {
        ss.GetBlurBuffer(i)->Clear(dc);
        ss.GetBlurBuffer(i)->Activate(dc);
        bitBlockTransfer->Blit(dc, frameBuffer->shaderResourceViews[0].GetAddressOf(), 6 + i, 1, luminance);
        ss.GetBlurBuffer(i)->DeActivate(dc);
    }
#pragma endregion

#pragma region �z��

    PostEffect::Instance().UpdateHeatHaze(dc);

    // �I�t�X�N���[���̃V�F�[�_�[���\�[�X�r���[��GPU���ɑ���
    ID3D11ShaderResourceView* srvs[2]
    {
        // �P�x���o�����V�[��
        frameBuffer->shaderResourceViews[0].Get(),
        // �[�x
        frameBuffer->shaderResourceViews[1].Get()
    };

    FrameBuffer* heatHazeBuffer = ss.GetOffscreenBuffer(OFFSCREEN::HEATHAZE);
    ID3D11PixelShader* heatHaze = ss.GetOffscreenShader(OFFSCREEN::HEATHAZE);

    // �I�t�X�N���[���ɂ�����ʂɋP�x���o���s��
    heatHazeBuffer->Clear(dc);
    heatHazeBuffer->Activate(dc);
    bitBlockTransfer->Blit(dc, srvs, 6, _countof(srvs), heatHaze);
    heatHazeBuffer->DeActivate(dc);
#pragma endregion

    PostEffect::Instance().UpdateBloom(dc);

    // �I�t�X�N���[���̃V�F�[�_�[���\�[�X�r���[��GPU���ɑ���
    ID3D11ShaderResourceView* finalSrvs[7]
    {
        // �Q�[���V�[��
        frameBuffer->shaderResourceViews[0].Get(),
        // �z��
        heatHazeBuffer->shaderResourceViews[0].Get(),
        // �쐣���u���[
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
    rc.lightDirection = lightDir;    // ���C�g�����i�������j
    rc.lightColor     = lightColor;  // ���C�g�F

    Camera& camera = Camera::Instance();

    // �V���h�E�}�b�v�̐ݒ�
    rc.shadowmapData.shadowmap   = dsvShadowmap->GetSRV().Get();
    rc.shadowmapData.lightVP     = lightVP;
    rc.shadowmapData.shadowColor = shadowColor;
    rc.shadowmapData.shadowBias  = shadowBias;

    // �J�����ʒu
    rc.viewPosition.x = camera.GetEye().x;
    rc.viewPosition.y = camera.GetEye().y;
    rc.viewPosition.z = camera.GetEye().z;
    rc.viewPosition.w = 1;

    rc.view       = camera.GetView();
    rc.projection = camera.GetProjection();
}

#endif