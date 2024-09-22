#include "PBRShader.h"
#include "System/Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/CreateShader.h"
#include "Component/Character/Player/player.h"

void PBRShader::Load(const char* csoName)
{
	HRESULT hr           = S_OK;
	Graphics& graphics   = Graphics::Instance();
	ID3D11Device* device = graphics.GetDevice();

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	// CSOのファイル名を取得
	std::string shader = "Shader\\";
	std::string name   = csoName;
	std::string ps     = "PBRShaderPS.cso";

	std::string psName = shader + name + ps;
	
	// シェーダー
	CreateShader::CreateVSFromCso(device, "Shader\\PBRShaderVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	CreateShader::CreatePSFromCso(device, psName.c_str(), pixelShader.ReleaseAndGetAddressOf());

	// 定数バッファ
	CreateShader::CreateCB(device, sizeof(CbScene), sceneConstantBuffer.ReleaseAndGetAddressOf());
	CreateShader::CreateCB(device, sizeof(CbMesh), meshConstantBuffer.ReleaseAndGetAddressOf());
	CreateShader::CreateCB(device, sizeof(CbSubset), subsetConstantBuffer.ReleaseAndGetAddressOf());
	CreateShader::CreateCB(device, sizeof(CbPerFrame), perFrameConstantBuffer.ReleaseAndGetAddressOf());
	CreateShader::CreateCB(device, sizeof(CbShadow), shadowConstantBuffer.ReleaseAndGetAddressOf());
}

// 描画開始
void PBRShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		meshConstantBuffer.Get(),
		subsetConstantBuffer.Get(),
		perFrameConstantBuffer.Get(),
		shadowConstantBuffer.Get()
	};
	dc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	dc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	// シーン用定数バッファ更新
	CbScene cbScene;
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

	cbScene.lightDirection = rc.lightDirection;
	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);

	CbPerFrame cbPerFrame;
	cbPerFrame.LightDir	  = rc.lightDirection;
	cbPerFrame.LightColor = rc.lightColor;
	cbPerFrame.EyePos	  = rc.viewPosition;
	//cbPerFrame.playerPos  = Player::Instance().GetPosition();
	dc->UpdateSubresource(perFrameConstantBuffer.Get(), 0, 0, &cbPerFrame, 0, 0);

	// シャドウマップ用定数バッファ更新
	CbShadow cbShadow;
	cbShadow.lightVP     = rc.shadowmapData.lightVP;
	cbShadow.shadowColor = rc.shadowmapData.shadowColor;
	cbShadow.shadowBias  = rc.shadowmapData.shadowBias;

	dc->UpdateSubresource(shadowConstantBuffer.Get(), 0, 0, &cbShadow, 0, 0);

	dc->PSSetShaderResources(15, 1, &rc.shadowmapData.shadowmap);
}

// 描画
void PBRShader::Draw(ID3D11DeviceContext* dc, const Model* model)
{
	const ModelResource* resource = model->GetResource();
	const auto& nodes             = model->GetNodes();

	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュ用定数バッファ更新
		CbMesh cbMesh;
		::memset(&cbMesh, 0, sizeof(cbMesh));
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX worldTransform  = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX boneTransform   = offsetTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&cbMesh.boneTransforms[i], boneTransform);
			}
		}
		else
		{
			cbMesh.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		}
		dc->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			ID3D11ShaderResourceView* srvs[] =
			{
				subset.material->albedoMap.Get(),
				subset.material->normalMap.Get(),
				subset.material->metallicMap.Get(),
				subset.material->roughnessMap.Get(),
				subset.material->ambientOcclusionMap.Get()
			};
			dc->PSSetShaderResources(0, _countof(srvs), srvs);

			CbSubset cbSubset;
			cbSubset.materialColor = subset.material->color;
			cbSubset.textureNarrow = subset.material->textureNarrow;
			dc->UpdateSubresource(subsetConstantBuffer.Get(), 0, 0, &cbSubset, 0, 0);

			dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
	}
}

// 描画終了
void PBRShader::End(ID3D11DeviceContext* dc)
{
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);

	ID3D11ShaderResourceView* srvs[] =
	{
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);

	ID3D11ShaderResourceView* srv = nullptr;
	dc->PSSetShaderResources(15, 1, &srv);
}
