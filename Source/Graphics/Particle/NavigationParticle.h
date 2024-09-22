#pragma once
#include "Particle.h"

#define NUMTHREADS_X 16

// �p�[�e�B�N���̃X�e�[�^�X
struct ParticleStatus
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 moveSpeed;
	DirectX::XMFLOAT4 color;
	int state = 0;
};

// �i�r�Q�[�V�����p�[�e�B�N��
class NavigationParticle final : public Particle 
{
public:
	// �萔�o�b�t�@
	struct CBScene
	{
		DirectX::XMFLOAT4X4 viewProjection;
	};

	// �p�[�e�B�N���̒萔�o�b�t�@
	struct CBParticle
	{	
		DirectX::XMFLOAT3 emitterPosition;
		float			  particleSize;
		DirectX::XMFLOAT3 direction;
		float			  time;
		DirectX::XMFLOAT3 targetPosition;
		float			  deltaTime;
	};
	CBParticle particleData;	

	NavigationParticle(size_t particleCount);
	~NavigationParticle() = default;

	NavigationParticle(const NavigationParticle&) = delete;
	NavigationParticle(NavigationParticle&&)      = delete;
	NavigationParticle& operator=(const NavigationParticle&) = delete;
	NavigationParticle& operator=(NavigationParticle&&)		 = delete;

	void Initialize(const float& elapsedTime,const DirectX::XMFLOAT3& position);
	void Update(const float& elapsedTime);
	void Render(const RenderContext& rc);

	void SetTargetPosition(const DirectX::XMFLOAT3& pos) { this->targetPos = pos; }
	void SetDirection(const DirectX::XMFLOAT3& direction) { this->direction = direction; }
private:
	size_t maxParticleCount;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;

	// ID3D11UnorderedAccessView : �����_�����O���Ƀp�C�v���C���ɃA�N�Z�X�ł��郊�\�[�X�̕������w�肵�܂��B
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uavParticle;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  srvParticle;

	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vsParticle;
	// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  psParticle;
	// �W�I���g���V�F�[�_�[ : �s�N�Z���V�F�[�_�[�ɓn�����I�u�W�F�N�g�̒��_�̉��H���ł���
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> gsParticle;
	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	// �R���s���[�g�V�F�[�_�[�i�X�V�j
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> csInitializeParticle;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> csParticle;
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;

private:
	DirectX::XMFLOAT3 position  = {};
	DirectX::XMFLOAT3 targetPos = {};
	DirectX::XMFLOAT3 direction = {};
};
