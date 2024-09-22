#pragma once
#include <wrl.h>
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include <deque>

class TrailShader
{
private:
	static const UINT VertexCapacity = 1024;
private:
	// トレイルの位置情報
	struct TrailParam
	{
		DirectX::XMFLOAT3 head;  // 先端
		DirectX::XMFLOAT3 tail;  // 末端
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4 materialColor;
	};

public:
	TrailShader(const char* filename, const int maxTrail = 8);
	~TrailShader() {}

	void Update();
	void Render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection, int blend);

	// トレイルの位置情報セット
	void Set(const DirectX::XMFLOAT3& head, const DirectX::XMFLOAT3& tail);

	// 頂点バッファにセット
	void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texcoord);

private:
	void Begin(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection, int blend);
	void End(ID3D11DeviceContext* dc);

private:
	std::deque<TrailParam> trailParams;

	std::vector<Vertex> vertices;

	std::unique_ptr<Texture> texture = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> subsetConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout;

	int maxTrailCount = 8;
};