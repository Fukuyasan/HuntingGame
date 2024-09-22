#pragma once
#include "Particle.h"

#define NUMTHREADS_X 16

// パーティクルのステータス
struct ParticleStatus
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 moveSpeed;
	DirectX::XMFLOAT4 color;
	int state = 0;
};

// ナビゲーションパーティクル
class NavigationParticle final : public Particle 
{
public:
	// 定数バッファ
	struct CBScene
	{
		DirectX::XMFLOAT4X4 viewProjection;
	};

	// パーティクルの定数バッファ
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

	// ID3D11UnorderedAccessView : レンダリング中にパイプラインにアクセスできるリソースの部分を指定します。
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uavParticle;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  srvParticle;

	// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vsParticle;
	// ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  psParticle;
	// ジオメトリシェーダー : ピクセルシェーダーに渡されるオブジェクトの頂点の加工ができる
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> gsParticle;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	// コンピュートシェーダー（更新）
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> csInitializeParticle;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> csParticle;
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;

private:
	DirectX::XMFLOAT3 position  = {};
	DirectX::XMFLOAT3 targetPos = {};
	DirectX::XMFLOAT3 direction = {};
};
