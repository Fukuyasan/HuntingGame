#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class ShadowmapCasterShader : public Shader
{
public:
	ShadowmapCasterShader(ID3D11Device* device);
	~ShadowmapCasterShader() override = default;

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	void Draw(ID3D11DeviceContext* dc, const Model* model) override;
	void End(ID3D11DeviceContext* dc) override;

private:
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> meshConstantBuffer;
};
