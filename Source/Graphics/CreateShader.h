#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

namespace CreateShader
{
	// csoから情報読み込み
	long LoadFile(const char* csoName,
		std::unique_ptr<unsigned char[]>& csoData);

	// 頂点シェーダーのロード、入力レイアウトの生成
	HRESULT CreateVSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
		UINT numElements);

	// ピクセルシェーダーのロード
	HRESULT CreatePSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11PixelShader** pixelShader);

	// ジオメトリシェーダーのロード
	HRESULT CreateGSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11GeometryShader** geometryShader);

	// コンピュートシェーダーのロード
	HRESULT CreateCSFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11ComputeShader** computeShader);


	// 定数バッファのロード
	HRESULT CreateCB(ID3D11Device* device,
		UINT size,
		ID3D11Buffer** buffer);
}