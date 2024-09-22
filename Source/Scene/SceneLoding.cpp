#include "Graphics/Graphics.h"
#include "Graphics/RenderState.h"
#include "Graphics/ShaderState.h"
#include "System/Input/Input.h"
#include "SceneLoding.h"
#include "SceneManager.h"

#include "UI/UIManager.h"

// ������
void SceneLoading::Initialize()
{
    // �X���b�h�J�n   
    thread = new std::thread(LoadingThread, this);

    for (int i = 0; i < _countof(nowLoading); ++i)
    {
        nowLoading[i] = std::make_unique<NowLoading>(i, 0.1f * (i + 1));
    }
}

// �I����
void SceneLoading::Finalize()
{
    // �X���b�h�I����
    if (thread != nullptr)
    {
        thread->join();

        delete thread;
        thread = nullptr;
    }
}

// �X�V����
void SceneLoading::Update(const float& elapsedTime)
{
    // NOWLOADING�̍X�V����
    for (int i = 0; i < _countof(nowLoading); ++i)
    {
        nowLoading[i]->Update(elapsedTime);
    }

    // ���̃V�[���̏���������������V�[����؂�ւ���
    if (nextScene->IsReady())
    {
        SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
    }
}

// �`�揈��
void SceneLoading::Render()
{
    Graphics& graphics          = Graphics::Instance();
    RenderState& bs             = RenderState::Instance();
    ShaderState& ss             = ShaderState::Instance();
    ID3D11DeviceContext* dc     = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };  // RGBA(0.0f�`1.0f)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // �u�����h�ݒ�
    bs.SetBlend(dc, BLEND_MODE::ALPHA);
    bs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

    // �T���v���[�X�e�[�g�ݒ�
    bs.SetPSSampler(dc, 0, SAMPLER_MODE::POINT);
    bs.SetPSSampler(dc, 1, SAMPLER_MODE::LINEAR);
    bs.SetPSSampler(dc, 2, SAMPLER_MODE::ANISOTROPIC);
    bs.SetPSSampler(dc, 3, SAMPLER_MODE::LINER_BORDER_BLACK);
    bs.SetPSSampler(dc, 4, SAMPLER_MODE::LINER_BORDER_WHITE);
    bs.SetPSSampler(dc, 5, SAMPLER_MODE::LINEAR_MIRROR);

    SpriteShader* shader = ss.GetSpriteShader(SPRITE::Default);
    shader->Begin(dc);

    // NOWLOADING�`��
    for (int i = 0; i < _countof(nowLoading); ++i)
    {
        nowLoading[i]->Render(dc, shader);
    }

    shader->End(dc);
}

// ���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    // COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    CoInitialize(nullptr);

    // ���̃V�[���̏��������s��
    scene->nextScene->Initialize();    
    
    // �X���b�h���I���O��COM�֘A�̏I����
    CoUninitialize();

    // ���̃V�[���̏��������ݒ�
    scene->nextScene->SetReady();
}
