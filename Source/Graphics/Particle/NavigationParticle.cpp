#include "NavigationParticle.h"
#include "Graphics/CreateShader.h"
#include "System/Misc.h"

// �o�b�t�@�Ȃǂ̐ݒ�
NavigationParticle::NavigationParticle(size_t particleCount) : maxParticleCount(particleCount)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	HRESULT hr = S_OK;

	// �p�[�e�B�N���̃X�e�[�^�X�̃o�b�t�@
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth           = static_cast<UINT>(sizeof(ParticleStatus) * particleCount);
	bufferDesc.StructureByteStride = sizeof(ParticleStatus);
	bufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags      = 0;
	bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&bufferDesc, nullptr, particleBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// �V�F�[�_�[���\�[�X�r���[
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format               = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements   = static_cast<UINT>(particleCount);

	hr = device->CreateShaderResourceView(particleBuffer.Get(), &srvDesc, srvParticle.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// �A���I�[�_�[�h�A�N�Z�X�r���[
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format              = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements  = static_cast<UINT>(particleCount);
	uavDesc.Buffer.Flags        = 0;

	hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, uavParticle.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// �萔�o�b�t�@
	CreateShader::CreateCB(device, sizeof(CBParticle), constantBuffer.GetAddressOf());
	CreateShader::CreateCB(device, sizeof(CBScene), sceneConstantBuffer.GetAddressOf());

	// ���_�V�F�[�_�[
	CreateShader::CreateVSFromCso(device, "Shader\\ParticleBaseVS.cso", vsParticle.ReleaseAndGetAddressOf(), NULL, NULL, NULL);
	// �s�N�Z���V�F�[�_�[
	CreateShader::CreatePSFromCso(device, "Shader\\ParticleBasePS.cso", psParticle.ReleaseAndGetAddressOf());
	// �W�I���g���V�F�[�_�[
	CreateShader::CreateGSFromCso(device, "Shader\\ParticleBaseGS.cso", gsParticle.ReleaseAndGetAddressOf());
	// �R���s���[�g�V�F�[�_�[
	CreateShader::CreateCSFromCso(device, "Shader\\Navigation\\NavigationInitializeParticleCS.cso", csInitializeParticle.ReleaseAndGetAddressOf());
	CreateShader::CreateCSFromCso(device, "Shader\\Navigation\\NavigationParticleCS.cso",			  csParticle.ReleaseAndGetAddressOf());
}

// �p�[�e�B�N���̏����ݒ�
void NavigationParticle::Initialize(const float& elapsedTime, const DirectX::XMFLOAT3& position)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// �A���I�[�_�[�A�N�Z�X�r���[���R���s���[�g�V�F�[�_�[�ɃZ�b�g
	dc->CSSetUnorderedAccessViews(0, 1, uavParticle.GetAddressOf(), NULL);

	// �萔�o�b�t�@�̐ݒ�
	particleData.emitterPosition = position;
	particleData.direction       = direction;
	particleData.particleSize    = 0.04f;

	particleData.time			+= elapsedTime;
	particleData.deltaTime       = elapsedTime;
	
	// �ݒ肵���f�[�^���R���s���[�g�V�F�[�_�[�ɑ���
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
	dc->CSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

	// �R���s���[�g�V�F�[�_�[�̃Z�b�g
	dc->CSSetShader(csInitializeParticle.Get(), NULL, 0);

	// �X���b�h�̂��߂Ƀf�B�X�p�b�`�Ƀ��������m�ۂ���i�����j
	const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount), NUMTHREADS_X) / NUMTHREADS_X;
	dc->Dispatch(threadGroupCountX, 1, 1);

	// ��n��
	ID3D11UnorderedAccessView* uavNull{};
	dc->CSSetUnorderedAccessViews(0, 1, &uavNull, NULL);
}

// �p�[�e�B�N���̍X�V�ݒ�
void NavigationParticle::Update(const float& elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// �A���I�[�_�[�A�N�Z�X�r���[���R���s���[�g�V�F�[�_�[�ɃZ�b�g
	dc->CSSetUnorderedAccessViews(0, 1, uavParticle.GetAddressOf(), NULL);

	// �萔�o�b�t�@�̐ݒ�
	particleData.targetPosition  = targetPos;
	particleData.time			+= elapsedTime;
	particleData.deltaTime       = elapsedTime;

	// �ݒ肵���f�[�^���R���s���[�g�V�F�[�_�[�ɑ���
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
	dc->CSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

	// �R���s���[�g�V�F�[�_�[�̃Z�b�g
	dc->CSSetShader(csParticle.Get(), NULL, 0);

	// �X���b�h�̂��߂Ƀf�B�X�p�b�`�Ƀ��������m�ۂ���i�����j
	const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount), NUMTHREADS_X) / NUMTHREADS_X;
	dc->Dispatch(threadGroupCountX, 1, 1);

	// ��n��
	ID3D11UnorderedAccessView* uavNull{};
	dc->CSSetUnorderedAccessViews(0, 1, &uavNull, NULL);
}

// �p�[�e�B�N���̕`��
void NavigationParticle::Render(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// �e�V�F�[�_�[�̃Z�b�g
	dc->VSSetShader(vsParticle.Get(), nullptr, 0);
	dc->PSSetShader(psParticle.Get(), nullptr, 0);
	dc->GSSetShader(gsParticle.Get(), nullptr, 0);
	dc->GSSetShaderResources(9, 1, srvParticle.GetAddressOf());

	// �萔�o�b�t�@�̃Z�b�g
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
	dc->VSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

	// �V�[���̃J����
	CBScene scene;
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMStoreFloat4x4(&scene.viewProjection, V * P);

	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &scene, 0, 0);
	dc->VSSetConstantBuffers(4, 1, sceneConstantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(4, 1, sceneConstantBuffer.GetAddressOf());

	// ���̓��C�A�E�g�i����͖����j
	dc->IASetInputLayout(nullptr);
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	// �|�C���g���X�g���g�p
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(static_cast<UINT>(maxParticleCount), 0);

	// ��n��
	ID3D11ShaderResourceView* srvNull{};
	dc->GSSetShaderResources(9, 1, &srvNull);
	dc->GSSetShaderResources(4, 1, &srvNull);
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
}
