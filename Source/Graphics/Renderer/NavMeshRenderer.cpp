#include <memory>
#include <RecastDebugDraw.h>
#include "System/Misc.h"
#include "NavMeshRenderer.h"
#include "Graphics/CreateShader.h"

NavMeshRenderer::NavMeshRenderer(ID3D11Device* device, UINT vertexCount)
	: capacity((vertexCount / 6) * 6 + 6)
{
	HRESULT hr = S_OK;

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダー作成
	CreateShader::CreateVSFromCso(device, "Shader\\NavMeshVS.cso", vertexShader.GetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	// ピクセルシェーダ
	CreateShader::CreatePSFromCso(device, "Shader\\NavMeshPS.cso", pixelShader.GetAddressOf());

	// 定数バッファ
	CreateShader::CreateCB(device, sizeof(CBuffer), constantBuffer.GetAddressOf());

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, alphaBlendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, maskZeroDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		hr = device->CreateDepthStencilState(&desc, maskAllDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, cullBackRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		desc.CullMode = D3D11_CULL_NONE;
		hr = device->CreateRasterizerState(&desc, cullNoneRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(Vertex) * capacity;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ダミーテクスチャ
	{
		const int size = 8;
		UINT pixels[size * size];
		::memset(pixels, 0xFF, sizeof(pixels));

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = size;
		desc.Height = size;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		data.pSysMem = pixels;
		data.SysMemPitch = size;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, dummyShaderResourceView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// チェッカーテクスチャ
	{
		const unsigned int col0 = duRGBA(215, 215, 215, 255);
		const unsigned int col1 = duRGBA(255, 255, 255, 255);
		const int size = 64;
		UINT pixels[size * size];

		//while (size > 0)
		{
			for (int y = 0; y < size; ++y)
			{
				for (int x = 0; x < size; ++x)
				{
					pixels[x + y * size] = (x == 0 || y == 0) ? col0 : col1;
				}
			}
		}

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = size;
		desc.Height = size;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		data.pSysMem = pixels;
		data.SysMemPitch = size * 4;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, checkerShaderResourceView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void NavMeshRenderer::Draw(dtNavMesh* navMesh, const dtNavMeshQuery* navQuery)
{
	if (navMesh && navQuery)
	{
		// ナビメッシュ描画
		duDebugDrawNavMeshWithClosedList(this, *navMesh, *navQuery, navMeshDrawFlags);
	}
}

void NavMeshRenderer::Draw(rcPolyMeshDetail* polyMeshDetail)
{
	if (polyMeshDetail)
	{
		// 詳細ポリゴンメッシュ描画
		duDebugDrawPolyMeshDetail(this, *polyMeshDetail);

	}
}

void NavMeshRenderer::Draw(dtCrowd* crowd)
{
	// エージェント描画
	if (crowd)
	{
		for (int i = 0; i < crowd->getAgentCount(); ++i)
		{
			const dtCrowdAgent* ag = crowd->getAgent(i);
			if (!ag->active) continue;

			const float radius = ag->params.radius;
			const float* pos = ag->npos;

			unsigned int col = duRGBA(0, 0, 0, 32);
			//if (m_agentDebug.idx == i)
			//	col = duRGBA(255, 0, 0, 128);

			duDebugDrawCircle(this, pos[0], pos[1], pos[2], radius, col, 2.0f);
		}

		for (int i = 0; i < crowd->getAgentCount(); ++i)
		{
			const dtCrowdAgent* ag = crowd->getAgent(i);
			if (!ag->active) continue;

			const float height = ag->params.height;
			const float radius = ag->params.radius;
			const float* pos = ag->npos;
			const float* vel = ag->vel;
			const float* dvel = ag->dvel;

			unsigned int col = duRGBA(220, 220, 220, 128);
			if (ag->targetState == DT_CROWDAGENT_TARGET_REQUESTING || ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE)
			{
				col = duLerpCol(col, duRGBA(128, 0, 255, 128), 32);
			}
			else if (ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_PATH)
			{
				col = duLerpCol(col, duRGBA(128, 0, 255, 128), 128);
			}
			else if (ag->targetState == DT_CROWDAGENT_TARGET_FAILED)
			{
				col = duRGBA(255, 32, 16, 128);
			}
			else if (ag->targetState == DT_CROWDAGENT_TARGET_VELOCITY)
			{
				col = duLerpCol(col, duRGBA(64, 255, 0, 128), 128);
			}

			duDebugDrawCylinder(this,
				pos[0] - radius, pos[1] + radius * 0.1f, pos[2] - radius,
				pos[0] + radius, pos[1] + height, pos[2] + radius, col
			);

			duDebugDrawCircle(this, pos[0], pos[1] + height, pos[2], radius, col, 2.0f);

			duDebugDrawArrow(this, pos[0], pos[1] + height, pos[2],
				pos[0] + dvel[0], pos[1] + height + dvel[1], pos[2] + dvel[2],
				0.0f, 0.4f, duRGBA(0, 192, 255, 192), 2.0f);
			duDebugDrawArrow(this, pos[0], pos[1] + height, pos[2],
				pos[0] + vel[0], pos[1] + height + vel[1], pos[2] + vel[2],
				0.0f, 0.4f, duRGBA(0, 0, 0, 160), 2.0f);
		}
	}
}

void NavMeshRenderer::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	if (primitives.size() > 0)
	{
		// シェーダー設定
		dc->IASetInputLayout(inputLayout.Get());
		dc->VSSetShader(vertexShader.Get(), nullptr, 0);
		dc->PSSetShader(pixelShader.Get(), nullptr, 0);
		dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		// 定数バッファ設定
		dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		// 定数バッファ更新
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX VP = V * P;
		CBuffer data;
		DirectX::XMStoreFloat4x4(&data.wvp, VP);
		dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);

		// 頂点バッファ設定
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

		const FLOAT blendFactor[] = { 1, 1, 1, 1 };
		dc->OMSetBlendState(alphaBlendState.Get(), blendFactor, 0xFFFFFFFF);

		Vertex* v = vertices.data();
		for (Primitive& primitive : primitives)
		{
			// 描画設定
			dc->IASetPrimitiveTopology(primitive.primitiveTopology);
			dc->OMSetDepthStencilState(primitive.depthStencilState, 0);
			dc->RSSetState(primitive.rasterizerState);
			dc->PSSetShaderResources(0, 1, &primitive.shaderResourceView);

			// 描画(頂点バッファサイズが足りない場合は複数回に分けて描画)
			UINT totalVertexCount = primitive.vertexCount;
			UINT startVertex = 0;
			UINT vertexCount = (totalVertexCount < capacity) ? totalVertexCount : capacity;
			while (startVertex < totalVertexCount)
			{
				// 頂点バッファ更新
				D3D11_MAPPED_SUBRESOURCE mappedVB;
				dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
				memcpy(mappedVB.pData, v + startVertex, sizeof(Vertex) * vertexCount);
				dc->Unmap(vertexBuffer.Get(), 0);

				// 描画実行
				dc->Draw(vertexCount, 0);

				// 次の描画準備
				startVertex += vertexCount;
				if ((startVertex + vertexCount) > totalVertexCount)
				{
					vertexCount = totalVertexCount - startVertex;
				}
			}
			v += totalVertexCount;
		}
		vertices.clear();
		primitives.clear();
	}
}

void NavMeshRenderer::depthMask(bool state)
{
	currentDepthMask = state;
}

void NavMeshRenderer::texture(bool state)
{
	currentTexture = state;
}

void NavMeshRenderer::begin(duDebugDrawPrimitives prim, float size)
{
	Primitive& primitive = primitives.emplace_back();

	quadCount = -1;
	vertexSize = -1;

	switch (prim)
	{
	case DU_DRAW_POINTS:
		vertexSize = size;
		primitive.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		primitive.rasterizerState = cullNoneRasterizerState.Get();
		break;

	case DU_DRAW_LINES:
		vertexSize = size;
		primitive.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		primitive.rasterizerState = cullNoneRasterizerState.Get();
		break;

	case DU_DRAW_QUADS:
		quadCount = 0;
		primitive.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		primitive.rasterizerState = cullBackRasterizerState.Get();
		break;

	case DU_DRAW_TRIS:
		primitive.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		primitive.rasterizerState = cullBackRasterizerState.Get();
		break;
	};

	primitive.startVertex = static_cast<UINT>(vertices.size());
	primitive.vertexCount = 0;
}

// 頂点積み込み
void NavMeshRenderer::vertex(const float* pos, unsigned int color)
{
	vertex(pos[0], pos[1], pos[2], color);
}

void NavMeshRenderer::vertex(const float x, const float y, const float z, unsigned int color)
{
	vertex(x, y, z, color, 0, 0);
}

void NavMeshRenderer::vertex(const float* pos, unsigned int color, const float* uv)
{
	vertex(pos[0], pos[1], pos[2], color, uv[0], uv[1]);
}

void NavMeshRenderer::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	Vertex& vertex = vertices.emplace_back();
	vertex.position.x = x;
	vertex.position.y = y;
	vertex.position.z = z;
	vertex.texcoord.x = u;
	vertex.texcoord.y = v;
	vertex.color = color;

	Primitive& primitive = primitives.back();
	primitive.vertexCount++;

	// クアッドの場合はトライアングルリストで表示するための残り２点を追加
	if (quadCount >= 0)
	{
		quadCount++;
		if (quadCount == 4)
		{
			Vertex* v = &vertex - 3;
			vertices.emplace_back(v[0]);
			vertices.emplace_back(v[2]);

			quadCount = 0;
			primitive.vertexCount += 2;
		}
	}
}

void NavMeshRenderer::end()
{

	Primitive& primitive = primitives.back();

	if (currentDepthMask)
	{
		primitive.depthStencilState = maskAllDepthStencilState.Get();
	}
	else
	{
		primitive.depthStencilState = maskZeroDepthStencilState.Get();
	}

	if (currentTexture)
	{
		primitive.shaderResourceView = checkerShaderResourceView.Get();
	}
	else
	{
		primitive.shaderResourceView = dummyShaderResourceView.Get();
	}
}
