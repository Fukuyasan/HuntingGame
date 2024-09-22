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

// コンストラクタ
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
	, input(hWnd)
{
	// グラフィック初期化
	Graphics::Instance().Initialize(hWnd);

	// 各ステート初期化
	ShaderState::Instance().Initialize(hWnd);
	RenderState::Instance().Initialize();

	// エフェクトマネージャー初期化
	EffectManager::Instance().Initialize();

	// シーン初期化
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));

	// リソースマネージャー初期化
	ResourceManager::Instance().Initialize();

	// オーディオ初期化
	Audio::Instance().Initialize();
}

// デストラクタ
Framework::~Framework()
{
	// シーン終了化
	SceneManager::Instance().Clear();

	// エフェクトマネージャー終了化
	EffectManager::Instance().Finalize();

	// リソースマネージャー終了化
	ResourceManager::Instance().Finalize();

	// オーディオ終了化
	Audio::Instance().Finalize();
}

// 更新処理
void Framework::Update()
{
	const float elapsedTime = TimeManager::Instance().GetDeltaTime();

	// 入力更新処理
	input.Update();

	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);
}

// 描画処理
void Framework::Render()
{
	Graphics&    graphics    = Graphics::Instance();
	ShaderState& shaderState = ShaderState::Instance();

	// 別スレッドにデバイスコンテキストが使われていた場合に
	// 同時アクセスしないように排他制御する

	// ※ std::lock_guard変数のコンストラクタにミューテックスを渡すことで、信号待ちをする
	std::lock_guard<std::mutex> mutex{ graphics.GetMutex() };

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// IMGUIフレーム開始処理
	shaderState.GetImGuiRenderer()->NewFrame();

	// シーン描画処理
	SceneManager::Instance().Render();

	// IMGUIデモウインドウ描画（IMGUI機能テスト用）
	//ImGui::ShowDemoWindow();

	// IMGUI描画
	shaderState.GetImGuiRenderer()->Render(dc);

	// バックバッファに描画した画を画面に表示する。
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

// フレームレート計算
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

// アプリケーションループ
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

// メッセージハンドラ
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
