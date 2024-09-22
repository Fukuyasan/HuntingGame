#pragma once
#include <d3d11.h>
#include <wrl.h>

// ブレンドステート
enum class BLEND_MODE
{
	NONE,
	ALPHA,
	ADD,
	MULTIPRY,
	MAX
};

// デプスステンシルステート
enum class DEPTH_MODE
{
	ZT_ON_ZW_ON,
	ZT_ON_ZW_OFF,
	ZT_OFF_ZW_ON,
	ZT_OFF_ZW_OFF,
	MAX
};

// サンプラーステート
enum class SAMPLER_MODE
{
	POINT,
	LINEAR,
	ANISOTROPIC,
	LINER_BORDER_BLACK,
	LINER_BORDER_WHITE,
	LINEAR_MIRROR,
	MAX
};

// ラスタライザステート
enum class RASTERIZER_MODE
{
	SOLID_BACK,
	SOLID_NONE,
	WIREFRAME_BACK,
	WIREFRAME_NONE,
	MAX
};

class RenderState final
{
private:
	RenderState()  = default;
	~RenderState() = default;

public:
	RenderState(const RenderState&) = delete;
	RenderState(RenderState&&)      = delete;
	RenderState operator=(const RenderState&) = delete;
	RenderState operator=(RenderState&&)	  = delete;

public:
	// インスタンス取得
	static RenderState& Instance()
	{ 
		static RenderState instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 各ステート設定
	void SetBlend(ID3D11DeviceContext* dc, BLEND_MODE blendMode);
	void SetDepth(ID3D11DeviceContext* dc, DEPTH_MODE depthMode);
	void SetPSSampler(ID3D11DeviceContext* dc, UINT startSlot, SAMPLER_MODE samplerMode);
	void SetRasterizer(ID3D11DeviceContext* dc, RASTERIZER_MODE rasterizerMode);

private:
	
	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates[static_cast<int>(BLEND_MODE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStates[static_cast<int>(DEPTH_MODE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerStates[static_cast<int>(SAMPLER_MODE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerStates[static_cast<int>(RASTERIZER_MODE::MAX)];
};
