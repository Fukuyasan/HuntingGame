#pragma once
#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class PBRShader : public Shader
{
public:
	PBRShader(const char* csoName) { Load(csoName); }
	~PBRShader() override {}

	void Load(const char* csoName);

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	void Draw(ID3D11DeviceContext* dc, const Model* model) override;
	void End(ID3D11DeviceContext* dc) override;

private:
	static const int MaxBones = 128;

	// �萔�o�b�t�@
	// 
	struct CbPerFrame
	{
		DirectX::XMFLOAT4 LightDir;	   // ���C�g�̕���
		DirectX::XMFLOAT4 LightColor;  // ���C�g�̐F
		DirectX::XMFLOAT4 EyePos;	   // �J�����ʒu
		//DirectX::XMFLOAT3 playerPos;   // �v���C���[�ʒu(�f�B�U�����O�p)
		//float dummy;
	};
	
	// �V�[���p
	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	lightDirection;
	};
			
	// ���b�V���p
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	// �T�u�Z�b�g�p
	struct CbSubset
	{
		DirectX::XMFLOAT4 materialColor;
		int				  textureNarrow;
		float			  dummy[3];
	};

	// �V���h�E�}�b�v�p
	struct CbShadow
	{
		DirectX::XMFLOAT4X4 lightVP;
		DirectX::XMFLOAT3   shadowColor;
		float				shadowBias;
	};

	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> perFrameConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> subsetConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowConstantBuffer;
};
