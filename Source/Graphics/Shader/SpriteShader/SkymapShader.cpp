#include "SkymapShader.h"
#include "System/Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/CreateShader.h"

SkymapShader::SkymapShader(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// シェーダー
	CreateShader::CreateVSFromCso(device, "Shader\\SkymapVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	CreateShader::CreatePSFromCso(device, "Shader\\SkymapPS.cso", pixelShader.ReleaseAndGetAddressOf());

	// 定数バッファ
	CreateShader::CreateCB(device, sizeof(CbScene), sceneConstantBuffer.ReleaseAndGetAddressOf());
}

void SkymapShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	// シェーダーの設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(),  nullptr, 0);
	// 入力レイアウトの設定
	dc->IASetInputLayout(inputLayout.Get());

	dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 定数バッファの設定 : 12番にセット
	dc->VSSetConstantBuffers(12, 1, sceneConstantBuffer.GetAddressOf());

	// シーン用定数バッファ更新
	CbScene cbScene;

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);

	// 逆行列を取得
	DirectX::XMStoreFloat4x4(&cbScene.inverseView, DirectX::XMMatrixInverse(nullptr, V));
	DirectX::XMStoreFloat4x4(&cbScene.inverseProjection, DirectX::XMMatrixInverse(nullptr, P));
	cbScene.viewPosition = rc.viewPosition;

	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

void SkymapShader::Draw(ID3D11DeviceContext* dc, const Sprite* sprite)
{
	UINT stride = sizeof(Sprite::Vertex);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1,	sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	dc->PSSetShaderResources(20, 1, sprite->GetShaderResourceView().GetAddressOf());
	
	dc->Draw(4, 0);
}

void SkymapShader::End(ID3D11DeviceContext* dc)
{
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
}
