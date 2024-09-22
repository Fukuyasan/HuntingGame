#include <stdio.h>
#include <memory>
#include "System/Misc.h"

#include "Graphics/ShaderState.h"
#include "Graphics/CreateShader.h"

#include "DebugRenderer.h"
#include "LineRenderer.h"

DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// シェーダー設定
	CreateShader::CreateVSFromCso(device, "Shader\\DebugVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	CreateShader::CreatePSFromCso(device, "Shader\\DebugPS.cso", pixelShader.ReleaseAndGetAddressOf());
	
	// 定数バッファ設定
	CreateShader::CreateCB(device, sizeof(CbMesh), constantBuffer.ReleaseAndGetAddressOf());

	// メッシュ作成
	CreateSphereMesh(device, 1.0f, 16, 16);
	CreateCylinderMesh(device, 1.0f, 1.0f, 0.0f, 1.0f, 16, 1);
	CreateSquareMesh(device, 1.0f);
}

// 描画
void DebugRenderer::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	//dc->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;

	// プリミティブ設定
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// プリミティブ描画
	DrawSphere(  dc, &stride, &offset, VP);
	DrawCilinder(dc, &stride, &offset, VP);
	DrawSquare(  dc, &stride, &offset, VP);
}

// ------------------------------------------
//
//		 デバッグ表示するプリミティブ追加
//
// ------------------------------------------

void DebugRenderer::AddSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	sphere.color  = color;
	spheres.emplace_back(sphere);
}

void DebugRenderer::AddCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color)
{
	Cylinder cylinder;
	cylinder.position = position;
	cylinder.radius   = radius;
	cylinder.height   = height;
	cylinder.color    = color;
	cylinders.emplace_back(cylinder);
}

void DebugRenderer::AddCapsule(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& goalPos, float radius, const DirectX::XMFLOAT4& color)
{
	// カプセルの球体の部分
	Sphere sphere;
	sphere.center = startPos;
	sphere.radius = radius;
	sphere.color  = color;
	spheres.emplace_back(sphere);
	sphere.center = goalPos;
	spheres.emplace_back(sphere);

	// カプセルの円柱の部分
	LineRenderer* line = ShaderState::Instance().GetLineRenderer();

	int   slices = 16;
	float dTheta = DirectX::XM_2PI / slices;

	for (int i = 0; i < slices; i++)
	{
		float phi = i * dTheta;

		float x = startPos.x + sinf(phi) * radius;
		float y = startPos.y;
		float z = startPos.z + cosf(phi) * radius;

		line->AddVertex({x,y,z}, color);

		x = goalPos.x + sinf(phi) * radius;
		y = goalPos.y;
		z = goalPos.z + cosf(phi) * radius;

		line->AddVertex({x,y,z}, color);
	}
}

void DebugRenderer::AddSquare(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color)
{
	Square square;
	square.position = position;
	square.size     = size;
	square.color    = color;
	squares.emplace_back(square);
}

// ------------------------------------------
//
//				 メッシュ作成
//
// ------------------------------------------

void DebugRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks)
//void DebugRenderer::CreateSphereMesh(ID3D11Device* device, float start, float radius, int slices, int stacks)
{
	sphereVertexCount = stacks * slices * 2 + slices * stacks * 2;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(sphereVertexCount);

	DirectX::XMFLOAT3* p = vertices.get();
	
	CreateSphere(p, radius, 0.0f, slices, stacks);

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth                   = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * sphereVertexCount);
		desc.Usage                       = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags                   = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags              = 0;
		desc.MiscFlags                   = 0;
		desc.StructureByteStride         = 0;
		subresourceData.pSysMem          = vertices.get();
		subresourceData.SysMemPitch      = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, sphereVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void DebugRenderer::CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks)
{
	cylinderVertexCount = 2 * slices * (stacks + 1) + 2 * slices;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(cylinderVertexCount);

	DirectX::XMFLOAT3* p = vertices.get();

	CreateCylinder(p, radius1, radius2, start, height, slices, stacks);

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth                   = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * cylinderVertexCount);
		desc.Usage                       = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags                   = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags              = 0;
		desc.MiscFlags                   = 0;
		desc.StructureByteStride         = 0;
		subresourceData.pSysMem          = vertices.get();
		subresourceData.SysMemPitch      = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, cylinderVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void DebugRenderer::CreateSquareMesh(ID3D11Device* device, float radius)
{
	// ８頂点＊4面＝３２頂点
	squareVertexCount = 32;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(squareVertexCount);

	DirectX::XMFLOAT3* p = vertices.get();

	CreateSquare(p, radius);

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth                   = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * squareVertexCount);
		desc.Usage                       = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags                   = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags              = 0;
		desc.MiscFlags                   = 0;
		desc.StructureByteStride         = 0;
		subresourceData.pSysMem          = vertices.get();
		subresourceData.SysMemPitch      = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, squareVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// ------------------------------------------
//
//				プリミティブ作成
//
// ------------------------------------------

void DebugRenderer::CreateSphere(DirectX::XMFLOAT3* p, float radius, float start, int slices, int stacks)
{
	float phiStep = DirectX::XM_PI / stacks;
	float thetaStep = DirectX::XM_2PI / slices;

	// 横線の設定
	for (int i = 0; i < stacks; ++i)
	{
		float phi = i * phiStep;
		float y = radius * cosf(phi);
		float r = radius * sinf(phi);

		// 縦線の設定
		for (int j = 0; j < slices; ++j)
		{
			// 
			float theta = j * thetaStep;
			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;

			theta += thetaStep;

			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;
		}
	}

	thetaStep = DirectX::XM_2PI / stacks;
	for (int i = 0; i < slices; ++i)
	{
		DirectX::XMMATRIX M = DirectX::XMMatrixRotationY(i * thetaStep);
		for (int j = 0; j < stacks; ++j)
		{
			float theta = j * thetaStep;
			DirectX::XMVECTOR V1 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P1 = DirectX::XMVector3TransformCoord(V1, M);
			DirectX::XMStoreFloat3(p++, P1);

			int n = (j + 1) % stacks;
			theta += thetaStep;

			DirectX::XMVECTOR V2 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P2 = DirectX::XMVector3TransformCoord(V2, M);
			DirectX::XMStoreFloat3(p++, P2);
		}
	}
}

void DebugRenderer::CreateCylinder(DirectX::XMFLOAT3* p, float radius1, float radius2, float start, float height, int slices, int stacks)
{
	float stackHeight = height / stacks;
	float radiusStep = (radius2 - radius1) / stacks;

	// vertices of ring
	float dTheta = DirectX::XM_2PI / slices;

	// カプセルの円柱部分の設定
	for (int i = 0; i < slices; ++i)
	{
		int n = (i + 1) % slices;

		float c1 = cosf(i * dTheta);
		float s1 = sinf(i * dTheta);

		float c2 = cosf(n * dTheta);
		float s2 = sinf(n * dTheta);

		for (int j = 0; j <= stacks; ++j)
		{
			float y = start + j * stackHeight;
			float r = radius1 + j * radiusStep;

			p->x = r * c1;
			p->y = y;
			p->z = r * s1;
			p++;

			p->x = r * c2;
			p->y = y;
			p->z = r * s2;
			p++;
		}

		// 下の円
		p->x = radius1 * c1;
		p->y = start;
		p->z = radius1 * s1;
		p++;

		// 上の円
		p->x = radius2 * c1;
		p->y = start + height;
		p->z = radius2 * s1;
		p++;
	}
}

void DebugRenderer::CreateSquare(DirectX::XMFLOAT3* p, float radius)
{
	const float max = radius * 0.5f;
	const float min = -max;

	// Z軸
	for (int Zplane = 0; Zplane < 2; ++Zplane)
	{
		// 奥行き
		float Zdepth = max * (-2 * Zplane + 1);

		// Z ↑
		// 0,4--------1
		//   |		  |
		//   |		  |
		//   |		  |
		//   3--------2
		 
		// Z ↓
		// 5,9--------6
		//   |		  |
		//   |		  |
		//   |		  |
		//   8--------7

		// 左上
		p->x = min;
		p->y = max;
		p->z = Zdepth;
		p++;

		// 右上
		for (int i = 0; i < 2; ++i)
		{
			p->x = max;
			p->y = max;
			p->z = Zdepth;
			p++;
		}

		// 右下
		for (int i = 0; i < 2; ++i)
		{
			p->x = max;
			p->y = min;
			p->z = Zdepth;
			p++;
		}

		// 左下
		for (int i = 0; i < 2; ++i)
		{
			p->x = min;
			p->y = min;
			p->z = Zdepth;
			p++;
		}

		// 左上
		p->x = min;
		p->y = max;
		p->z = Zdepth;
		p++;
	}

	// Z軸
	for (int Xplane = 0; Xplane < 2; ++Xplane)
	{
		// 奥行き
		float Xdepth = max * (-2 * Xplane + 1);

		// X ↑
		// 10,14--------11
		//     |		 |
		//     |		 |
		//     |		 |
		//    13--------12

		// X ↓
		// 15,19--------16
		//     |		 |
		//     |		 |
		//     |		 |
		//    18--------17

		// 左上
		p->x = Xdepth;
		p->y = max;
		p->z = min;
		p++;

		// 右上
		for (int i = 0; i < 2; ++i)
		{
			p->x = Xdepth;
			p->y = max;
			p->z = max;
			p++;
		}

		// 右下
		for (int i = 0; i < 2; ++i)
		{
			p->x = Xdepth;
			p->y = min;
			p->z = max;
			p++;
		}

		// 左下
		for (int i = 0; i < 2; ++i)
		{
			p->x = Xdepth;
			p->y = min;
			p->z = min;
			p++;
		}

		// 左上
		p->x = Xdepth;
		p->y = max;
		p->z = min;
		p++;
	}
}

// ------------------------------------------
//
//					描画
//
// ------------------------------------------

void DebugRenderer::DrawSphere(ID3D11DeviceContext* dc, UINT* stride, UINT* offset, const DirectX::XMMATRIX& VP)
{
	dc->IASetVertexBuffers(0, 1, sphereVertexBuffer.GetAddressOf(), stride, offset);
	for (const Sphere& sphere : spheres)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(sphere.radius, sphere.radius, sphere.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(sphere.center.x, sphere.center.y, sphere.center.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = sphere.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		dc->Draw(sphereVertexCount, 0);
	}
	spheres.clear();
}

void DebugRenderer::DrawCilinder(ID3D11DeviceContext* dc, UINT* stride, UINT* offset, const DirectX::XMMATRIX& VP)
{
	dc->IASetVertexBuffers(0, 1, cylinderVertexBuffer.GetAddressOf(), stride, offset);
	for (const Cylinder& cylinder : cylinders)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cylinder.radius, cylinder.height, cylinder.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cylinder.position.x, cylinder.position.y, cylinder.position.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = cylinder.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		dc->Draw(cylinderVertexCount, 0);
	}
	cylinders.clear();
}

void DebugRenderer::DrawSquare(ID3D11DeviceContext* dc, UINT* stride, UINT* offset, const DirectX::XMMATRIX& VP)
{
	dc->IASetVertexBuffers(0, 1, squareVertexBuffer.GetAddressOf(), stride, offset);
	for (const Square& square : squares)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S   = DirectX::XMMatrixScaling(square.size.x, square.size.y, square.size.z);
		DirectX::XMMATRIX T   = DirectX::XMMatrixTranslation(square.position.x, square.position.y, square.position.z);
		DirectX::XMMATRIX W   = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = square.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		dc->Draw(squareVertexCount, 0);
	}
	squares.clear();
}
