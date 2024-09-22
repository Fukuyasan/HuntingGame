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

// c++20 �� u8 ���g���Ȃ��̂���������
inline const char* operator""_u(const char8_t* const str, size_t len)
{
	return reinterpret_cast<const char*>(str);
}

// �O���t�B�b�N�X
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
	// �C���X�^���X�擾
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}
	
	// ������
	void Initialize(HWND hWnd);
	void Clear(float r, float g, float b);

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() const { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() const { return deviceContext.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	Microsoft::WRL::ComPtr<ID3D11Debug> const& GetDebug() const {
		return debug;
	}

	// �X�N���[����,�����擾
	const float GetScreenWidth()  const { return screenWidth; }
	const float GetScreenHeight() const { return screenHeight; }

	// �~���[�e�b�N�X�擾
	// �� DeviceContext�𓯎��A�N�Z�X�����Ȃ����߂�
	//    �r������p�I�u�W�F�N�g
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

	// �~���[�e�b�N�X
	std::mutex mutex;
};

