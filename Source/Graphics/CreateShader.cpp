#include <d3d11.h>
#include <wrl.h>
#include <sstream>

#include "CreateShader.h"
#include "System/Misc.h"

// cso������ǂݍ���
long CreateShader::LoadFile(const char* csoName, std::unique_ptr<unsigned char[]>& csoData)
{
	FILE* fp = nullptr;

	// csoName�̃|�C���^���󂯎��
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���|�C���^��0�o�C�g�̒n�_����I�[�܂ňړ�������
	fseek(fp, 0, SEEK_END);
	// �t�@�C���|�C���^�̃T�C�Y���擾
	long csoSize = ftell(fp);
	// �t�@�C���|�C���^���[����0�o�C�g�̒n�_�܂ňړ�������
	fseek(fp, 0, SEEK_SET);

	// �t�@�C���|�C���^�̃T�C�Y�̂P�o�C�g�܂œǂݍ��݁AcsoData�Ɋi�[����
	csoData = std::make_unique<unsigned char[]>(csoSize);
	fread(csoData.get(), csoSize, 1, fp);
	fclose(fp);

	return csoSize;
}

// ���_�V�F�[�_�[�̃��[�h�A���̓��C�A�E�g�̐���
HRESULT CreateShader::CreateVSFromCso(ID3D11Device* device,
	const char* csoName,
	ID3D11VertexShader** vertexShader,
	ID3D11InputLayout** inputLayout,
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
	UINT numElements)
{
	// �t�@�C�����[�h
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);

	HRESULT hr = device->CreateVertexShader(
		csoData.get(),
		csoSize,
		nullptr,
		vertexShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	
	// ���̓��C�A�E�g�̐���
	if (inputLayout)
	{		
		hr = device->CreateInputLayout(inputElementDesc, numElements, csoData.get(), csoSize, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	return hr;
}

// �s�N�Z���V�F�[�_�I�u�W�F�N�g�̐���
HRESULT CreateShader::CreatePSFromCso(ID3D11Device* device,
	const char* csoName,
	ID3D11PixelShader** pixelShader)
{
	// �t�@�C�����[�h
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);
		
	HRESULT hr = device->CreatePixelShader(
		csoData.get(),
		csoSize,
		nullptr,
		pixelShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// �W�I���g���V�F�[�_�[�I�u�W�F�N�g�̐���
HRESULT CreateShader::CreateGSFromCso(ID3D11Device* device, const char* csoName, ID3D11GeometryShader** geometryShader)
{
	// �t�@�C�����[�h
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);

	HRESULT hr = device->CreateGeometryShader(
		csoData.get(),
		csoSize,
		nullptr,
		geometryShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;	
}

// �R���s���[�g�V�F�[�_�[�I�u�W�F�N�g�̐���
HRESULT CreateShader::CreateCSFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** computeShader)
{
	// �t�@�C�����[�h
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);

	HRESULT hr = device->CreateComputeShader(
		csoData.get(),
		csoSize,
		nullptr,
		computeShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// �V�[���p�o�b�t�@�̍쐬
HRESULT CreateShader::CreateCB(ID3D11Device* device, UINT size, ID3D11Buffer** buffer)
{
	
	D3D11_BUFFER_DESC desc;
	::memset(&desc, 0, sizeof(desc));
	desc.ByteWidth           = size;
	desc.Usage               = D3D11_USAGE_DEFAULT;
	desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags      = 0;
	desc.MiscFlags           = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}