#include "SkymapShader.h"
#include "System/Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/CreateShader.h"

SkymapShader::SkymapShader(ID3D11Device* device)
{
	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// �V�F�[�_�[
	CreateShader::CreateVSFromCso(device, "Shader\\SkymapVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	CreateShader::CreatePSFromCso(device, "Shader\\SkymapPS.cso", pixelShader.ReleaseAndGetAddressOf());

	// �萔�o�b�t�@
	CreateShader::CreateCB(device, sizeof(CbScene), sceneConstantBuffer.ReleaseAndGetAddressOf());
}

void SkymapShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	// �V�F�[�_�[�̐ݒ�
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(),  nullptr, 0);
	// ���̓��C�A�E�g�̐ݒ�
	dc->IASetInputLayout(inputLayout.Get());

	dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �萔�o�b�t�@�̐ݒ� : 12�ԂɃZ�b�g
	dc->VSSetConstantBuffers(12, 1, sceneConstantBuffer.GetAddressOf());

	// �V�[���p�萔�o�b�t�@�X�V
	CbScene cbScene;

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);

	// �t�s����擾
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
