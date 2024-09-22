#include <stdlib.h>
#include <fstream>
#include <functional>
#include <cereal.hpp>
#include <archives/binary.hpp>
#include <types/string.hpp>
#include <types/vector.hpp>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#define BUFFER_SIZE 256
#include <stb_image.h>

#include "ModelResource.h"
#include "System/Misc.h"
#include "System/Logger.h"
#include "System/String/StringFunction.h"

#include "Cereal/Cereal.h"

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelResource::Node, 1)
CEREAL_CLASS_VERSION(ModelResource::Material, 1)
CEREAL_CLASS_VERSION(ModelResource::Subset, 1)
CEREAL_CLASS_VERSION(ModelResource::Vertex, 1)
CEREAL_CLASS_VERSION(ModelResource::Mesh, 1)
CEREAL_CLASS_VERSION(ModelResource::NodeKeyData, 1)
CEREAL_CLASS_VERSION(ModelResource::Keyframe, 1)
CEREAL_CLASS_VERSION(ModelResource::Animation, 1)
CEREAL_CLASS_VERSION(ModelResource, 1)

template<class Archive>
void ModelResource::Node::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(id),
		CEREAL_NVP(name),
		CEREAL_NVP(path),
		CEREAL_NVP(parentIndex),
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::Material::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(textureFilename),
		CEREAL_NVP(normalTexture),
		CEREAL_NVP(metallicTexture),
		CEREAL_NVP(roughnessTexture),
		CEREAL_NVP(ambientOcclusionTexture),
		//CEREAL_NVP(emissionTexture),
		CEREAL_NVP(textureNarrow),
		CEREAL_NVP(color)
	);
}

template<class Archive>
void ModelResource::Subset::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(startIndex),
		CEREAL_NVP(indexCount),
		CEREAL_NVP(materialIndex)
	);
}

template<class Archive>
void ModelResource::Vertex::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(position),
		CEREAL_NVP(normal),
		CEREAL_NVP(tangent),
		CEREAL_NVP(texcoord),
		CEREAL_NVP(color),
		CEREAL_NVP(boneWeight),
		CEREAL_NVP(boneIndex)
	);
}

template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(vertices),
		CEREAL_NVP(indices),
		CEREAL_NVP(subsets),
		CEREAL_NVP(nodeIndex),
		CEREAL_NVP(nodeIndices),
		CEREAL_NVP(offsetTransforms),
		CEREAL_NVP(boundsMin),
		CEREAL_NVP(boundsMax)
	);
}

template<class Archive>
void ModelResource::NodeKeyData::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::Keyframe::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(seconds),
		CEREAL_NVP(nodeKeys)
	);
}

template<class Archive>
void ModelResource::Animation::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(secondsLength),
		CEREAL_NVP(keyframes)
	);
}

// 読み込み
void ModelResource::Load(ID3D11Device* device, const char* filename)
{
	// ディレクトリパス取得
	char drive[32], dir[256], dirname[256];
	::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
	::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

	// デシリアライズ
	Deserialize(filename);

	// モデル構築
	BuildModel(device, dirname);
}

void ModelResource::LoadAnimation(const char* filename)
{
	// ディレクトリパス取得
	char drive[32], dir[BUFFER_SIZE], dirname[BUFFER_SIZE];
	::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
	::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

	// デシリアライズ
	DeserializeAnimation(filename);
}

// モデル構築
void ModelResource::BuildModel(ID3D11Device* device, const char* dirname)
{
	for (Material& material : materials)
	{
		// 相対パスの解決
		char filename[256];

		// アルベドテクスチャ読み込み
		::_makepath_s(filename, 256, nullptr, dirname, material.textureFilename.c_str(), nullptr);
		LoadTexture(device, filename, nullptr, material.albedoMap.GetAddressOf(), 0xFFFFFFFF);

		// ノーマルテクスチャ読み込み
		::_makepath_s(filename, 256, nullptr, dirname, material.normalTexture.c_str(), nullptr);
		LoadTexture(device, filename, nullptr, material.normalMap.GetAddressOf(), 0xFFFF7E7E);

		// メタリックテクスチャ読み込み
		::_makepath_s(filename, 256, nullptr, dirname, material.metallicTexture.c_str(), nullptr);
		LoadTexture(device, filename, nullptr, material.metallicMap.GetAddressOf(), 0xFFFFFF0C);

		// ラフネステクスチャ読み込み
		::_makepath_s(filename, 256, nullptr, dirname, material.roughnessTexture.c_str(), nullptr);
		LoadTexture(device, filename, nullptr, material.roughnessMap.GetAddressOf(), 0xFFFFFFFF);

		// AOテクスチャ読み込み
		::_makepath_s(filename, 256, nullptr, dirname, material.ambientOcclusionTexture.c_str(), nullptr);
		LoadTexture(device, filename, nullptr, material.ambientOcclusionMap.GetAddressOf(), 0xFFFFFFFF);

		//// エミッションテクスチャ読み込み
		//::_makepath_s(filename, 256, nullptr, dirname, material.emissionTexture.c_str(), nullptr);
		//LoadTexture(device, filename, nullptr, material.emissionMap.GetAddressOf(), 0);
		
	}

	for (Mesh& mesh : meshes)
	{
		// サブセット
		for (Subset& subset : mesh.subsets)
		{
			subset.material = &materials.at(subset.materialIndex);
		}

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.vertices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * mesh.indices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.indices.data();
			subresourceData.SysMemPitch = 0; //Not use for index buffers.
			subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		//// バウンディングボックス
		//{
		//	DirectX::XMVECTOR ExtentMin = DirectX::XMLoadFloat3(&mesh.boundsMin);
		//	DirectX::XMVECTOR ExtentMax = DirectX::XMLoadFloat3(&mesh.boundsMax);

		//	DirectX::BoundingBox::CreateFromPoints(mesh.boundingBox, ExtentMin, ExtentMax);
		//}
	}
}

// テクスチャ読み込み
void ModelResource::LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, ID3D11ShaderResourceView** srv, UINT dummyColor)
{
	// ファイルのパスを分解
	char drive[BUFFER_SIZE], dirname[BUFFER_SIZE], fname[BUFFER_SIZE], ext[BUFFER_SIZE];
	::_splitpath_s(filename, drive, sizeof(drive), dirname, sizeof(dirname), fname, sizeof(fname), ext, sizeof(ext));

	// drive   : ドライブ文字  C: 
	// dirname : 末尾のスラッシュを含むディレクトリパス
	// fname   : 拡張子なしの基本ファイル名
	// ext     : . までのファイル名

	// ファイル名の後ろにsuffixを付ける(_A, _N, _MSAO)など
	if (suffix != nullptr)
	{
		::strcat_s(fname, sizeof(fname), suffix);
	}

	// パスを結合
	char filepath[BUFFER_SIZE];
	::_makepath_s(filepath, BUFFER_SIZE, drive, dirname, fname, ext);

	// マルチバイト文字からワイド文字へ変換
	wchar_t wfilename[256];
	::MultiByteToWideChar(CP_ACP, 0, filepath, -1, wfilename, 256);

	// DDSテクスチャ読み込み
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(device, wfilename, resource.GetAddressOf(), srv);
	
	// テクスチャ読み込み
	if (FAILED(hr)) {
		hr = DirectX::CreateWICTextureFromFile(device, wfilename, resource.GetAddressOf(), srv);
		if (FAILED(hr))
		{
			// WICでサポートされていないフォーマットの場合（TGAなど）は
			// STBで画像読み込みをしてテクスチャを生成する
			int width, height, bpp;
			unsigned char* pixels = stbi_load(filepath, &width, &height, &bpp, STBI_rgb_alpha);
			if (pixels != nullptr)
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = width;
				desc.Height = height;
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
				data.SysMemPitch = width * 4;

				Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
				HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv);
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// 後始末
				stbi_image_free(pixels);
			}
			else
			{
				// 読み込み失敗したらダミーテクスチャを作る
				LOG("load failed : %s\n", filepath);

				const int width = 8;
				const int height = 8;
				UINT pixels[width * height];
				for (int yy = 0; yy < height; ++yy)
				{
					for (int xx = 0; xx < width; ++xx)
					{
						pixels[yy * width + xx] = dummyColor;
					}
				}

				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = width;
				desc.Height = height;
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
				data.SysMemPitch = width;

				Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
				HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv);
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}
		}
	}
}

//// シリアライズ
//void ModelResource::Serialize(const char* filename)
//{
//	std::ofstream ostream(filename, std::ios::binary);
//	if (ostream.is_open())
//	{
//		cereal::BinaryOutputArchive archive(ostream);
//
//		try
//		{
//			archive(
//				CEREAL_NVP(nodes),
//				CEREAL_NVP(materials),
//				CEREAL_NVP(meshes)
//			);
//		}
//		catch (...)
//		{
//			LOG("model deserialize failed.\n%s\n", filename);
//			return;
//		}
//	}
//}

// デシリアライズ
void ModelResource::Deserialize(const char* filename)
{
	std::ifstream istream(filename, std::ios::binary);
	if (istream.is_open())
	{
		cereal::BinaryInputArchive archive(istream);

		try
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materials),
				CEREAL_NVP(meshes)
			);
		}
		catch (...)
		{
			LOG("model deserialize failed.\n%s\n", filename);
			return;
		}
	}
	else
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "File not found > %s", filename);
		_ASSERT_EXPR_A(false, buffer);
	}
}

void ModelResource::DeserializeAnimation(const char* filename)
{
	std::ifstream istream(filename, std::ios::binary);
	if (istream.is_open())
	{
		cereal::BinaryInputArchive archive(istream);

		try
		{
			archive(
				CEREAL_NVP(animations)
			);
		}
		catch (...)
		{
			LOG("model deserialize failed.\n%s\n", filename);
			return;
		}
	}
	else
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "File not found > %s", filename);
		_ASSERT_EXPR_A(false, buffer);
	}
}

// ノードインデックスを取得する
int ModelResource::FindNodeIndex(NodeId nodeId) const
{
	int nodeCount = static_cast<int>(nodes.size());
	for (int i = 0; i < nodeCount; ++i)
	{
		if (nodes[i].id == nodeId)
		{
			return i;
		}
	}
	return -1;
}
