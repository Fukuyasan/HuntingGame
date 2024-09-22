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
// ������
void SceneGame::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	StageManager& stageManager = StageManager::Instance();
	EnemyManager& enemyManager = EnemyManager::Instance();

#pragma region �X�e�[�W
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
#pragma endregion
	
#pragma region MetaAI
	MetaAI::Instance().Initialize();
#pragma endregion

#pragma region �i�r���b�V��
	// �i�r���b�V��������
	navmesh = std::make_unique<SoloMesh>("navMeshData.bin");

	// �G�[�W�F���g������
	agent = std::make_unique<NavMeshAgent>(navmesh.get());
#pragma endregion

#pragma region �v���C���[
	player = std::make_unique<Player>();
#pragma endregion

#pragma region �{�X�����X�^�[
	const int dragonCount = 1;
	for (int i = 0; i < dragonCount; ++i)
	{
#if DragonFlg
		dragon = new Dragon();

		// �G�[�W�F���g�o�^
		AgentParams ap{};
		ap.radius          = 7.0f;
		ap.height          = 5.0f;
		ap.maxSpeed        = 20.0f;
		ap.maxAcceleration = ap.maxSpeed;

		// �G�[�W�F���g�Z�b�g
		agent->AddAgent(dragon->transform.GetPosition(), ap);
		dragon->SetAgent(agent, i);

		enemyManager.Register(dragon);

#else
		wyvern = new Wyvern();

		// �G�[�W�F���g�o�^
		AgentParams ap{};
		ap.radius = 7.0f;
		ap.height = 5.0f;
		ap.maxSpeed = 20.0f;
		ap.maxAcceleration = ap.maxSpeed;

		// �G�[�W�F���g�Z�b�g
		agent->AddAgent(wyvern->transform.GetPosition(), ap);
		wyvern->SetAgent(agent, i);

		enemyManager.Register(wyvern);
#endif
	}
#pragma endregion

#pragma region ���H����
	const int stegoCount = 8;
	for (int i = 0; i < stegoCount; ++i)
	{		
		Stego* stego = new Stego();

		// �~��ɏo��������
		float theta        = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
		float range        = Mathf::RandomRange(0.0f + i, 10.0f);

		stego->transform.SetPositionX(stego->transform.GetPosition().x + sinf(theta) * range);
		stego->transform.SetPositionY(stego->transform.GetPosition().y);
		stego->transform.SetPositionZ(stego->transform.GetPosition().z + cosf(theta) * range);
		
		// �G�[�W�F���g�o�^
		AgentParams ap{};
		ap.height          = 5.0f;
		ap.radius          = 2.0f;
		ap.maxSpeed		   = 50.0f;
		ap.maxAcceleration = ap.maxSpeed;
		agent->AddAgent(stego->transform.GetPosition(), ap);

		// �G�[�W�F���g�Z�b�g
		stego->SetAgent(agent, i + dragonCount);
		
		enemyManager.Register(stego);
	}
#pragma endregion

#pragma region �J����
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
	
	// �J�����R���g���[��������
	cameraController = std::make_unique<CameraController>();
	cameraController->RegisterRaycastModels(stageMain->GetModel(0));
	cameraController->SetOwner(player.get());
#if DragonFlg
	cameraController->SetTarget(dragon);
#else
	cameraController->SetTarget(wyvern);
#endif
#pragma endregion

#pragma region �e�N�X�`��
	const char* skymapTex[] =
	{
		"Data/Sprite/drakensberg_solitary_mountain_4k.DDS",
		"Data/Sprite/dikhololo_night_4k.DDS"
	};
	
	// �f�B�U�����O�p�e�N�X�`��
	Texture::LoadTexture("Data/Sprite/bayerTex.DDS", graphics.GetDevice(), ditheringTexture.GetAddressOf());

	// �X�J�C�}�b�v�e�N�X�`��
	skymap = std::make_unique<Sprite>(skymapTex[0]);
	Texture::LoadTexture(skymapTex[0], graphics.GetDevice(), skymapTexture.GetAddressOf());
	
#pragma endregion
		
	// �I�t�X�N���[��
	bitBlockTransfer = std::make_unique<FullscreeQuad>(graphics.GetDevice());

	// �V���h�E�}�b�v
	dsvShadowmap = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);

	// �|�X�g�G�t�F�N�g
	PostEffect::Instance().Initialize();

	// �Q�[���}�l�[�W���[������
	GameManager::Instance().Initialize();
}

// �I����
void SceneGame::Finalize()
{
	// �X�e�[�W�I����
	StageManager::Instance().Clear();

	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();

	// �e�I����
	ProjectileManager::Instance().Clear();

	UIManager::Instance().Clear();

	GameManager::Instance().Finalize();
}

// �X�V����
void SceneGame::Update(const float& elapsedTime)
{
	GameManager::Instance().Update(elapsedTime);

	// true �� imgui �\��
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_START)
	{
		debugFlg = !debugFlg;
	}

	// �~�߂�
	if (theWorld)
	{
		// �v���C���[�X�V����
		player->Update(elapsedTime);

		// �J�����R���g���[���[����
		cameraController->noOwnerCamera = true;
		cameraController->Update(elapsedTime);
		return;
	}
	else
	{
		cameraController->noOwnerCamera = false;
	}

	// �߂�AI�X�V
	MetaAI::Instance().Update(elapsedTime);

	// �|�X�g�G�t�F�N�g�X�V
	PostEffect::Instance().Update(elapsedTime);

	// �^�[�Q�b�g�J����
	if (gamePad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER)
	{
#if DragonFlg
		cameraController->SetTarget(dragon);
#else
		cameraController->SetTarget(wyvern);
#endif
	}

	// ���b�N�I���J����
	if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_THUMB)
	{
		cameraController->SetLockOnCamera();
	}
 
	// �J�����R���g���[���[����
	cameraController->Update(elapsedTime);

	// �v���C���[�X�V����
	player->Update(elapsedTime);

	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �e�X�V
	ProjectileManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);

	// UI�X�V
	UIManager::Instance().Update(elapsedTime);

	agent->Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics      = Graphics::Instance();
	RenderState& rs			= RenderState::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

#if 1
	// �I�t�X�N���[���̌x�����Ȃ���
	ID3D11RenderTargetView* rtvsNull[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	dc->OMSetRenderTargets(_countof(rtvsNull), rtvsNull, 0);
	ID3D11ShaderResourceView* srvsNull[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	dc->VSSetShaderResources(0, _countof(srvsNull), srvsNull);
	dc->PSSetShaderResources(0, _countof(srvsNull), srvsNull);
#endif

	// �T���v���[�X�e�[�g�ݒ�
	rs.SetPSSampler(dc, 0, 1, SAMPLER_MODE::POINT);
	rs.SetPSSampler(dc, 1, 1, SAMPLER_MODE::LINEAR);
	rs.SetPSSampler(dc, 2, 1, SAMPLER_MODE::ANISOTROPIC);
	rs.SetPSSampler(dc, 3, 1, SAMPLER_MODE::LINER_BORDER_BLACK);
	rs.SetPSSampler(dc, 4, 1, SAMPLER_MODE::LINER_BORDER_WHITE);
	rs.SetPSSampler(dc, 5, 1, SAMPLER_MODE::LINEAR_MIRROR);

	// �V���h�E�}�b�v�̕`��
	RenderShadowmap(dc);
	
	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	graphics.Clear(0.0f, 0.0f, 0.5f);

	// �`�揈��
	RenderContext rc{};
	rc.lightDirection = lightDir;    // ���C�g�����i�������j
	rc.lightColor     = lightColor;  // ���C�g�F

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();

	// �V���h�E�}�b�v�̐ݒ�
	rc.shadowmapData.shadowmap   = dsvShadowmap->GetSRV().Get();
	rc.shadowmapData.lightVP     = lightVP;
	rc.shadowmapData.shadowColor = shadowColor;
	rc.shadowmapData.shadowBias  = shadowBias;

	// �J�����ʒu
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;

	// �r���[�s��
	rc.view = camera.GetView();
	// �v���W�F�N�V�����s��
	rc.projection = camera.GetProjection();	

	// �Q�[���V�[���̃r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT	vp = {};
	vp.Width    = graphics.GetScreenWidth();
	vp.Height   = graphics.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);
	
	// 3D�`��
#if UsePostEffect
	RenderOffscreen(dc, rc);
#else
	Render3D(dc, rc);
#endif

	// 2D�`��
	Render2D(dc);
}

// �V���h�E�}�b�v�`��
void SceneGame::RenderShadowmap(ID3D11DeviceContext* dc)
{
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = dsvShadowmap->GetDSV().Get();

	// ��ʃN���A
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// �����_�[�^�[�Q�b�g�ݒ�
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT	vp = {};
	vp.Width    = static_cast<float>(dsvShadowmap->GetWidth());
	vp.Height   = static_cast<float>(dsvShadowmap->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	RenderContext rc{};

	DirectX::XMFLOAT3 LightDirection = { lightDir.x,lightDir.y,lightDir.z };

	//�J�����p�����[�^�ݒ�	
	// ���s��������J�����ʒu���쐬���A�������猴�_�̈ʒu������悤�Ɏ����s����쐬
	DirectX::XMVECTOR lightPosition = DirectX::XMLoadFloat3(&LightDirection);
	lightPosition = DirectX::XMVectorScale(lightPosition, lightBias);

	// �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
		lightPosition,
		DirectX::XMVectorSet(0, 0, 0, 0),
		DirectX::XMVectorSet(0, 1, 0, 0)
	);
	DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowRect, shadowRect, 0.1f, 1000.0f);

	DirectX::XMStoreFloat4x4(&rc.view, V);
	DirectX::XMStoreFloat4x4(&rc.projection, P);

	DirectX::XMStoreFloat4x4(&lightVP, V * P);
	
	// �V���h�E�}�b�v�`��
	RenderState& rs = RenderState::Instance();
	ShaderState& ss = ShaderState::Instance();
	Shader* shadow  = ss.GetShadowmapShader();

	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

	shadow->Begin(dc, rc);

	// �X�e�[�W
	StageManager::Instance().Render(dc, shadow);
	// �v���C���[
	player->Render(dc, shadow);
	// �G
	EnemyManager::Instance().Render(dc, shadow);

	shadow->End(dc);
}

void SceneGame::Render3D(ID3D11DeviceContext* dc, const RenderContext& rc)
{	
	Graphics& graphics = Graphics::Instance();

	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

	rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_BACK);

#pragma region �X�J�C�}�b�v�`��
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_OFF);
	rs.SetBlend(dc, BLEND_MODE::NONE);
	{
		// 20�ԂɃX�J�C�}�b�v�p�̃e�N�X�`����ݒ�
		graphics.GetDeviceContext()->PSSetShaderResources(20, 1, skymapTexture.GetAddressOf());

		SkymapShader* shader = ss.GetSkymapShader();

		shader->Begin(dc, rc);
		shader->Draw(dc, skymap.get());
		shader->End(dc);
	}
#pragma endregion

#pragma region 3D���f���`��
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_NONE);
	{
		// �G�l�~�[�`��
		Shader* shader = ss.GetModelShader(MODEL::EnemyPBR);

		shader->Begin(dc, rc);
		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);

		shader = ss.GetModelShader(MODEL::DefaultPBR);

		// �f�B�U�����O�p�̃e�N�X�`���X�V
		graphics.GetDeviceContext()->PSSetShaderResources(21, 1, ditheringTexture.GetAddressOf());

		// �V�F�[�_�ɃJ�����̏���n��
		shader->Begin(dc, rc);

		// �v���C���[�`��
		player->Render(dc, shader);

		// �e�`��
		ProjectileManager::Instance().Render(dc, shader);

		// �X�e�[�W�`��
		StageManager::Instance().Render(dc, shader);
		
		shader->End(dc);
	}
#pragma endregion

#pragma region �G�t�F�N�g�`��
	{
		rs.SetBlend(dc, BLEND_MODE::ALPHA);
		rs.SetDepth(dc, DEPTH_MODE::ZT_OFF_ZW_OFF);
		rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_BACK);
		
		EffectManager::Instance().Render(rc.view, rc.projection);

		// �p�[�e�B�N��
		rs.SetBlend(dc, BLEND_MODE::ADD);
		rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);
#if DragonFlg
		dragon->breath->Render();
		dragon->eruption->Render();
#else
#endif
		// ���̃g���C��
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
		// UI�`��
		SpriteShader* shader = ss.GetSpriteShader(SPRITE::Default);		
		shader->Begin(dc);
		UIManager::Instance().Render(dc, shader);
		shader->End(dc);
	}
	if (debugFlg)
	{
		// 2D�f�o�b�OGUI�`��
		DrawDebugGUI();
	}

}

void SceneGame::RenderOffscreen(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

	rs.SetRasterizer(dc, RASTERIZER_MODE::SOLID_BACK);

#if 1
	// �I�t�X�N���[��
	FrameBuffer* frameBuffer = ss.GetOffscreenBuffer(OFFSCREEN::OFFSCREEN_SET);

	// �Q�[���V�[�����I�t�X�N���[���ɂ���B
	frameBuffer->Clear(dc);

	frameBuffer->Activate(dc);

	// 3D�`��
	Render3D(dc, rc);

	// 3D�f�o�b�OGUI�`��
	DrawDebugRenderer(dc, rc);

	frameBuffer->DeActivate(dc);

	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetDepth(dc, DEPTH_MODE::ZT_OFF_ZW_OFF);

#pragma region �쐣���u���[
	ID3D11PixelShader* luminance = ss.GetOffscreenShader(OFFSCREEN::LUMINANCE);

#if 1
	// �쐣���u���[����Ă݂�
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

#pragma region �z��
	// �萔�o�b�t�@�X�V
	PostEffect::Instance().UpdateHeatHaze(dc);

	// �I�t�X�N���[���̃V�F�[�_�[���\�[�X�r���[��GPU���ɑ���
	ID3D11ShaderResourceView* srvs[2]
	{
		// �Q�[���V�[��
		frameBuffer->shaderResourceViews[0].Get(),
		// �[�x
		frameBuffer->shaderResourceViews[1].Get()
	};

	FrameBuffer* heatHazeBuffer = ss.GetOffscreenBuffer(OFFSCREEN::HEATHAZE);
	ID3D11PixelShader* heatHaze = ss.GetOffscreenShader(OFFSCREEN::HEATHAZE);

	// �I�t�X�N���[���ɂ�����ʂɋP�x���o���s��
	heatHazeBuffer->Clear(dc);
	heatHazeBuffer->Activate(dc);
	bitBlockTransfer->Blit(dc, srvs, 6, _countof(srvs), heatHaze);
	heatHazeBuffer->DeActivate(dc);
#pragma endregion

#pragma region ���W�A���u���[
	PostEffect::Instance().UpdateShockBlur(dc);

	FrameBuffer* shockBlurBuffer = ss.GetOffscreenBuffer(OFFSCREEN::SHOCKBLUR);
	ID3D11PixelShader* shockBlur = ss.GetOffscreenShader(OFFSCREEN::SHOCKBLUR);

	// �I�t�X�N���[���ɂ�����ʂɃ��W�A���u���[���s��
	shockBlurBuffer->Clear(dc);
	shockBlurBuffer->Activate(dc);
	bitBlockTransfer->Blit(dc, frameBuffer->shaderResourceViews[0].GetAddressOf(), 6, 1, shockBlur);
	shockBlurBuffer->DeActivate(dc);
#pragma endregion
	PostEffect::Instance().UpdateBloom(dc);

	// �I�t�X�N���[���̃V�F�[�_�[���\�[�X�r���[��GPU���ɑ���
	ID3D11ShaderResourceView* finalSrvs[7]
	{
		// �Q�[���V�[��
		frameBuffer->shaderResourceViews[0].Get(),
		// �z��
		heatHazeBuffer->shaderResourceViews[0].Get(),
		// ���W�A���u���[
		shockBlurBuffer->shaderResourceViews[0].Get(),
		// �쐣���u���[
		ss.GetBlurBuffer(0)->shaderResourceViews[0].Get(),
		ss.GetBlurBuffer(1)->shaderResourceViews[0].Get(),
		ss.GetBlurBuffer(2)->shaderResourceViews[0].Get(),
		ss.GetBlurBuffer(3)->shaderResourceViews[0].Get()
	};
	ID3D11PixelShader* Final = ss.GetOffscreenShader(OFFSCREEN::FINAL);
	bitBlockTransfer->Blit(dc, finalSrvs, 6, _countof(finalSrvs), Final);

#pragma endregion
#else
	// 3D�`��
	Render3D(dc, rc);

	// 3D�f�o�b�OGUI�`��
	DrawDebugRenderer(dc, rc);
#endif
}

void SceneGame::DrawDebugRenderer(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

	if (showPrimitive)
	{
		// 3D�f�o�b�O�`��
		rs.SetBlend(dc, BLEND_MODE::ALPHA);
		rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

		player->DrawDebugPrimitive();

		EnemyManager::Instance().DrawDebugPrimitive();

		ProjectileManager::Instance().DrawDebugPrimitive();

		StageManager::Instance().DrawDebugPrimitive();

		// ���C�������_���`����s
		ss.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		ss.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// �i�r���b�V�������_���`��
	if(showNavmesh)
	{
		// �i�r���b�V�������_���`�揀��
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
		// �g�����X�t�H�[��
		//�X�e�[�W��GUI
		auto& stageManager = StageManager::Instance();
		stageManager.DrawDebugGUI();
		
		cameraController->DrawDebugGUI();
		player->DrawDebugGUI();

		// �|�X�g�G�t�F�N�g
		PostEffect::Instance().DrawDebubGUI();

		if (ImGui::CollapsingHeader("FOG"))
		{
			ImGui::SliderFloat3("lightDir",   &lightDir.x, -10.0f, 10.0f);
			ImGui::SliderFloat4("LightColor", &lightColor.x, 0.0f, 10.0f);
		}

		// �V���h�E�}�b�v
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