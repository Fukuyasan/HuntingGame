#include "Framework.h"

#include <memory>
#include <sstream>

#include "Graphics/Graphics.h"
#include "Graphics/RenderState.h"
#include "Graphics/ShaderState.h"

#include "Input/Input.h"

#include "Scene/SceneLoding.h"
#include "Scene/SceneTitle.h"
#include "Scene/SceneTest.h"
#include "Scene/SceneManager.h"

#include "Effect/EffectManager.h"

#include "Graphics/Model/ResourceManager.h"

#include "Audio/Audio.h"

#include "System/Time/TimeManager.h"

// �R���X�g���N�^
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
	, input(hWnd)
{
	// �O���t�B�b�N������
	Graphics::Instance().Initialize(hWnd);

	// �e�X�e�[�g������
	ShaderState::Instance().Initialize(hWnd);
	RenderState::Instance().Initialize();

	// �G�t�F�N�g�}�l�[�W���[������
	EffectManager::Instance().Initialize();

	// �V�[��������
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));

	// ���\�[�X�}�l�[�W���[������
	ResourceManager::Instance().Initialize();

	// �I�[�f�B�I������
	Audio::Instance().Initialize();
}

// �f�X�g���N�^
Framework::~Framework()
{
	// �V�[���I����
	SceneManager::Instance().Clear();

	// �G�t�F�N�g�}�l�[�W���[�I����
	EffectManager::Instance().Finalize();

	// ���\�[�X�}�l�[�W���[�I����
	ResourceManager::Instance().Finalize();

	// �I�[�f�B�I�I����
	Audio::Instance().Finalize();
}

// �X�V����
void Framework::Update()
{
	const float elapsedTime = TimeManager::Instance().GetDeltaTime();

	// ���͍X�V����
	input.Update();

	// �V�[���X�V����
	SceneManager::Instance().Update(elapsedTime);
}

// �`�揈��
void Framework::Render()
{
	Graphics&    graphics    = Graphics::Instance();
	ShaderState& shaderState = ShaderState::Instance();

	// �ʃX���b�h�Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
	// �����A�N�Z�X���Ȃ��悤�ɔr�����䂷��

	// �� std::lock_guard�ϐ��̃R���X�g���N�^�Ƀ~���[�e�b�N�X��n�����ƂŁA�M���҂�������
	std::lock_guard<std::mutex> mutex{ graphics.GetMutex() };

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// IMGUI�t���[���J�n����
	shaderState.GetImGuiRenderer()->NewFrame();

	// �V�[���`�揈��
	SceneManager::Instance().Render();

	// IMGUI�f���E�C���h�E�`��iIMGUI�@�\�e�X�g�p�j
	//ImGui::ShowDemoWindow();

	// IMGUI�`��
	shaderState.GetImGuiRenderer()->Render(dc);

	// �o�b�N�o�b�t�@�ɕ`�悵�������ʂɕ\������B
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

// �t���[�����[�g�v�Z
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((TimeManager::Instance().m_timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// �A�v���P�[�V�������[�v
int Framework::Run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			TimeManager::Instance().Update();

			CalculateFrameStats();

			Update();
			Render();
		}
	}
	return static_cast<int>(msg.wParam);
}

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ShaderState::Instance().GetImGuiRenderer()->HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		TimeManager::Instance().m_timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		TimeManager::Instance().m_timer.Start();
		break;
	case WM_MOUSEWHEEL:
		Input::Instance().GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
