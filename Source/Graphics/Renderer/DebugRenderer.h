#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class DebugRenderer
{
public:
	DebugRenderer(ID3D11Device* device);
	~DebugRenderer() {}

public:
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// プリミティブ追加
	void AddSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);
	
	void AddCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);
	
	void AddCapsule(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& goalPos, float radius, const DirectX::XMFLOAT4& color);
	
	void AddSquare(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color);

private:
	// slices : 縦線の数
	// stacks : 横線の数
	
	// メッシュ作成
	void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);
	void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);
	void CreateSquareMesh(ID3D11Device* device, float radius);

	// プリミティブ作成
	void CreateSphere(DirectX::XMFLOAT3* p, float radius, float start, int slices, int stacks);
	void CreateCylinder(DirectX::XMFLOAT3* p, float radius1, float radius2, float start, float height, int slices, int stacks);
	void CreateSquare(DirectX::XMFLOAT3* p, float radius);

private:
	// 描画
	void DrawSphere(ID3D11DeviceContext* dc,   UINT* stride, UINT* offset, const DirectX::XMMATRIX& VP);
	void DrawCilinder(ID3D11DeviceContext* dc, UINT* stride, UINT* offset, const DirectX::XMMATRIX& VP);
	void DrawSquare(ID3D11DeviceContext* dc,   UINT* stride, UINT* offset, const DirectX::XMMATRIX& VP);

private:
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	wvp;
		DirectX::XMFLOAT4	color;
	};

	// 球
	struct Sphere
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	center;
		float				radius;
	};

	// 円柱
	struct Cylinder
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
		float				radius;
		float				height;
	};

	// 四角
	struct Square
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 size;
		DirectX::XMFLOAT4 color;
	};

	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> sphereVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cylinderVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> squareVertexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	// シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout;

	// 配列
	std::vector<Sphere>	  spheres;
	std::vector<Cylinder> cylinders;
	std::vector<Square> squares;

	// 頂点数
	UINT	sphereVertexCount   = 0;
	UINT	cylinderVertexCount = 0;
	UINT	squareVertexCount   = 0;
};
