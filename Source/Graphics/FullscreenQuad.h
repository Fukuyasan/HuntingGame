#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <cstdint>

class FullscreeQuad
{
public:
	FullscreeQuad(ID3D11Device* device);
	virtual ~FullscreeQuad() = default;

	void Blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** srv,
		uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader = nullptr);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  embeddedPixelShader;
};
