#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/RenderContext.h"
#include "Graphics/Model/Model.h"

// ���f���p
class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// �`��J�n
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	// �`��
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;

	// �`��I��
	virtual void End(ID3D11DeviceContext* context) = 0;

protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader  = nullptr;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout  = nullptr;
};