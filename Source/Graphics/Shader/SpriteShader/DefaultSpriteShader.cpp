#include "DefaultSpriteShader.h"

#include "Graphics/Graphics.h"
#include "Graphics\CreateShader.h"

#include <string>

void SpriteShader::Load(const char* csoName)
{
	Graphics& graphics = Graphics::Instance();

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// CSOのファイル名を取得
	std::string shader = "Shader\\";
	std::string name   = csoName;
	std::string ps     = "PS.cso";

	std::string psName = shader + name + ps;

	// シェーダー
	CreateShader::CreateVSFromCso(graphics.GetDevice(), "Shader\\SpriteVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	CreateShader::CreatePSFromCso(graphics.GetDevice(), psName.c_str(), pixelShader.ReleaseAndGetAddressOf());
}

void SpriteShader::Begin(ID3D11DeviceContext* dc)
{
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());
	
	dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void SpriteShader::Draw(ID3D11DeviceContext* dc, const Sprite* sprite)
{
	UINT stride = sizeof(Sprite::Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1,   sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	dc->PSSetShaderResources(0, 1, sprite->GetShaderResourceView().GetAddressOf());
	
	dc->Draw(4, 0);
}

void SpriteShader::End(ID3D11DeviceContext* dc)
{
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
}
