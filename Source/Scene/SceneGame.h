#pragma once

// �t�@�C�����C���N���[�h
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
// �Q�[���V�[��
class SceneGame final : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override = default;

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(const float& elapsedTime) override;

	// �`�揈��
	void Render() override;
	void RenderShadowmap(ID3D11DeviceContext* dc);
	void Render3D(ID3D11DeviceContext* dc, const RenderContext& rc);
	void Render2D(ID3D11DeviceContext* dc);
	void RenderOffscreen(ID3D11DeviceContext* dc, const RenderContext& rc);

	// �f�o�b�O�`��
	void DrawDebugRenderer(ID3D11DeviceContext* dc, const RenderContext& rc);
	void DrawDebugGUI();
	void DrawDebugButton();

private:
	std::unique_ptr<Player> player = nullptr;  // �v���C���[
	Dragon* dragon                 = nullptr;  // �h���S��
	Wyvern*	wyvern                 = nullptr;  // ���C�o�[��

	std::unique_ptr<CameraController> cameraController = nullptr;  // �J�����R���g���[��

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ditheringTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymapTexture	  = nullptr;
	std::unique_ptr<Sprite> skymap = nullptr;

	// �I�t�X�N���[��
	std::unique_ptr<FullscreeQuad> bitBlockTransfer;

	// �i�r���b�V��
	std::unique_ptr<NavMesh> navmesh;
	std::shared_ptr<NavMeshAgent> agent;

	// �V���h�E�}�b�v�p���
	static const UINT SHADOWMAP_SIZE = 16382;  // �V���h�E�}�b�v�̃T�C�Y
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

	// ���E���~�߂�i�J�����̂ݓ�������j
	bool theWorld = false;
	
public:
	std::unique_ptr<AudioSource> gameBGM = nullptr;
};
#endif