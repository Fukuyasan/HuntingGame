#include "TrailShader.h"
#include "Graphics/Graphics.h"
#include "Graphics/CreateShader.h"

#include "System/Misc.h"

#include <cmath>

TrailShader::TrailShader(const char* filename, const int maxTrail) : maxTrailCount(maxTrail)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	texture = std::make_unique<Texture>(filename);

	// �V�F�[�_�[�ݒ�
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	CreateShader::CreateVSFromCso(device, "Shader\\TrailVS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	CreateShader::CreatePSFromCso(device, "Shader\\TrailPS.cso", pixelShader.GetAddressOf());

	// �萔�o�b�t�@
	CreateShader::CreateCB(device, sizeof(CbScene), sceneConstantBuffer.GetAddressOf());
	CreateShader::CreateCB(device, sizeof(CbSubset), subsetConstantBuffer.GetAddressOf());

	// ���_�o�b�t�@
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth           = sizeof(Vertex) * VertexCapacity;
		desc.Usage               = D3D11_USAGE_DYNAMIC;
		desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags           = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void TrailShader::Update()
{
	if (trailParams.size() > maxTrailCount)
		trailParams.pop_back();

	vertices.clear();

	// ���_�f�[�^���X�V����
	float amount = 1.0f / (trailParams.size() - 1);
	float v      = 0.0f;

	// �g���C������ (�X�v���C���Ȑ�)
	const int usedPosCount = static_cast<int>(trailParams.size()) - 3;
	for (int i = 0; i < usedPosCount; ++i)
	{
		DirectX::XMVECTOR head0 = DirectX::XMLoadFloat3(&trailParams[i].head);
		DirectX::XMVECTOR head1 = DirectX::XMLoadFloat3(&trailParams[i + 1].head);
		DirectX::XMVECTOR head2 = DirectX::XMLoadFloat3(&trailParams[i + 2].head);
		DirectX::XMVECTOR head3 = DirectX::XMLoadFloat3(&trailParams[i + 3].head);

		DirectX::XMVECTOR tail0 = DirectX::XMLoadFloat3(&trailParams[i].tail);
		DirectX::XMVECTOR tail1 = DirectX::XMLoadFloat3(&trailParams[i + 1].tail);
		DirectX::XMVECTOR tail2 = DirectX::XMLoadFloat3(&trailParams[i + 2].tail);
		DirectX::XMVECTOR tail3 = DirectX::XMLoadFloat3(&trailParams[i + 3].tail);

		for (float k = 0; k < 1.0f; k += 0.1f)
		{
			DirectX::XMVECTOR Head = DirectX::XMVectorCatmullRom(head0, head1, head2, head3, k);
			DirectX::XMVECTOR Tail = DirectX::XMVectorCatmullRom(tail0, tail1, tail2, tail3, k);

			DirectX::XMFLOAT3 head, tail;
			DirectX::XMStoreFloat3(&head, Head);
			DirectX::XMStoreFloat3(&tail, Tail);

			// ���_�o�b�t�@�ɏ��ǉ�
			AddVertex(head, DirectX::XMFLOAT2(1.0f, v));
			AddVertex(tail, DirectX::XMFLOAT2(0.0f, v));

			v += amount * 0.1f;
		}
	}
}

void TrailShader::Render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection, int blend)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// �V�F�[�_�L����
	Begin(dc, view, projection, blend);

	// ���_�o�b�t�@�[���X�V
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	UINT vertexCount = static_cast<UINT>(vertices.size());
	UINT start = 0;
	UINT count = (std::min)(vertexCount, VertexCapacity);

	// ���_�ҏW
	while (start < vertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		memcpy(mappedSubresource.pData, &vertices[start], sizeof(Vertex) * count);
		
		dc->Unmap(vertexBuffer.Get(), 0);

		dc->Draw(count, 0);

		start += count;
		if ((start + count) > vertexCount)
		{
			count = vertexCount - start;
		}
	}

	//�V�F�[�_������
	End(dc);
}

void TrailShader::Set(const DirectX::XMFLOAT3& head, const DirectX::XMFLOAT3& tail)
{
	TrailParam& param = trailParams.emplace_front();
	param.head   = head;
	param.tail   = tail;
}

void TrailShader::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texcoord)
{
	Vertex& vertex  = vertices.emplace_back();
	vertex.position = position;
	vertex.texcoord = texcoord;
}

void TrailShader::Begin(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection, int blend)
{
	// �V�F�[�_�[��ݒ�
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(),  nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// �萔�o�b�t�@�̍X�V
	CbScene cb;
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(projection);
	DirectX::XMStoreFloat4x4(&cb.viewProjection, V * P);
	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cb, 0, 0);

	// �萔�o�b�t�@��ݒ�
	dc->VSSetConstantBuffers(0, 1, sceneConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(0, 1, sceneConstantBuffer.GetAddressOf());

	// �v���~�e�B�u�E�g�|���W�[
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	dc->PSSetShaderResources(15, 1, texture->GetShaderResourceView().GetAddressOf());

}

void TrailShader::End(ID3D11DeviceContext* dc)
{
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
}