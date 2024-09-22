#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>

#include "Shader.h"
#include "FrameBuffer.h"
#include "Shader/ModelShader/Shadowmap/ShadowmapCasterShader.h"

#include "Renderer/DebugRenderer.h"
#include "Renderer/LineRenderer.h"
#include "Renderer/ImGuiRenderer.h"
#include "Renderer/NavMeshRenderer.h"

// c++20 で u8 が使えないのを解決する
inline const char* operator""_u(const char8_t* const str, size_t len)
{
	return reinterpret_cast<const char*>(str);
}

// グラフィックス
class Graphics final
{
private:
	Graphics()  = default;
	~Graphics() = default;

public:
	Graphics(const Graphics&) = delete;
	Graphics(Graphics&&)      = delete;
	Graphics operator=(const Graphics&) = delete;
	Graphics operator=(Graphics&&)		= delete;

public:
	// インスタンス取得
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}
	
	// 初期化
	void Initialize(HWND hWnd);
	void Clear(float r, float g, float b);

	// デバイス取得
	ID3D11Device* GetDevice() const { return device.Get(); }

	// デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContext() const { return deviceContext.Get(); }

	// スワップチェーン取得
	IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	Microsoft::WRL::ComPtr<ID3D11Debug> const& GetDebug() const {
		return debug;
	}

	// スクリーン幅,高さ取得
	const float GetScreenWidth()  const { return screenWidth; }
	const float GetScreenHeight() const { return screenHeight; }

	// ミューテックス取得
	// ※ DeviceContextを同時アクセスさせないための
	//    排他制御用オブジェクト
	std::mutex& GetMutex() { return mutex; }
private:
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	Microsoft::WRL::ComPtr<ID3D11Debug> debug;

	float	screenWidth;
	float	screenHeight;

	// ミューテックス
	std::mutex mutex;
};

