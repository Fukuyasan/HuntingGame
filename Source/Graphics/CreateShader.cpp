#include <d3d11.h>
#include <wrl.h>
#include <sstream>

#include "CreateShader.h"
#include "System/Misc.h"

// csoから情報読み込み
long CreateShader::LoadFile(const char* csoName, std::unique_ptr<unsigned char[]>& csoData)
{
	FILE* fp = nullptr;

	// csoNameのポインタを受け取る
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// ファイルポインタを0バイトの地点から終端まで移動させる
	fseek(fp, 0, SEEK_END);
	// ファイルポインタのサイズを取得
	long csoSize = ftell(fp);
	// ファイルポインタを先端から0バイトの地点まで移動させる
	fseek(fp, 0, SEEK_SET);

	// ファイルポインタのサイズの１バイトまで読み込み、csoDataに格納する
	csoData = std::make_unique<unsigned char[]>(csoSize);
	fread(csoData.get(), csoSize, 1, fp);
	fclose(fp);

	return csoSize;
}

// 頂点シェーダーのロード、入力レイアウトの生成
HRESULT CreateShader::CreateVSFromCso(ID3D11Device* device,
	const char* csoName,
	ID3D11VertexShader** vertexShader,
	ID3D11InputLayout** inputLayout,
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
	UINT numElements)
{
	// ファイルロード
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);

	HRESULT hr = device->CreateVertexShader(
		csoData.get(),
		csoSize,
		nullptr,
		vertexShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	
	// 入力レイアウトの生成
	if (inputLayout)
	{		
		hr = device->CreateInputLayout(inputElementDesc, numElements, csoData.get(), csoSize, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	return hr;
}

// ピクセルシェーダオブジェクトの生成
HRESULT CreateShader::CreatePSFromCso(ID3D11Device* device,
	const char* csoName,
	ID3D11PixelShader** pixelShader)
{
	// ファイルロード
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);
		
	HRESULT hr = device->CreatePixelShader(
		csoData.get(),
		csoSize,
		nullptr,
		pixelShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// ジオメトリシェーダーオブジェクトの生成
HRESULT CreateShader::CreateGSFromCso(ID3D11Device* device, const char* csoName, ID3D11GeometryShader** geometryShader)
{
	// ファイルロード
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);

	HRESULT hr = device->CreateGeometryShader(
		csoData.get(),
		csoSize,
		nullptr,
		geometryShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;	
}

// コンピュートシェーダーオブジェクトの生成
HRESULT CreateShader::CreateCSFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** computeShader)
{
	// ファイルロード
	std::unique_ptr<unsigned char[]> csoData;
	int csoSize = LoadFile(csoName, csoData);

	HRESULT hr = device->CreateComputeShader(
		csoData.get(),
		csoSize,
		nullptr,
		computeShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// シーン用バッファの作成
HRESULT CreateShader::CreateCB(ID3D11Device* device, UINT size, ID3D11Buffer** buffer)
{
	
	D3D11_BUFFER_DESC desc;
	::memset(&desc, 0, sizeof(desc));
	desc.ByteWidth           = size;
	desc.Usage               = D3D11_USAGE_DEFAULT;
	desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags      = 0;
	desc.MiscFlags           = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}