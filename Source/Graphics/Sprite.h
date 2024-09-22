#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

// スプライト
class Sprite
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

public:
	Sprite() : Sprite(nullptr) {}
	Sprite(const char* filename);
	~Sprite() = default;

	// 描画実行
	void Update(
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;

	void Update(
		const DirectX::XMFLOAT2& position,
		const DirectX::XMFLOAT2& size,
		const DirectX::XMFLOAT2& scale,
		const DirectX::XMFLOAT2& texPos,
		const DirectX::XMFLOAT2& texSize,
		const float angle = 0,
		const DirectX::XMFLOAT4& color = {1,1,1,1}) const;

	void Update(
		const DirectX::XMFLOAT2& position,
		const DirectX::XMFLOAT2& texPos,
		const DirectX::XMFLOAT2& texSize,
		const float angle = 0,
		const DirectX::XMFLOAT4& color = { 1,1,1,1 }) const;

#pragma region セッター ゲッター
	// テクスチャ幅取得、高さ取得
	const int GetTextureWidth() const { return textureWidth; }
	const int GetTextureHeight() const { return textureHeight; }

	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return vertexBuffer; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return shaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView() const { return shaderResourceView; }
#pragma endregion

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer>			 vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	int textureWidth  = 0;
	int textureHeight = 0;
};