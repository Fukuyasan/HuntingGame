#pragma once

#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

class ModelResource final
{
public:
	ModelResource() {}
	~ModelResource() {}

	using NodeId = UINT64;

	// モデルのボーン情報
	struct Node
	{
		NodeId				id;
		std::string			name;
		std::string			path;
		int					parentIndex;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	// マテリアル情報
	struct Material
	{
		std::string			name;

		// テクスチャ名
		std::string			textureFilename;
		std::string			normalTexture;
		std::string			metallicTexture;
		std::string			roughnessTexture;
		std::string			ambientOcclusionTexture;
		std::string			emissionTexture;

		// ※ ステージが広く、テクスチャが伸びてしまい汚く見えるので
		// UVに掛け算するための変数
		int textureNarrow;

		DirectX::XMFLOAT4	color = { 0.8f, 0.8f, 0.8f, 1.0f };

		// テクスチャをシェーダ内で使えるようにする
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoMap;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metallicMap;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessMap;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ambientOcclusionMap;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissionMap;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Subset
	{
		UINT		startIndex = 0;
		UINT		indexCount = 0;
		int			materialIndex = 0;

		Material* material = nullptr;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position   = { 0, 0, 0 };
		DirectX::XMFLOAT3	normal     = { 0, 0, 0 };
		DirectX::XMFLOAT3	tangent    = { 0, 0, 0 };
		DirectX::XMFLOAT2	texcoord   = { 0, 0 };
		DirectX::XMFLOAT4	color      = { 1, 1, 1, 1 };
		DirectX::XMFLOAT4	boneWeight = { 1, 0, 0, 0 };
		DirectX::XMUINT4	boneIndex  = { 0, 0, 0, 0 };

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Mesh
	{
		std::vector<Vertex>						vertices;
		std::vector<UINT>						indices;
		std::vector<Subset>						subsets;

		int										nodeIndex;
		std::vector<int>						nodeIndices;
		std::vector<DirectX::XMFLOAT4X4>		offsetTransforms;

		DirectX::XMFLOAT3						boundsMin;
		DirectX::XMFLOAT3						boundsMax;

		//DirectX::BoundingBox boundingBox;

		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct NodeKeyData
	{
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};
	struct Animation
	{
		std::string					name;
		float						secondsLength;
		std::vector<Keyframe>		keyframes;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	// 各種データ取得
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<Node>& GetNodes() const { return nodes; }
	const std::vector<Animation>& GetAnimations() const { return animations; }
	const std::vector<Material>& GetMaterials() const { return materials; }

	// 読み込み
	void Load(ID3D11Device* device, const char* filename);
	void LoadAnimation(const char* filename);

protected:
	// モデルセットアップ
	void BuildModel(ID3D11Device* device, const char* dirname);

	// シリアライズ
	//void Serialize(const char* filename);

	// デシリアライズ
	void Deserialize(const char* filename);
	void DeserializeAnimation(const char* filename);

	// ノードインデックスを取得する
	int FindNodeIndex(NodeId nodeId) const;

	// ステージのテクスチャ読み込み
	void LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, ID3D11ShaderResourceView** srv, UINT dummyColor = 0xFFFFFFFF);


protected:
	std::vector<Node>		nodes;
	std::vector<Material>	materials;
	std::vector<Mesh>		meshes;
	std::vector<Animation>	animations;
};
