#include "RenderState.h"
#include "System/Misc.h"
#include "Graphics/Graphics.h"

void RenderState::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	HRESULT hr           = S_OK;

	// ブレンドステートの設定	
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		// NONE
		desc.AlphaToCoverageEnable                 = false;
		desc.IndependentBlendEnable                = false;
		desc.RenderTarget[0].BlendEnable           = false;
		desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_MODE::NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		
		// ALPHA
		desc.AlphaToCoverageEnable                 = false;
		desc.IndependentBlendEnable                = false;
		desc.RenderTarget[0].BlendEnable           = true;
		desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_MODE::ALPHA)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		
		// ADD
		desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE D3D11_BLEND_SRC_ALPHA
		desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_MODE::ADD)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		
		// MULTIPRY
		desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ZERO; //D3D11_BLEND_ONE D3D11_BLEND_SRC_ALPHA
		desc.RenderTarget[0].DestBlend             = D3D11_BLEND_SRC_COLOR;
		desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_DEST_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_MODE::MULTIPRY)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// デプスステンシルモード
	{
		D3D11_DEPTH_STENCIL_DESC depthDesc{};
		// 震度テスト：オン　震度ライト：オン
		depthDesc.DepthEnable    = true;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthDesc, depthStates[static_cast<size_t>(DEPTH_MODE::ZT_ON_ZW_ON)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 震度テスト：オン　震度ライト：オフ
		depthDesc.DepthEnable    = true;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthDesc, depthStates[static_cast<size_t>(DEPTH_MODE::ZT_ON_ZW_OFF)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 震度テスト：オフ　震度ライト：オン
		depthDesc.DepthEnable    = false;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthDesc, depthStates[static_cast<size_t>(DEPTH_MODE::ZT_OFF_ZW_ON)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 震度テスト：オフ　震度ライト：オフ
		depthDesc.DepthEnable    = false;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthDesc, depthStates[static_cast<size_t>(DEPTH_MODE::ZT_OFF_ZW_OFF)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// サンプラーステート
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias     = 0;
		samplerDesc.MaxAnisotropy  = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MinLOD         = 0;
		samplerDesc.MaxLOD         = D3D11_FLOAT32_MAX;

		// POINT
		hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_MODE::POINT)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// LINEAR
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_MODE::LINEAR)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// ANISOTROPIC
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_MODE::ANISOTROPIC)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;

		// LINER_BORDER_BLACK
		hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_MODE::LINER_BORDER_BLACK)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		samplerDesc.BorderColor[0] = 1;
		samplerDesc.BorderColor[1] = 1;
		samplerDesc.BorderColor[2] = 1;
		samplerDesc.BorderColor[3] = 1;

		// LINEAR_BORDER_WHITE
		hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_MODE::LINER_BORDER_WHITE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

		// LINEAR_MIRROR
		hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_MODE::LINEAR_MIRROR)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}

	// ラスタライザステート
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode              = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode              = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias             = 0;
		rasterizer_desc.DepthBiasClamp        = 0;
		rasterizer_desc.SlopeScaledDepthBias  = 0;
		rasterizer_desc.DepthClipEnable       = TRUE;
		rasterizer_desc.ScissorEnable         = FALSE;
		rasterizer_desc.MultisampleEnable     = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;

		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizerStates[static_cast<size_t>(RASTERIZER_MODE::SOLID_BACK)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;

		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizerStates[static_cast<size_t>(RASTERIZER_MODE::SOLID_NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = TRUE;

		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizerStates[static_cast<size_t>(RASTERIZER_MODE::WIREFRAME_BACK)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;

		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizerStates[static_cast<size_t>(RASTERIZER_MODE::WIREFRAME_NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}
}

// ブレンドステート設定
void RenderState::SetBlend(ID3D11DeviceContext* dc, BLEND_MODE blendMode)
{
	dc->OMSetBlendState(blendStates[static_cast<size_t>(blendMode)].Get(), nullptr, 0xFFFFFFFF);
}

// 震度ステート設定
void RenderState::SetDepth(ID3D11DeviceContext* dc, DEPTH_MODE depthMode)
{
	dc->OMSetDepthStencilState(depthStates[static_cast<size_t>(depthMode)].Get(), 0);
}

// サンプラーステート設定
void RenderState::SetPSSampler(ID3D11DeviceContext* dc, UINT startSlot, SAMPLER_MODE samplerMode)
{
	dc->PSSetSamplers(startSlot, 1, samplerStates[static_cast<size_t>(samplerMode)].GetAddressOf());
}

// ラスタライザステート設定
void RenderState::SetRasterizer(ID3D11DeviceContext* dc, RASTERIZER_MODE rasterizerMode)
{
	dc->RSSetState(rasterizerStates[static_cast<size_t>(rasterizerMode)].Get());
}
