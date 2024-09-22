#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

// �V���h�E�}�b�v�p���
struct ShadowmapData
{
	ID3D11ShaderResourceView* shadowmap;
	DirectX::XMFLOAT4X4       lightVP;
	DirectX::XMFLOAT3         shadowColor;
	float					  shadowBias;
};

// �����_�[�R���e�L�X�g
struct RenderContext
{
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;
	DirectX::XMFLOAT4		lightColor;

	ShadowmapData			shadowmapData;
};