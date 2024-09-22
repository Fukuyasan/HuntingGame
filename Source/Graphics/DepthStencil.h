#pragma once

#include <wrl.h>
#include <d3d11.h>

// 深度ステンシルバッファ
class DepthStencil
{
public:
	DepthStencil(UINT width, UINT height);
	~DepthStencil() {}

	// シェーダーリソースビュー取得
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSRV() { return srv; }

	// 深度ステンシルビュー取得
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV() { return dsv; }

	// テクスチャの情報取得
	D3D11_TEXTURE2D_DESC GetTexture2dDesc() { return texture2dDesc; }
	inline int GetWidth() const { return texture2dDesc.Width; }
	inline int GetHeight() const { return texture2dDesc.Height; }
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   dsv;
	D3D11_TEXTURE2D_DESC							 texture2dDesc;
};