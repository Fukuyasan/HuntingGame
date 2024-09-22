#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/Sprite.h"

#include "Graphics/RenderContext.h"

class SkymapShader
{
public:
	SkymapShader(ID3D11Device* device);
	~SkymapShader() = default;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};

	// 定数バッファ
	struct CbScene
	{
		DirectX::XMFLOAT4X4 inverseView;
		DirectX::XMFLOAT4X4 inverseProjection;
		DirectX::XMFLOAT4   viewPosition;
	};

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc);
	void Draw(ID3D11DeviceContext* dc, const Sprite* sprite);
	void End(ID3D11DeviceContext* dc);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>		  sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	  vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	  pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	  inputLayout;
};


