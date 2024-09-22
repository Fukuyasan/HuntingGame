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

	// ���^AI
	MetaAI::Instance().Initialize();

	// �X�e�[�W
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/Stage/model/Terrain.mdl");
		gameObject->SetName("Stage");
		gameObject->transform.SetPosition({ 0.0f, 0.0f, 0.0f });
		gameObject->transform.SetScale(0.02f);
		gameObject->SetLayer(Layer::Stage);
		gameObject->AddComponent<StageMain>();
	}
	// �v���C���[
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
	// ��
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/Sword/Sword.mdl");
		gameObject->SetName("Sword");
		gameObject->SetLayer(Layer::Weapon);
		gameObject->AddComponent<Sword>();
	}
	// �I�g��
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
	// �n���}�[
	{
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel("Data/Model/Hammer/Hammer.mdl");
		gameObject->SetName("Hammer");
		gameObject->SetLayer(Layer::Weapon);
		gameObject->AddComponent<Hammer>();
	}
	// �J����
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

		// �J�����R���g���[��������
		cameraController = std::make_unique<CameraController>();
		cameraController->RegisterRaycastModels(*GameObjectManager::Instance().Find("Player")->GetModel());
		cameraController->SetOwner(GameObjectManager::Instance().Find("Player"));
		cameraController->SetTarget(GameObjectManager::Instance().Find("Dragon"));
	}

	{
		// �X�J�C�}�b�v
		const char* skymapPath = "Data/Sprite/drakensberg_solitary_mountain_4k.DDS";
		skymap = std::make_unique<Sprite>(skymapPath);

		// �V���h�E�}�b�v
		dsvShadowmap = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	}
}

void SceneTest::Finalize()
{

}

void SceneTest::Update(const float& elapsedTime)
{
	// �߂�AI�X�V
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

	// �T���v���[�X�e�[�g�ݒ�
	rs.SetPSSampler(dc, 0, SAMPLER_MODE::POINT);
	rs.SetPSSampler(dc, 1, SAMPLER_MODE::LINEAR);
	rs.SetPSSampler(dc, 2, SAMPLER_MODE::ANISOTROPIC);
	rs.SetPSSampler(dc, 3, SAMPLER_MODE::LINER_BORDER_BLACK);
	rs.SetPSSampler(dc, 4, SAMPLER_MODE::LINER_BORDER_WHITE);
	rs.SetPSSampler(dc, 5, SAMPLER_MODE::LINEAR_MIRROR);

	// �V���h�E�}�b�v�̕`��
	RenderShadowmap(dc);

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	graphics.Clear(0.0f, 0.0f, 0.5f);

	// �`�揈��
	RenderContext rc{};
	rc.lightDirection = lightDir;    // ���C�g�����i�������j
	rc.lightColor	  = lightColor;  // ���C�g�F

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

	// ��ʃN���A
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// �����_�[�^�[�Q�b�g�ݒ�
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(dsvShadowmap->GetWidth());
	vp.Height = static_cast<float>(dsvShadowmap->GetHeight());
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
	Shader* shadow = ss.GetShadowmapShader();

	rs.SetBlend(dc, BLEND_MODE::ALPHA);
	rs.SetDepth(dc, DEPTH_MODE::ZT_ON_ZW_ON);

	// �e�̕`��
	GameObjectManager::Instance().Render(rc, shadow);
}

void SceneTest::Render3D(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance();

	ShaderState& ss = ShaderState::Instance();
	RenderState& rs = RenderState::Instance();

#pragma region �X�J�C�}�b�v�`��
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
