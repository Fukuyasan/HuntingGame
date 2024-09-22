#pragma once

// ファイルをインクルード
#include "Scene.h"

#include "Audio\Audio.h"

#include "Component/Character/Player/player.h"
#include "Component/Character/Enemy/Dragon/Dragon.h"
#include "Component/Character/Enemy/Wyvern/Wyvern.h"

#include "Camera/CameraController.h"

#include "Graphics/Sprite.h"
#include "Graphics/FullscreenQuad.h"
#include "Graphics/Renderer/NavMeshRenderer.h"
#include "Graphics/DepthStencil.h"
#include "Graphics/RenderContext.h"

#include "AI/NavMesh/NavMesh.h"
#include "AI/NavMesh/Solo/SoloMesh.h"
#include "AI/NavMesh/NavMeshAgent.h"

#include "UI/UIManager.h"
#include "UI/Gauge/UIGauge.h"

//#define UseSceneGame

#ifdef UseSceneGame
// ゲームシーン
class SceneGame final : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override = default;

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
	std::unique_ptr<Player> player = nullptr;  // プレイヤー
	Dragon* dragon                 = nullptr;  // ドラゴン
	Wyvern*	wyvern                 = nullptr;  // ワイバーン

	std::unique_ptr<CameraController> cameraController = nullptr;  // カメラコントローラ

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ditheringTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymapTexture	  = nullptr;
	std::unique_ptr<Sprite> skymap = nullptr;

	// オフスクリーン
	std::unique_ptr<FullscreeQuad> bitBlockTransfer;

	// ナビメッシュ
	std::unique_ptr<NavMesh> navmesh;
	std::shared_ptr<NavMeshAgent> agent;

	// シャドウマップ用情報
	static const UINT SHADOWMAP_SIZE = 16382;  // シャドウマップのサイズ
	std::unique_ptr<DepthStencil> dsvShadowmap;
	float				shadowRect = 500.0f;
	DirectX::XMFLOAT4X4 lightVP;
	DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };
	float shadowBias = 0.0001f;

	float lightBias = -250.0f;

private:
	DirectX::XMFLOAT4 lightDir   = { -0.5f,-1.3f,-1.5f, 0 };
	DirectX::XMFLOAT4 lightColor = { 3, 3, 3, 1 };

	bool debugFlg       = false;
	bool showPrimitive  = false;
	bool showUI         = true;
	bool showNavmesh    = false;

	// 世界を止める（カメラのみ動かせる）
	bool theWorld = false;
	
public:
	std::unique_ptr<AudioSource> gameBGM = nullptr;
};
#endif