#pragma once

// ファイルをインクルード
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/DepthStencil.h"
#include "Graphics/RenderContext.h"
#include "Graphics/FullscreenQuad.h"

#include "Camera/CameraController.h"

// ゲームシーン
class SceneTest final : public Scene
{
public:
	SceneTest() {}
	~SceneTest() override = default;

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(const float& elapsedTime) override;

	// 描画処理
	void Render() override;
	void RenderShadowmap(ID3D11DeviceContext* dc);
	void Render3D(ID3D11DeviceContext* dc, const RenderContext& rc);
	void Render2D(ID3D11DeviceContext* dc);
	void RenderOffscreen(ID3D11DeviceContext* dc, const RenderContext& rc);

	// デバッグ描画
	void DrawDebugRenderer(ID3D11DeviceContext* dc, const RenderContext& rc);
	void DrawDebugGUI();
	void DrawDebugButton();

private:
	std::unique_ptr<CameraController> cameraController = nullptr;  // カメラコントローラ

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ditheringTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymapTexture    = nullptr;
	std::unique_ptr<Sprite> skymap                                    = nullptr;

	// オフスクリーン
	std::unique_ptr<FullscreeQuad> bitBlockTransfer;

	// ナビメッシュ

	// シャドウマップ用情報
	static inline const UINT SHADOWMAP_SIZE = 16382;  // シャドウマップのサイズ
	std::unique_ptr<DepthStencil> dsvShadowmap;
	float				shadowRect = 500.0f;
	DirectX::XMFLOAT4X4 lightVP;
	DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };
	float shadowBias = 0.0001f;

	float lightBias = -250.0f;

private:
	DirectX::XMFLOAT4 lightDir   = { -0.5f,-1.3f,-1.5f, 0 };
	DirectX::XMFLOAT4 lightColor = { 1, 1, 1, 1 };

	bool debugFlg      = false;
	bool showPrimitive = false;
	bool showUI        = true;
	bool showNavmesh   = false;

	// 世界を止める（カメラのみ動かせる）
	bool theWorld = false;

};