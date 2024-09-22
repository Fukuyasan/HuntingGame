#include "SceneTest.h"

#include "Graphics/Graphics.h"
#include "Graphics/RenderState.h"
#include "Graphics/ShaderState.h"
#include "Graphics/CreateShader.h"
#include "Graphics/Renderer/NavMeshRenderer.h"

#include "Camera/Camera.h"

#include "AI/MetaAI/MetaAI.h"

#include "GameObject/GameObject.h"
#include "GameObject/GameObjectManager.h"

#include "Component/Weapon/Sword.h"
#include "Component/Weapon/Hammer.h"
#include "Component/Stage/StageMain.h"
#include "Component/Animator/Animator.h"
#include "Component/Movement/Movement.h"
#include "Component/Character/Otomo/Otomo.h"
#include "Component/Character/Player/player.h"
#include "Component/Character/Enemy/Dragon/Dragon.h"

void SceneTest::Initialize()
{
	Graphics& graphics = Graphics::Instance();

	// メタAI
	MetaAI::Instance().Initialize();

	// ステージ
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/Stage/model/Terrain.mdl");
		gameObject->SetName("Stage");
		gameObject->transform.SetPosition({ 0.0f, 0.0f, 0.0f });
		gameObject->transform.SetScale(0.02f);
		gameObject->SetLayer(Layer::Stage);
		gameObject->AddComponent<StageMain>();
	}
	// プレイヤー
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/Player/HunterTest.mdl");
		gameObject->GetModel()->LoadAnimation("Data/Model/Player/HunterTest.anim");
		gameObject->GetModel()->SetupRootMotion("mixamorig:Hips");
		gameObject->SetName("Player");
		gameObject->SetLayer(Layer::Player);
		gameObject->AddComponent<Movement>();
		gameObject->AddComponent<Animator>();
		gameObject->AddComponent<Player>();
	}
	// 剣
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/Sword/Sword.mdl");
		gameObject->SetName("Sword");
		gameObject->SetLayer(Layer::Weapon);
		gameObject->AddComponent<Sword>();
	}
	// オトモ
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/SDUnityChan/SDUnityChan.mdl");
		gameObject->GetModel()->LoadAnimation("Data/Model/SDUnityChan/SDUnityChan.anim");
		gameObject->GetModel()->SetupRootMotion("Character1_Hips");
		gameObject->SetName("Otomo");
		gameObject->SetLayer(Layer::Otomo);
		gameObject->AddComponent<Movement>();
		gameObject->AddComponent<Animator>();
		gameObject->AddComponent<Otomo>();
	}
	// ハンマー
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/Hammer/Hammer.mdl");
		gameObject->SetName("Hammer");
		gameObject->SetLayer(Layer::Weapon);
		gameObject->AddComponent<Hammer>();
	}
	// カメラ
	{
		Camera& camera = Camera::Instance();
		camera.SetLookAt(
			DirectX::XMFLOAT3(0, 1, -10),
			DirectX::XMFLOAT3(0, 1, 0),
			DirectX::XMFLOAT3(0, 1, 0)
		);
		camera.SetPerspectiveFov(
			DirectX::XMConvertToRadians(45),
			graphics.GetScreenWidth() / graphics.GetScreenHeight(),
			0.1f,
			10000.0f
		);

		// カメラコントローラ初期化
		cameraController = std::make_unique<CameraController>();
		cameraController->RegisterRaycastModels(*GameObjectManager::Instance().Find("Player")->GetModel());
		cameraController->SetOwner(GameObjectManager::Instance().Find("Player"));
		cameraController->SetTarget(GameObjectManager::Instance().Find("Dragon"));
	}

	{
		// スカイマップ
		const char* skymapPath = "Data/Sprite/drakensberg_solitary_mountain_4k.DDS";
		skymap = std::make_unique<Sprite>(skymapPath);

		// シャドウマップ
		dsvShadowmap = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	}
}

void SceneTest::Finalize()
{

}

void SceneTest::Update(const float& elapsedTime)
{
	// めたAI更新
	MetaAI::Instance().Update(elapsedTime);

	GameObjectManager::Instance().Update();

	cameraController->Update(elapsedTime);
}

void SceneTest::Render()
{
	Graphics& graphics = Graphics::Instance();

	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// サンプラーステート設定
	rs.SetPSSampler(dc, 0, SAMPLER_MODE::POINT);
	rs.SetPSSampler(dc, 1, SAMPLER_MODE::LINEAR);
	rs.SetPSSampler(dc, 2, SAMPLER_MODE::ANISOTROPIC);
	rs.SetPSSampler(dc, 3, SAMPLER_MODE::LINER_BORDER_BLACK);
	rs.SetPSSampler(dc, 4, SAMPLER_MODE::LINER_BORDER_WHITE);
	rs.SetPSSampler(dc, 5, SAMPLER_MODE::LINEAR_MIRROR);

	// シャドウマップの描画
	RenderShadowmap(dc);

	// 画面クリア＆レンダーターゲット設定
	graphics.Clear(0.0f, 0.0f, 0.5f);

	// 描画処理
	RenderContext rc{};
	rc.lightDirection = lightDir;    // ライト方向（下方向）
	rc.lightColor	  = lightColor;  // ライト色

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();

	// シャドウマップの設定
	rc.shadowmapData.shadowmap   = dsvShadowmap->GetSRV().Get();
	rc.shadowmapData.lightVP     = lightVP;
	rc.shadowmapData.shadowColor = shadowColor;
	rc.shadowmapData.shadowBias  = shadowBias;

	// カメラ位置
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;

	// ビュー行列
	rc.view = camera.GetView();
	// プロジェクション行列
	rc.projection = camera.GetProjection();

	// ゲームシーンのビューポートの設定
	D3D11_VIEWPORT vp = {};
	vp.Width	= graphics.GetScreenWidth();
	vp.Height	= graphics.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	Render3D(dc, rc);

	Render2D(dc);
}

void SceneTest::RenderShadowmap(ID3D11DeviceContext* dc)
{
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = dsvShadowmap->GetDSV().Get();

	// 画面クリア
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// レンダーターゲット設定
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(dsvShadowmap->GetWidth());
	vp.Height = static_cast<float>(dsvShadowmap->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	RenderContext rc{};

	DirectX::XMFLOAT3 LightDirection = { lightDir.x,lightDir.y,lightDir.z };

	//カメラパラメータ設定	
	// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を作成
	DirectX::XMVECTOR lightPosition = DirectX::XMLoadFloat3(&LightDirection);
	lightPosition = DirectX::XMVectorScale(lightPosition, lightBias);

	// シャドウマップに描画したい範囲の射影行列を生成
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
		lightPosition,
		DirectX::XMVectorSet(0, 0, 0, 0),
		DirectX::XMVectorSet(0, 1, 0, 0)
	);
	DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowRect, shadowRect, 0.1f, 1000.0f);

	DirectX::XMStoreFloat4x4(&rc.view, V);
	DirectX::XMStoreFloat4x4(&rc.projection, P);

	DirectX::XMStoreFloat4x4(&lightVP, V * P);

	// シャドウマップ描画
	RenderState& rs = RenderState::Instance();
	ShaderState& ss = ShaderState::Instance();
	Shader* shadow = ss.GetShadowmapShader();

	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

	// 影の描画
	GameObjectManager::Instance().Render(rc, shadow);
}

void SceneTest::Render3D(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance();

	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

#pragma region スカイマップ描画
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_OFF);
	rs.SetBlend(dc, BLEND_MODE::NONE);
	{
		SkymapShader* shader = ss.GetSkymapShader();

		shader->Begin(dc, rc);
		shader->Draw(dc, skymap.get());
		shader->End(dc);
	}
#pragma endregion

	Shader* shader = ss.GetModelShader(MODEL::DefaultPBR);

	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
	rs.SetBlend(dc, BLEND_MODE::ALPHA);

	GameObjectManager::Instance().Render(rc, shader);
}

void SceneTest::Render2D(ID3D11DeviceContext* dc)
{
	GameObjectManager::Instance().OnGUI();
}

void SceneTest::RenderOffscreen(ID3D11DeviceContext* dc, const RenderContext& rc)
{
}

void SceneTest::DrawDebugRenderer(ID3D11DeviceContext* dc, const RenderContext& rc)
{
}

void SceneTest::DrawDebugGUI()
{
}

void SceneTest::DrawDebugButton()
{
}
