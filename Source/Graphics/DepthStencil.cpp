#include <stdio.h>
#include "Graphics/Graphics.h"
#include "DepthStencil.h"

#include "System/Misc.h"
#include "System/Logger.h"

DepthStencil::DepthStencil(UINT width, UINT height)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	// テクスチャ
	texture2dDesc = {};
	texture2dDesc.Width              = width;
	texture2dDesc.Height             = height;
	texture2dDesc.MipLevels          = 1;
	texture2dDesc.ArraySize          = 1;
	texture2dDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count   = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage              = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags     = 0;
	texture2dDesc.MiscFlags          = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
	HRESULT hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// シェーダーリソースビュー
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	ZeroMemory(&srvDesc, sizeof(srvDesc));	
	srvDesc.Format                    = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels       = 1;

	hr = device->CreateShaderResourceView(texture2d.Get(), &srvDesc, srv.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 深度ステンシルビュー
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags              = 0;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(texture2d.Get(), &dsvDesc, dsv.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

}
