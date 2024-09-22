#include "SceneGame.h"

#include "SceneTitle.h"
#include "SceneLoding.h"
#include "SceneManager.h"

#include "Graphics/Graphics.h"
#include "Graphics/RenderState.h"
#include "Graphics/ShaderState.h"
#include "Graphics/CreateShader.h"

#include "Camera/Camera.h"

#include "Component/Character/Enemy/Stego/Stego.h"
#include "Component/Character/Enemy/EnemyManager.h"

#include "System/Effect/EffectManager.h"
#include "System/Input/Input.h"

#include "Component/Stage/StageMain.h"

#include "Graphics/Texture.h"
#include "Graphics/PostEffect.h"

#include "AI/MetaAI/MetaAI.h"

#include "Manager/GameManager.h"

#define DragonFlg 1

#define UsePostEffect 1

#ifdef UseSceneGame 
// 初期化
void SceneGame::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	StageManager& stageManager = StageManager::Instance();
	EnemyManager& enemyManager = EnemyManager::Instance();

#pragma region ステージ
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
#pragma endregion
	
#pragma region MetaAI
	MetaAI::Instance().Initialize();
#pragma endregion

#pragma region ナビメッシュ
	// ナビメッシュ初期化
	navmesh = std::make_unique<SoloMesh>("navMeshData.bin");

	// エージェント初期化
	agent = std::make_unique<NavMeshAgent>(navmesh.get());
#pragma endregion

#pragma region プレイヤー
	player = std::make_unique<Player>();
#pragma endregion

#pragma region ボスモンスター
	const int dragonCount = 1;
	for (int i = 0; i < dragonCount; ++i)
	{
#if DragonFlg
		dragon = new Dragon();

		// エージェント登録
		AgentParams ap{};
		ap.radius          = 7.0f;
		ap.height          = 5.0f;
		ap.maxSpeed        = 20.0f;
		ap.maxAcceleration = ap.maxSpeed;

		// エージェントセット
		agent->AddAgent(dragon->transform.GetPosition(), ap);
		dragon->SetAgent(agent, i);

		enemyManager.Register(dragon);

#else
		wyvern = new Wyvern();

		// エージェント登録
		AgentParams ap{};
		ap.radius = 7.0f;
		ap.height = 5.0f;
		ap.maxSpeed = 20.0f;
		ap.maxAcceleration = ap.maxSpeed;

		// エージェントセット
		agent->AddAgent(wyvern->transform.GetPosition(), ap);
		wyvern->SetAgent(agent, i);

		enemyManager.Register(wyvern);
#endif
	}
#pragma endregion

#pragma region 草食動物
	const int stegoCount = 8;
	for (int i = 0; i < stegoCount; ++i)
	{		
		Stego* stego = new Stego();

		// 円状に出現させる
		float theta        = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
		float range        = Mathf::RandomRange(0.0f + i, 10.0f);

		stego->transform.SetPositionX(stego->transform.GetPosition().x + sinf(theta) * range);
		stego->transform.SetPositionY(stego->transform.GetPosition().y);
		stego->transform.SetPositionZ(stego->transform.GetPosition().z + cosf(theta) * range);
		
		// エージェント登録
		AgentParams ap{};
		ap.height          = 5.0f;
		ap.radius          = 2.0f;
		ap.maxSpeed		   = 50.0f;
		ap.maxAcceleration = ap.maxSpeed;
		agent->AddAgent(stego->transform.GetPosition(), ap);

		// エージェントセット
		stego->SetAgent(agent, i + dragonCount);
		
		enemyManager.Register(stego);
	}
#pragma endregion

#pragma region カメラ
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
	cameraController->RegisterRaycastModels(stageMain->GetModel(0));
	cameraController->SetOwner(player.get());
#if DragonFlg
	cameraController->SetTarget(dragon);
#else
	cameraController->SetTarget(wyvern);
#endif
#pragma endregion

#pragma region テクスチャ
	const char* skymapTex[] =
	{
		"Data/Sprite/drakensberg_solitary_mountain_4k.DDS",
		"Data/Sprite/dikhololo_night_4k.DDS"
	};
	
	// ディザリング用テクスチャ
	Texture::LoadTexture("Data/Sprite/bayerTex.DDS", graphics.GetDevice(), ditheringTexture.GetAddressOf());

	// スカイマップテクスチャ
	skymap = std::make_unique<Sprite>(skymapTex[0]);
	Texture::LoadTexture(skymapTex[0], graphics.GetDevice(), skymapTexture.GetAddressOf());
	
#pragma endregion
		
	// オフスクリーン
	bitBlockTransfer = std::make_unique<FullscreeQuad>(graphics.GetDevice());

	// シャドウマップ
	dsvShadowmap = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);

	// ポストエフェクト
	PostEffect::Instance().Initialize();

	// ゲームマネージャー初期化
	GameManager::Instance().Initialize();
}

// 終了化
void SceneGame::Finalize()
{
	// ステージ終了時
	StageManager::Instance().Clear();

	// エネミー終了化
	EnemyManager::Instance().Clear();

	// 弾終了化
	ProjectileManager::Instance().Clear();

	UIManager::Instance().Clear();

	GameManager::Instance().Finalize();
}

// 更新処理
void SceneGame::Update(const float& elapsedTime)
{
	GameManager::Instance().Update(elapsedTime);

	// true で imgui 表示
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_START)
	{
		debugFlg = !debugFlg;
	}

	// 止める
	if (theWorld)
	{
		// プレイヤー更新処理
		player->Update(elapsedTime);

		// カメラコントローラー処理
		cameraController->noOwnerCamera = true;
		cameraController->Update(elapsedTime);
		return;
	}
	else
	{
		cameraController->noOwnerCamera = false;
	}

	// めたAI更新
	MetaAI::Instance().Update(elapsedTime);

	// ポストエフェクト更新
	PostEffect::Instance().Update(elapsedTime);

	// ターゲットカメラ
	if (gamePad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER)
	{
#if DragonFlg
		cameraController->SetTarget(dragon);
#else
		cameraController->SetTarget(wyvern);
#endif
	}

	// ロックオンカメラ
	if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_THUMB)
	{
		cameraController->SetLockOnCamera();
	}
 
	// カメラコントローラー処理
	cameraController->Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// 弾更新
	ProjectileManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// UI更新
	UIManager::Instance().Update(elapsedTime);

	agent->Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics      = Graphics::Instance();
	RenderState& rs			= RenderState::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

#if 1
	// オフスクリーンの警告をなくす
	ID3D11RenderTargetView* rtvsNull[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	dc->OMSetRenderTargets(_countof(rtvsNull), rtvsNull, 0);
	ID3D11ShaderResourceView* srvsNull[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	dc->VSSetShaderResources(0, _countof(srvsNull), srvsNull);
	dc->PSSetShaderResources(0, _countof(srvsNull), srvsNull);
#endif

	// サンプラーステート設定
	rs.SetPSSampler(dc, 0, 1, SAMPLER_MODE::POINT);
	rs.SetPSSampler(dc, 1, 1, SAMPLER_MODE::LINEAR);
	rs.SetPSSampler(dc, 2, 1, SAMPLER_MODE::ANISOTROPIC);
	rs.SetPSSampler(dc, 3, 1, SAMPLER_MODE::LINER_BORDER_BLACK);
	rs.SetPSSampler(dc, 4, 1, SAMPLER_MODE::LINER_BORDER_WHITE);
	rs.SetPSSampler(dc, 5, 1, SAMPLER_MODE::LINEAR_MIRROR);

	// シャドウマップの描画
	RenderShadowmap(dc);
	
	// 画面クリア＆レンダーターゲット設定
	graphics.Clear(0.0f, 0.0f, 0.5f);

	// 描画処理
	RenderContext rc{};
	rc.lightDirection = lightDir;    // ライト方向（下方向）
	rc.lightColor     = lightColor;  // ライト色

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
	D3D11_VIEWPORT	vp = {};
	vp.Width    = graphics.GetScreenWidth();
	vp.Height   = graphics.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);
	
	// 3D描画
#if UsePostEffect
	RenderOffscreen(dc, rc);
#else
	Render3D(dc, rc);
#endif

	// 2D描画
	Render2D(dc);
}

// シャドウマップ描画
void SceneGame::RenderShadowmap(ID3D11DeviceContext* dc)
{
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = dsvShadowmap->GetDSV().Get();

	// 画面クリア
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// レンダーターゲット設定
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width    = static_cast<float>(dsvShadowmap->GetWidth());
	vp.Height   = static_cast<float>(dsvShadowmap->GetHeight());
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
	Shader* shadow  = ss.GetShadowmapShader();

	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

	shadow->Begin(dc, rc);

	// ステージ
	StageManager::Instance().Render(dc, shadow);
	// プレイヤー
	player->Render(dc, shadow);
	// 敵
	EnemyManager::Instance().Render(dc, shadow);

	shadow->End(dc);
}

void SceneGame::Render3D(ID3D11DeviceContext* dc, const RenderContext& rc)
{	
	Graphics& graphics = Graphics::Instance();

	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

	rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_BACK);

#pragma region スカイマップ描画
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_OFF);
	rs.SetBlend(dc, BLEND_MODE::NONE);
	{
		// 20番にスカイマップ用のテクスチャを設定
		graphics.GetDeviceContext()->PSSetShaderResources(20, 1, skymapTexture.GetAddressOf());

		SkymapShader* shader = ss.GetSkymapShader();

		shader->Begin(dc, rc);
		shader->Draw(dc, skymap.get());
		shader->End(dc);
	}
#pragma endregion

#pragma region 3Dモデル描画
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_NONE);
	{
		// エネミー描画
		Shader* shader = ss.GetModelShader(MODEL::EnemyPBR);

		shader->Begin(dc, rc);
		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);

		shader = ss.GetModelShader(MODEL::DefaultPBR);

		// ディザリング用のテクスチャ更新
		graphics.GetDeviceContext()->PSSetShaderResources(21, 1, ditheringTexture.GetAddressOf());

		// シェーダにカメラの情報を渡す
		shader->Begin(dc, rc);

		// プレイヤー描画
		player->Render(dc, shader);

		// 弾描画
		ProjectileManager::Instance().Render(dc, shader);

		// ステージ描画
		StageManager::Instance().Render(dc, shader);
		
		shader->End(dc);
	}
#pragma endregion

#pragma region エフェクト描画
	{
		rs.SetBlend(dc, BLEND_MODE::ALPHA);
		rs.SetDepth(dc, DEPTH_MODE::ZT_OFF_ZW_OFF);
		rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_BACK);
		
		EffectManager::Instance().Render(rc.view, rc.projection);

		// パーティクル
		rs.SetBlend(dc, BLEND_MODE::ADD);
		rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
#if DragonFlg
		dragon->breath->Render();
		dragon->eruption->Render();
#else
#endif
		// 剣のトレイル
		rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_NONE);
		player->sword->TrailRender();
	}
#pragma endregion
}

void SceneGame::Render2D(ID3D11DeviceContext* dc)
{
	ShaderState& ss = ShaderState::Instance();
	//RenderState& rs = RenderState::Instance();

	if (showUI)
	{
		// UI描画
		SpriteShader* shader = ss.GetSpriteShader(SPRITE::Default);		
		shader->Begin(dc);
		UIManager::Instance().Render(dc, shader);
		shader->End(dc);
	}
	if (debugFlg)
	{
		// 2DデバッグGUI描画
		DrawDebugGUI();
	}

}

void SceneGame::RenderOffscreen(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

	rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_BACK);

#if 1
	// オフスクリーン
	FrameBuffer* frameBuffer = ss.GetOffscreenBuffer(OFFSCREEN::OFFSCREEN_SET);

	// ゲームシーンをオフスクリーンにする。
	frameBuffer->Clear(dc);

	frameBuffer->Activate(dc);

	// 3D描画
	Render3D(dc, rc);

	// 3DデバッグGUI描画
	DrawDebugRenderer(dc, rc);

	frameBuffer->DeActivate(dc);

	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetDepth(dc, DEPTH_MODE::ZT_OFF_ZW_OFF);

#pragma region 川瀬式ブラー
	ID3D11PixelShader* luminance = ss.GetOffscreenShader(OFFSCREEN::LUMINANCE);

#if 1
	// 川瀬式ブラーやってみる
	for (int i = 0; i < 4; ++i)
	{
		ss.GetBlurBuffer(i)->Clear(dc);
		ss.GetBlurBuffer(i)->Activate(dc);
		bitBlockTransfer->Blit(dc, frameBuffer->shaderResourceViews[0].GetAddressOf(), 6 + i, 1, luminance);
		ss.GetBlurBuffer(i)->DeActivate(dc);
	}
#else
	FrameBuffer* luminanceBuffer = ss.GetOffscreenBuffer(OFFSCREEN::LUMINANCE);
	luminanceBuffer->Activate(dc);


#endif
#pragma endregion

#pragma region 陽炎
	// 定数バッファ更新
	PostEffect::Instance().UpdateHeatHaze(dc);

	// オフスクリーンのシェーダーリソースビューをGPU側に送る
	ID3D11ShaderResourceView* srvs[2]
	{
		// ゲームシーン
		frameBuffer->shaderResourceViews[0].Get(),
		// 深度
		frameBuffer->shaderResourceViews[1].Get()
	};

	FrameBuffer* heatHazeBuffer = ss.GetOffscreenBuffer(OFFSCREEN::HEATHAZE);
	ID3D11PixelShader* heatHaze = ss.GetOffscreenShader(OFFSCREEN::HEATHAZE);

	// オフスクリーンにした画面に輝度抽出を行う
	heatHazeBuffer->Clear(dc);
	heatHazeBuffer->Activate(dc);
	bitBlockTransfer->Blit(dc, srvs, 6, _countof(srvs), heatHaze);
	heatHazeBuffer->DeActivate(dc);
#pragma endregion

#pragma region ラジアルブラー
	PostEffect::Instance().UpdateShockBlur(dc);

	FrameBuffer* shockBlurBuffer = ss.GetOffscreenBuffer(OFFSCREEN::SHOCKBLUR);
	ID3D11PixelShader* shockBlur = ss.GetOffscreenShader(OFFSCREEN::SHOCKBLUR);

	// オフスクリーンにした画面にラジアルブラーを行う
	shockBlurBuffer->Clear(dc);
	shockBlurBuffer->Activate(dc);
	bitBlockTransfer->Blit(dc, frameBuffer->shaderResourceViews[0].GetAddressOf(), 6, 1, shockBlur);
	shockBlurBuffer->DeActivate(dc);
#pragma endregion
	PostEffect::Instance().UpdateBloom(dc);

	// オフスクリーンのシェーダーリソースビューをGPU側に送る
	ID3D11ShaderResourceView* finalSrvs[7]
	{
		// ゲームシーン
		frameBuffer->shaderResourceViews[0].Get(),
		// 陽炎
		heatHazeBuffer->shaderResourceViews[0].Get(),
		// ラジアルブラー
		shockBlurBuffer->shaderResourceViews[0].Get(),
		// 川瀬式ブラー
		ss.GetBlurBuffer(0)->shaderResourceViews[0].Get(),
		ss.GetBlurBuffer(1)->shaderResourceViews[0].Get(),
		ss.GetBlurBuffer(2)->shaderResourceViews[0].Get(),
		ss.GetBlurBuffer(3)->shaderResourceViews[0].Get()
	};
	ID3D11PixelShader* Final = ss.GetOffscreenShader(OFFSCREEN::FINAL);
	bitBlockTransfer->Blit(dc, finalSrvs, 6, _countof(finalSrvs), Final);

#pragma endregion
#else
	// 3D描画
	Render3D(dc, rc);

	// 3DデバッグGUI描画
	DrawDebugRenderer(dc, rc);
#endif
}

void SceneGame::DrawDebugRenderer(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

	if (showPrimitive)
	{
		// 3Dデバッグ描画
		rs.SetBlend(dc, BLEND_MODE::ALPHA);
		rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

		player->DrawDebugPrimitive();

		EnemyManager::Instance().DrawDebugPrimitive();

		ProjectileManager::Instance().DrawDebugPrimitive();

		StageManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		ss.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		ss.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// ナビメッシュレンダラ描画
	if(showNavmesh)
	{
		// ナビメッシュレンダラ描画準備
		ss.GetNavMeshRenderer()->Draw(navmesh->GetNavMesh(), navmesh->GetNavMeshQuery());		
		ss.GetNavMeshRenderer()->Draw(agent->GetCrowd());
		
		//graphics.GetNavMeshRenderer()->Draw(navmesh->GetPolyMeshQuery());
		ss.GetNavMeshRenderer()->Render(dc, rc.view, rc.projection);
	}
}

void SceneGame::DrawDebugGUI()
{
	ShaderState& ss = ShaderState::Instance();

	DrawDebugButton();

	EnemyManager::Instance().DrawDebugGUI();
	GameManager::Instance().DrawGUI();
	navmesh->DrawGUI(/*navModel.get()*/);

	ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Game", nullptr, ImGuiWindowFlags_None))
	{
		// トランスフォーム
		//ステージのGUI
		auto& stageManager = StageManager::Instance();
		stageManager.DrawDebugGUI();
		
		cameraController->DrawDebugGUI();
		player->DrawDebugGUI();

		// ポストエフェクト
		PostEffect::Instance().DrawDebubGUI();

		if (ImGui::CollapsingHeader("FOG"))
		{
			ImGui::SliderFloat3("lightDir",   &lightDir.x, -10.0f, 10.0f);
			ImGui::SliderFloat4("LightColor", &lightColor.x, 0.0f, 10.0f);
		}

		// シャドウマップ
		if (ImGui::TreeNode("Shadowmap"))
		{
			ImGui::SliderFloat("DrawRect", &shadowRect, 1.0f, 2048.0f);
			ImGui::DragFloat("ShadowBias", &shadowBias, 0.0000001f, 0.0f, 1.0f, "%.10f");
			ImGui::DragFloat3("ShadowColor", &shadowColor.x, 0.0f, 1.0f);
			ImGui::SliderFloat("LightBias", &lightBias, -2048.0f, 2048.0f);
			ImGui::Text("texture");
			ImGui::Image(dsvShadowmap->GetSRV().Get(), { 256,256 }, { 0,0 }, { 1,1 }, { 1,1,1,1 });
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Blur"))
		{
			ImGui::Text("Blur : 1/2");
			ImGui::Image(ss.GetBlurBuffer(0)->shaderResourceViews[0].Get(), { 256,256 }, { 0,0 }, { 1,1 }, { 1,1,1,1 });

			ImGui::Text("Blur : 1/4");
			ImGui::Image(ss.GetBlurBuffer(1)->shaderResourceViews[0].Get(), { 256,256 }, { 0,0 }, { 1,1 }, { 1,1,1,1 });

			ImGui::Text("Blur : 1/8");
			ImGui::Image(ss.GetBlurBuffer(2)->shaderResourceViews[0].Get(), { 256,256 }, { 0,0 }, { 1,1 }, { 1,1,1,1 });

			ImGui::Text("Blur : 1/16");
			ImGui::Image(ss.GetBlurBuffer(3)->shaderResourceViews[0].Get(), { 256,256 }, { 0,0 }, { 1,1 }, { 1,1,1,1 });
							
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void SceneGame::DrawDebugButton()
{
	ImGui::SetNextWindowPos(ImVec2(10,   300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("DebugButton", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::Checkbox(u8"Primitive"_u, &showPrimitive);
		ImGui::Checkbox(u8"UI"_u,        &showUI);
		ImGui::Checkbox(u8"NavMesh"_u,   &showNavmesh);
		ImGui::Checkbox(u8"TheWorld"_u,  &theWorld);
	}
	ImGui::End();
}
#endif