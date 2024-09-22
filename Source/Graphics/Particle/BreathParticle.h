#pragma once
#include "Particle.h"

#define NUMTHREADS_X 16

// パーティクルのステータス
struct ParticleStatus 
{
	DirectX::XMFLOAT3 position{0,0,0};
	DirectX::XMFLOAT4 color{1,1,1,1};
	DirectX::XMFLOAT3 velocity{0,0,0};
	float age = 0;
	int state = 0;
};

// ブレスパーティクル
class BreathParticle final : public Particle
{
public:
	// 定数バッファ
	struct CBScene
	{
		DirectX::XMFLOAT4X4 viewProjection;
	};

public:
	// パーティクルの定数バッファ
	struct CBBreathParticle
	{
		DirectX::XMFLOAT3 position;
		float particleSize;
		float time;
		float deltaTime;
		float dummy[2];
	};
	CBBreathParticle particleData;

	BreathParticle(size_t particleCount);
	~BreathParticle() = default;

	void Load(const char* particleName);

	// ムーブコンストラクタ、コピーコンストラクタを delete しておく
	BreathParticle(const BreathParticle&) = delete;
	BreathParticle(BreathParticle&&)	  = delete;
	BreathParticle& operator=(const BreathParticle&) = delete;
	BreathParticle& operator=(BreathParticle&&)		 = delete;

	void Initialize(const DirectX::XMFLOAT3& position, const float& size = 0.1f);
	void Update(const float& elapsedTime);
	void Render();

	void Begin(ID3D11DeviceContext* dc);
	void Draw(ID3D11DeviceContext* dc);
	void End(ID3D11DeviceContext* dc);

private:
	size_t maxParticleCount;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;

	// ID3D11UnorderedAccessView : レンダリング中にパイプラインにアクセスできるリソースの部分を指定します。
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uavParticle;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  srvParticle;

	// 各シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader>   vsParticle;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>    psParticle;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> gsParticle;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>  csInitializeParticle;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>  csParticle;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;

};

