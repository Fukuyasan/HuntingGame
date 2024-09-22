#include "NavigationParticle.h"
#include "Graphics/CreateShader.h"
#include "System/Misc.h"

// バッファなどの設定
NavigationParticle::NavigationParticle(size_t particleCount) : maxParticleCount(particleCount)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	HRESULT hr = S_OK;

	// パーティクルのステータスのバッファ
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth           = static_cast<UINT>(sizeof(ParticleStatus) * particleCount);
	bufferDesc.StructureByteStride = sizeof(ParticleStatus);
	bufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags      = 0;
	bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&bufferDesc, nullptr, particleBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// シェーダーリソースビュー
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format               = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements   = static_cast<UINT>(particleCount);

	hr = device->CreateShaderResourceView(particleBuffer.Get(), &srvDesc, srvParticle.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// アンオーダードアクセスビュー
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format              = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements  = static_cast<UINT>(particleCount);
	uavDesc.Buffer.Flags        = 0;

	hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, uavParticle.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 定数バッファ
	CreateShader::CreateCB(device, sizeof(CBParticle), constantBuffer.GetAddressOf());
	CreateShader::CreateCB(device, sizeof(CBScene), sceneConstantBuffer.GetAddressOf());

	// 頂点シェーダー
	CreateShader::CreateVSFromCso(device, "Shader\\ParticleBaseVS.cso", vsParticle.ReleaseAndGetAddressOf(), NULL, NULL, NULL);
	// ピクセルシェーダー
	CreateShader::CreatePSFromCso(device, "Shader\\ParticleBasePS.cso", psParticle.ReleaseAndGetAddressOf());
	// ジオメトリシェーダー
	CreateShader::CreateGSFromCso(device, "Shader\\ParticleBaseGS.cso", gsParticle.ReleaseAndGetAddressOf());
	// コンピュートシェーダー
	CreateShader::CreateCSFromCso(device, "Shader\\Navigation\\NavigationInitializeParticleCS.cso", csInitializeParticle.ReleaseAndGetAddressOf());
	CreateShader::CreateCSFromCso(device, "Shader\\Navigation\\NavigationParticleCS.cso",			  csParticle.ReleaseAndGetAddressOf());
}

// パーティクルの初期設定
void NavigationParticle::Initialize(const float& elapsedTime, const DirectX::XMFLOAT3& position)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// アンオーダーアクセスビューをコンピュートシェーダーにセット
	dc->CSSetUnorderedAccessViews(0, 1, uavParticle.GetAddressOf(), NULL);

	// 定数バッファの設定
	particleData.emitterPosition = position;
	particleData.direction       = direction;
	particleData.particleSize    = 0.04f;

	particleData.time			+= elapsedTime;
	particleData.deltaTime       = elapsedTime;
	
	// 設定したデータをコンピュートシェーダーに送る
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
	dc->CSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

	// コンピュートシェーダーのセット
	dc->CSSetShader(csInitializeParticle.Get(), NULL, 0);

	// スレッドのためにディスパッチにメモリを確保する（多分）
	const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount), NUMTHREADS_X) / NUMTHREADS_X;
	dc->Dispatch(threadGroupCountX, 1, 1);

	// 後始末
	ID3D11UnorderedAccessView* uavNull{};
	dc->CSSetUnorderedAccessViews(0, 1, &uavNull, NULL);
}

// パーティクルの更新設定
void NavigationParticle::Update(const float& elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// アンオーダーアクセスビューをコンピュートシェーダーにセット
	dc->CSSetUnorderedAccessViews(0, 1, uavParticle.GetAddressOf(), NULL);

	// 定数バッファの設定
	particleData.targetPosition  = targetPos;
	particleData.time			+= elapsedTime;
	particleData.deltaTime       = elapsedTime;

	// 設定したデータをコンピュートシェーダーに送る
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
	dc->CSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

	// コンピュートシェーダーのセット
	dc->CSSetShader(csParticle.Get(), NULL, 0);

	// スレッドのためにディスパッチにメモリを確保する（多分）
	const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount), NUMTHREADS_X) / NUMTHREADS_X;
	dc->Dispatch(threadGroupCountX, 1, 1);

	// 後始末
	ID3D11UnorderedAccessView* uavNull{};
	dc->CSSetUnorderedAccessViews(0, 1, &uavNull, NULL);
}

// パーティクルの描画
void NavigationParticle::Render(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// 各シェーダーのセット
	dc->VSSetShader(vsParticle.Get(), nullptr, 0);
	dc->PSSetShader(psParticle.Get(), nullptr, 0);
	dc->GSSetShader(gsParticle.Get(), nullptr, 0);
	dc->GSSetShaderResources(9, 1, srvParticle.GetAddressOf());

	// 定数バッファのセット
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
	dc->VSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

	// シーンのカメラ
	CBScene scene;
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMStoreFloat4x4(&scene.viewProjection, V * P);

	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &scene, 0, 0);
	dc->VSSetConstantBuffers(4, 1, sceneConstantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(4, 1, sceneConstantBuffer.GetAddressOf());

	// 入力レイアウト（今回は無し）
	dc->IASetInputLayout(nullptr);
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	// ポイントリストを使用
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(static_cast<UINT>(maxParticleCount), 0);

	// 後始末
	ID3D11ShaderResourceView* srvNull{};
	dc->GSSetShaderResources(9, 1, &srvNull);
	dc->GSSetShaderResources(4, 1, &srvNull);
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
}
