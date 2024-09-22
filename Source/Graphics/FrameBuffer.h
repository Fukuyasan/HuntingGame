#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

#include <DirectXMath.h>

// オフスクリーン用のクラス
class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height);
	~FrameBuffer() = default;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[2];  // 0 : シーンのデータ, 1 : 深度のデータ

	D3D11_VIEWPORT viewport;

	void Clear(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 color = { 0,0,0,1 }, float depth = 1);

	void Activate(ID3D11DeviceContext* dc);
	void DeActivate(ID3D11DeviceContext* dc);

private:
	UINT viewPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewPorts[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
};
