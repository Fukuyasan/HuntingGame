#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

// シャドウマップ用情報
struct ShadowmapData
{
	ID3D11ShaderResourceView* shadowmap;
	DirectX::XMFLOAT4X4       lightVP;
	DirectX::XMFLOAT3         shadowColor;
	float					  shadowBias;
};

// レンダーコンテキスト
struct RenderContext
{
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;
	DirectX::XMFLOAT4		lightColor;

	ShadowmapData			shadowmapData;
};