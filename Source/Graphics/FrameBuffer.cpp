#include "FrameBuffer.h"
#include "System/Misc.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height)
{
	HRESULT hr = S_OK;

	// レンダーターゲットバッファの作成
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
	D3D11_TEXTURE2D_DESC texture2dDesc;
	texture2dDesc.Width              = width;
	texture2dDesc.Height             = height;
	texture2dDesc.MipLevels          = 1;
	texture2dDesc.ArraySize          = 1;
	texture2dDesc.Format             = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texture2dDesc.SampleDesc.Count   = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage              = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags     = 0;
	texture2dDesc.MiscFlags          = 0;
	hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// レンダーターゲットビューの作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = texture2dDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), &rtvDesc,
		renderTargetView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// シェーダーリソースビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(renderTargetBuffer.Get(), &srvDesc,
		shaderResourceViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 深度ステンシルバッファの作成
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	hr = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 深度ステンシルビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = 0;
	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = device->CreateShaderResourceView(depthStencilBuffer.Get(), &srvDesc,
		shaderResourceViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// ビューポート設定
	viewport.Width    = static_cast<float>(width);
	viewport.Height   = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void FrameBuffer::Clear(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 color, float depth)
{
	FLOAT Color[4] = { color.x,color.y,color.z,color.w };

	dc->ClearRenderTargetView(renderTargetView.Get(), Color);
	dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

void FrameBuffer::Activate(ID3D11DeviceContext* dc)
{
	viewPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	dc->RSGetViewports(&viewPortCount, cachedViewPorts);
	dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());

	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(1, renderTargetView.GetAddressOf(),depthStencilView.Get());
}

void FrameBuffer::DeActivate(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(viewPortCount, cachedViewPorts);
	dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());

}
