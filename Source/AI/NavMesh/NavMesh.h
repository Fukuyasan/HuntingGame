#pragma once
#include "Graphics/Model/Model.h"

#include <DetourNavMesh.h>
#include <Recast.h>

class NavMesh
{
public:
	// ナビメッシュをbinファイルに保存するための構造体
	struct NavmeshSetHeader
	{
		int magic;
		int version;
		int numTiles;
		dtNavMeshParams params;
	};

	// タイルメッシュの情報をbinファイルに保存するための構造体
	struct NavMeshTileHeader
	{
		dtTileRef tileRef;
		int dataSize;
	};

	// ポリゴンのフラグ
	enum PolyFlags
	{
		Walk = (1 << 0)
	};

public:
	NavMesh() {}
	virtual ~NavMesh() {}

	// ビルド処理
	virtual void BuildMesh(Model* model) = 0;

	// モデルから頂点を抽出
	virtual void DetectedModel(Model* stage);

	// クリーン処理
	void Clean();

	// NavMeshのセーブ、ロード
	virtual void SaveNavmesh(const char* filename, const dtNavMesh* mesh);
	virtual dtNavMesh* LoadNavmesh(const char* filename);

public:
	// ナビメッシュ取得
	dtNavMesh* GetNavMesh()const { return navMesh; }

	// ナビメッシュクエリ取得
	dtNavMeshQuery* GetNavMeshQuery()const { return navMeshQuery; }

	// 詳細ポリゴンメッシュ取得
	rcPolyMeshDetail* GetPolyMeshQuery()const { return polyMeshDetail; }

	// GUI
	void DrawGUI(/*Model* model*/);

protected:
	// ナビメッシュ
	dtNavMesh* navMesh = nullptr;
	// ナビメッシュクエリ
	dtNavMeshQuery* navMeshQuery = nullptr;
	// ポリゴンメッシュ
	rcPolyMesh* polyMesh = nullptr;
	// 詳細ポリゴンメッシュ
	rcPolyMeshDetail* polyMeshDetail = nullptr;
	// 
	rcContourSet* contourSet = nullptr;
	// コンパクトな静的ハイトフィールド
	rcCompactHeightfield* compactHeightField = nullptr;
	// ダイナミックなハイトフィールド
	rcHeightfield* heightField = nullptr;
	// コンフィグ
	rcConfig config;
	//
	rcContext context;

	const int NAVMESHSSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T';
	const int NAVMESHSSET_VERTION = 1;

protected:
	float cellSize             = 0.3f;
	float cellHeight           = 0.2f;
	float agentHeight          = 2.0f;
	float agentRadius          = 0.6f;
	float agentMaxClimb        = 1.0f;
	float agentMaxSlope        = 45.0f;
	float regionMinSize        = 8;
	float regionMergeSize      = 20;
	float edgeMaxLen           = 12.0f;
	float edgeMaxError         = 1.3f;
	float vertsPerPoly         = 6.0f;
	float detailSampleDist     = 6.0f;
	float detailSampleMaxError = 1.0f;

	// モデルから頂点を抽出
	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<int> indices;

	// バウンディングボックス
	DirectX::XMFLOAT3 boundMin = {}, boundMax = {};
};