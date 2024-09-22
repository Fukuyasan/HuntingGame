#pragma once
#include <d3d11.h>
#include <wrl.h>

#include <DirectXMath.h>
#include "System/Math/Mathf.h"

class PostEffect
{
	using CBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;

private:
	PostEffect()  = default;
	~PostEffect() = default;
public:
	PostEffect(const PostEffect&) = delete;
	PostEffect(PostEffect&&)	  = delete;
	PostEffect operator=(const PostEffect&) = delete;
	PostEffect operator=(PostEffect&&)		= delete;

	// インスタンス取得
	static PostEffect& Instance()
	{ 
		static PostEffect instance;
		return instance;	
	}

	// 初期化
	void Initialize();

	// 定数バッファ更新
	void Update(const float& elapsedTime);

	void DrawDebubGUI();
public:
	void UpdateBloom(ID3D11DeviceContext* dc);
	void UpdateHeatHaze(ID3D11DeviceContext* dc);
	void UpdateShockBlur(ID3D11DeviceContext* dc);

public:
	// ブラーの位置と強さ設定
	void SetBlurParam(const float power, const float time) { 
		shockBlur.blurPower = power; 
		BlurTime            = time;
		totalBlurTime       = 0.0f;
	}
	void SetBlurPos(const DirectX::XMFLOAT3& pos);

	void SetColorBalance(const DirectX::XMFLOAT4& color)
	{ 
		heatHaze.colorBalance[0] = color.x;
		heatHaze.colorBalance[1] = color.y;
		heatHaze.colorBalance[2] = color.z;
		heatHaze.colorBalance[3] = color.w;
	}

	void LerpColorBalance(const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& color2, const float lerpRate)
	{
		heatHaze.colorBalance[0] = Mathf::lerp(color.x, color2.x, lerpRate);
		heatHaze.colorBalance[1] = Mathf::lerp(color.y, color2.y, lerpRate);
		heatHaze.colorBalance[2] = Mathf::lerp(color.z, color2.z, lerpRate);
		heatHaze.colorBalance[3] = Mathf::lerp(color.w, color2.w, lerpRate);
	}
private:

	// ブルーム
	struct CBBloom
	{
		float min = 0.0f;
		float max = 1.0f;
		float gaussianSigma = 2.0f;
		float bloomIntensity = 0.5f;
		float exposure = 1.2f;
		float dummy[3];
	};
	CBBloom bloom;
	CBuffer cbBloom;

	// 陽炎
	struct CBHeatHaze
	{
		float amplitude       = 0.15f;
		float depthOffset     = 0.0f;
		float timeScale       = 0.1f;
		float seedScale       = 0.3f;
		float colorBalance[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float time;
		float dummy[3];
	};
	CBHeatHaze heatHaze;
	CBuffer	   cbHeatHaze;

	// ラジアルブラー
	struct CbShockBlur
	{
		DirectX::XMFLOAT2 centerTexel = { 0.5f, 0.5f };
		float blurPower;
		float dummy;
	};
	CbShockBlur shockBlur;
	CBuffer		cbShockBlur;

	float totalBlurTime = 0.0f;
	float BlurTime		= 0.0f;
};
