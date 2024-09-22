#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DebugDraw.h>
#include <DetourDebugDraw.h>
#include <DetourNavMesh.h>
#include <DetourCrowd.h>
#include <Recast.h>

class NavMeshRenderer : public duDebugDraw
{
public:
	NavMeshRenderer(ID3D11Device* device, UINT vertexCount = 4096);
	~NavMeshRenderer() override = default;

public:
	// ナビメッシュ描画
	void Draw(dtNavMesh* navMesh,const dtNavMeshQuery* navQuery);
	// 詳細ポリゴンメッシュ描画
	void Draw(rcPolyMeshDetail* polyMeshDetail);
	// エージェント描画
	void Draw(dtCrowd* crowd);

	// 描画実行
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

private:
	void depthMask(bool state) override;
	void texture(bool state) override;
	void begin(duDebugDrawPrimitives prim, float size = 1.0f) override;
	void vertex(const float* pos, unsigned int color) override;
	void vertex(const float x, const float y, const float z, unsigned int color) override;
	void vertex(const float* pos, unsigned int color, const float* uv) override;
	void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v) override;
	void end() override;

private:
	struct CBuffer
	{
		DirectX::XMFLOAT4X4 wvp;
	};

	struct Primitive
	{
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology   = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ID3D11RasterizerState* rasterizerState       = nullptr;
		ID3D11DepthStencilState* depthStencilState   = nullptr;
		ID3D11ShaderResourceView* shaderResourceView = nullptr;

		UINT						vertexCount = 0;
		UINT						startVertex = 0;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT2	texcoord;
		UINT				color;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		alphaBlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	cullBackRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	cullNoneRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	maskZeroDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	maskAllDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	checkerShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	dummyShaderResourceView;

	std::vector<Primitive>		primitives;
	std::vector<Vertex>			vertices;
	UINT						capacity = 0;
	int							quadCount = 0;
	float						vertexSize = 0;

	bool			currentDepthMask = false;
	bool			currentTexture = false;
	unsigned char	navMeshDrawFlags = DU_DRAWNAVMESH_OFFMESHCONS | DU_DRAWNAVMESH_CLOSEDLIST;

};