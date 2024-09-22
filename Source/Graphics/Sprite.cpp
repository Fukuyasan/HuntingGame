#include <stdio.h> 
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "Sprite.h"
#include "System/Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/CreateShader.h"
#include "Texture.h"

// コンストラクタ
Sprite::Sprite(const char* filename)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	HRESULT hr = S_OK;

	// 頂点データの定義
	// 0           1
	// +-----------+
	// |           |
	// |           |
	// +-----------+
	// 2           3
	Vertex vertices[] = {
		{ DirectX::XMFLOAT3(-1.0, +1.0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(+1.0, +1.0, 0), DirectX::XMFLOAT4(1, 0, 0, 1) },
		{ DirectX::XMFLOAT3(-1.0, -1.0, 0), DirectX::XMFLOAT4(0, 1, 0, 1) },
		{ DirectX::XMFLOAT3(+1.0, -1.0, 0), DirectX::XMFLOAT4(0, 0, 1, 1) },
	};

	// ポリゴンを描画するにはGPUに頂点データやシェーダーなどのデータを渡す必要がある。
	// GPUにデータを渡すにはID3D11***のオブジェクトを介してデータを渡します。

	// 頂点バッファの生成
	{
		// 頂点バッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc           = {};
		// 頂点バッファに頂点データを入れるための設定
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		buffer_desc.ByteWidth                   = sizeof(vertices);	// バッファ（データを格納する入れ物）のサイズ
		buffer_desc.Usage                       = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags                   = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとしてバッファを作成する。
		buffer_desc.CPUAccessFlags              = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags                   = 0;
		buffer_desc.StructureByteStride         = 0;		
		subresource_data.pSysMem                = vertices;	// ここに格納したい頂点データのアドレスを渡すことでCreateBuffer()時にデータを入れることができる。
		subresource_data.SysMemPitch            = 0; //頂点バッファでは扱わない
		subresource_data.SysMemSlicePitch       = 0; //頂点バッファでは扱わない
		// 頂点バッファオブジェクトの生成
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertexBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// テクスチャの生成
	if (filename != nullptr)
	{		
		// テクスチャ情報の取得
		D3D11_TEXTURE2D_DESC desc;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		Texture::LoadTexture(filename, device, shaderResourceView.GetAddressOf(), &desc);

		textureWidth  = desc.Width;
		textureHeight = desc.Height;
	}
	else
	{
		// ダミーテクスチャ作成
		const int width  = 8;
		const int height = 8;
		UINT pixels[width * height];
		::memset(pixels, 0xFF, sizeof(pixels));

		D3D11_TEXTURE2D_DESC desc = { 0 };
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		desc.Width                = width;
		desc.Height               = height;
		desc.MipLevels            = 1;
		desc.ArraySize            = 1;
		desc.Format               = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count     = 1;
		desc.SampleDesc.Quality   = 0;
		desc.Usage                = D3D11_USAGE_DEFAULT;
		desc.BindFlags            = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags       = 0;
		desc.MiscFlags            = 0;
		
		data.pSysMem              = pixels;
		data.SysMemPitch          = width;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureWidth  = desc.Width;
		textureHeight = desc.Height;
	}
}

void Sprite::Update(
	float dx, float dy,
	float dw, float dh, 
	float sx, float sy, 
	float sw, float sh,
	float angle, 
	float r, float g, float b, float a) const
{
	{
		ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

		// 現在設定されているビューポートからスクリーンサイズを取得する。
		D3D11_VIEWPORT viewport;
		UINT numViewports   = 1;
		dc->RSGetViewports(&numViewports, &viewport);
		float screen_width  = viewport.Width;
		float screen_height = viewport.Height;

		// スプライトを構成する４頂点のスクリーン座標を計算する
		DirectX::XMFLOAT2 positions[] = {
			DirectX::XMFLOAT2(dx,			dy),  // 左上
			DirectX::XMFLOAT2(dx + dw,		dy),  // 右上
			DirectX::XMFLOAT2(dx,      dy + dh),  // 左下
			DirectX::XMFLOAT2(dx + dw, dy + dh),  // 右下
		};

		// スプライトを構成する４頂点のテクスチャ座標を計算する
		DirectX::XMFLOAT2 texcoords[] = {
			DirectX::XMFLOAT2(sx,			sy),  // 左上
			DirectX::XMFLOAT2(sx + sw,		sy),  // 右上
			DirectX::XMFLOAT2(sx,      sy + sh),  // 左下
			DirectX::XMFLOAT2(sx + sw, sy + sh),  // 右下
		};

		// スプライトの中心で回転させるために４頂点の中心位置が
		// 原点(0, 0)になるように一旦頂点を移動させる。
		float mx = dx + dw * 0.5f;
		float my = dy + dh * 0.5f;
		for (auto& p : positions)
		{
			p.x -= mx;
			p.y -= my;
		}

		// 頂点を回転させる
		float theta = DirectX::XMConvertToRadians(angle);  // 角度をラジアン(θ)に変換
		float c     = cosf(theta);
		float s     = sinf(theta);
		for (auto& p : positions)
		{
			DirectX::XMFLOAT2 r = p;
			p.x = c * r.x + -s * r.y;
			p.y = s * r.x +  c * r.y;
		}

		// 回転のために移動させた頂点を元の位置に戻す
		for (auto& p : positions)
		{
			p.x += mx;
			p.y += my;
		}

		// スクリーン座標系からNDC座標系へ変換する。
		for (auto& p : positions)
		{
			p.x = 2.0f * p.x / screen_width - 1.0f;
			p.y = 1.0f - 2.0f * p.y / screen_height;
		}

		// 頂点バッファの内容の編集を開始する。
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// pDataを編集することで頂点データの内容を書き換えることができる。
		Vertex* v = static_cast<Vertex*>(mappedBuffer.pData);
		for (int i = 0; i < 4; ++i)
		{
			v[i].position.x = positions[i].x;
			v[i].position.y = positions[i].y;
			v[i].position.z = 0.0f;

			v[i].color.x = r;
			v[i].color.y = g;
			v[i].color.z = b;
			v[i].color.w = a;

			v[i].texcoord.x = texcoords[i].x / textureWidth;
			v[i].texcoord.y = texcoords[i].y / textureHeight;
		}

		// 頂点バッファの内容の編集を終了する。
		dc->Unmap(vertexBuffer.Get(), 0);
	}
}

void Sprite::Update(
	const DirectX::XMFLOAT2& position,
	const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& texPos,
	const DirectX::XMFLOAT2& texSize,
	const DirectX::XMFLOAT2& center,
	const float angle,
	const DirectX::XMFLOAT4& color) const
{
	float width  = texSize.x * scale.x;
	float height = texSize.y * scale.y;

	float pivotX = (width  / texSize.x) * center.x;
	float pivotY = (height / texSize.y) * center.y;

	float posX = position.x - pivotX;
	float posY = position.y - pivotY;

	Update(
		posX,		posY,
		width,		height,
		texPos.x,	texPos.y,
		texSize.x,	texSize.y,
		angle,
		color.x, color.y, color.z, color.w);
}

void Sprite::Update(
	const DirectX::XMFLOAT2& position,
	const DirectX::XMFLOAT2& texPos, 
	const DirectX::XMFLOAT2& texSize, 
	const float angle, 
	const DirectX::XMFLOAT4& color) const
{
	Update(position, { 1,1 }, texPos, texSize, {0,0}, angle, color);
}
