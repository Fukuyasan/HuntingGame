#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

namespace CreateShader
{
	// cso������ǂݍ���
	long LoadFile(const char* csoName,
		std::unique_ptr<unsigned char[]>& csoData);

	// ���_�V�F�[�_�[�̃��[�h�A���̓��C�A�E�g�̐���
	HRESULT CreateVSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
		UINT numElements);

	// �s�N�Z���V�F�[�_�[�̃��[�h
	HRESULT CreatePSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11PixelShader** pixelShader);

	// �W�I���g���V�F�[�_�[�̃��[�h
	HRESULT CreateGSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11GeometryShader** geometryShader);

	// �R���s���[�g�V�F�[�_�[�̃��[�h
	HRESULT CreateCSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11ComputeShader** computeShader);


	// �萔�o�b�t�@�̃��[�h
	HRESULT CreateCB(ID3D11Device* device,
		UINT size,
		ID3D11Buffer** buffer);
}