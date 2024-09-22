#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>

#include "Graphics/Sprite.h"

class SpriteShader
{
public:
	SpriteShader(const char* csoName) { Load(csoName); }
	~SpriteShader() {}

	void Load(const char* csoName);

	void Begin(ID3D11DeviceContext* dc);
	void Draw(ID3D11DeviceContext* dc, const Sprite* sprite);
	void End(ID3D11DeviceContext* dc);

protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout;

};