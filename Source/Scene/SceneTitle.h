#pragma once
#include "Scene.h"
#include "Graphics\DepthStencil.h"

#include "UI\Button\UIButton.h"

#include "Camera\CameraController.h"

#include "Graphics/FullscreenQuad.h"

#include "AI/NavMesh/NavMesh.h"
#include "AI/NavMesh/Solo/SoloMesh.h"
#include "AI/NavMesh/NavMeshAgent.h"

#ifdef UseTitle

// タイトルシーン
class SceneTitle : public Scene
{
public:
    SceneTitle()  = default;           // コンストラクタ
    ~SceneTitle() override = default;  // デストラクタ

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
    void Render2D(ID3D11DeviceContext* dc, const RenderContext& rc);
    void DrawDebugGUI();

    void RenderOffscreen(ID3D11DeviceContext* dc, const RenderContext& rc);

    void SetRenderContext(RenderContext& rc);

private:
    // ナビメッシュ
    std::unique_ptr<NavMesh> navmesh;
    std::unique_ptr<SoloMesh> navMesh;
    std::unique_ptr<Model> navModel;
    std::shared_ptr<NavMeshAgent> agent;

private:
    // シャドウマップのサイズ
    static const UINT SHADOWMAP_SIZE = 2048;

    // ライト用情報
    DirectX::XMFLOAT4 lightDir   = { -0.5f,-1.3f,-1.5f, 0 };
    DirectX::XMFLOAT4 lightColor = { 2, 2, 2, 1 };

    // シャドウマップ用情報
    std::unique_ptr<DepthStencil> dsvShadowmap;
    float shadowRect = 500.0f;
    DirectX::XMFLOAT4X4 lightVP;
    DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };
    float shadowBias = 0.0001f;
    float lightBias  = -250.0f;

    std::unique_ptr<UIButton> titleLogo = nullptr;
    std::unique_ptr<UIButton> key       = nullptr;

    std::unique_ptr<CameraController> controller = nullptr;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> maskSRV = nullptr;
    std::unique_ptr<Sprite> skymap = nullptr;

    std::unique_ptr<FullscreeQuad> bitBlockTransfer = nullptr;

    // カメラ用デバッグ
    DirectX::XMFLOAT3 eye   = { 127.280f,  6.954f, 148.576f };
    DirectX::XMFLOAT3 focus = { 113.274f, 15.697f, 171.503f };
    DirectX::XMFLOAT3 up    = { 0.161f,    0.951f,  -0.264f };

private:
    using CBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;

    struct CBMask
    {
        float			  dissolveThreshold;  // ディゾルブ量
        float			  edgeThreshold; 	  // 縁の閾値
        DirectX::XMFLOAT2 dummy;

        DirectX::XMFLOAT4 edgeColor;		  // 縁の色
    };
    CBMask mask;

    CBuffer cbMask; 	// 定数バッファ
};

#endif