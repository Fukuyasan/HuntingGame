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

	// 定数バッファ
	// 
	struct CbPerFrame
	{
		DirectX::XMFLOAT4 LightDir;	   // ライトの方向
		DirectX::XMFLOAT4 LightColor;  // ライトの色
		DirectX::XMFLOAT4 EyePos;	   // カメラ位置
		//DirectX::XMFLOAT3 playerPos;   // プレイヤー位置(ディザリング用)
		//float dummy;
	};
	
	// シーン用
	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	lightDirection;
	};
			
	// メッシュ用
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	// サブセット用
	struct CbSubset
	{
		DirectX::XMFLOAT4 materialColor;
		int				  textureNarrow;
		float			  dummy[3];
	};

	// シャドウマップ用
	struct CbShadow
	{
		DirectX::XMFLOAT4X4 lightVP;
		DirectX::XMFLOAT3   shadowColor;
		float				shadowBias;
	};

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> perFrameConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> subsetConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowConstantBuffer;
};
