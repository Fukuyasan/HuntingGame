#include "FullscreenQuad.h"
#include "CreateShader.h"
#include "System/Misc.h"
#include "Graphics.h"

FullscreeQuad::FullscreeQuad(ID3D11Device* device)
{
	CreateShader::CreateVSFromCso(device, "Shader/FullscreenQuadVS.cso", embeddedVertexShader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	CreateShader::CreatePSFromCso(device, "Shader/FullscreenQuadPS.cso", embeddedPixelShader.ReleaseAndGetAddressOf());
}

void FullscreeQuad::Blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** srv, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader)
{
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(nullptr);

	dc->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ? dc->PSSetShader(replacedPixelShader, 0, 0) : dc->PSSetShader(embeddedPixelShader.Get(), 0, 0);
	dc->PSSetShaderResources(startSlot, numViews, srv);

	dc->Draw(4, 0);
}
