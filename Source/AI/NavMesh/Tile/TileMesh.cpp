#include "TileMesh.h"
#include "System/Misc.h"

#include <DetourNavMeshQuery.h>
#include <DetourNavMeshBuilder.h>

#define TILESIZE 32

TileMesh::TileMesh(Model* model)
{
	BuildMesh(model);
}

TileMesh::~TileMesh()
{
	Clean();
}

void TileMesh::BuildMesh(Model* model)
{
	Clean();

	// モデルから頂点を抽出
	DetectedModel(model);

	// ナビメッシュの割り当て
	navMesh = dtAllocNavMesh();
	_ASSERT_EXPR_A(navMesh, "Could not create Detour navmesh");

	int tileBits = 14;
	int polyBits = 22 - tileBits;

	// ナビメッシュのパラメータ設定
	dtNavMeshParams params;
	rcVcopy(params.orig, &boundMin.x);
	params.tileWidth = TILESIZE * cellSize;
	params.tileHeight = TILESIZE * cellSize;
	params.maxTiles = 1 << tileBits;
	params.maxPolys = 1 << polyBits;

	dtStatus status;

	// ナビメッシュ初期化
	status = navMesh->init(&params);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh");

	// ナビメッシュクエリ割り当て
	navMeshQuery = dtAllocNavMeshQuery();

	// ナビメッシュクエリの生成
	status = navMeshQuery->init(navMesh, 2048);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh query");

	BuildAllTiles();
}

void TileMesh::BuildAllTiles()
{
	// グリッドサイズの設定
	int gridWigth = 0, gridHeight = 0;
	rcCalcGridSize(&boundMin.x, &boundMax.x, cellSize, &gridWigth, &gridHeight);

	// タイルサイズの設定
	const int   tileWigth = (gridWigth + TILESIZE - 1) / TILESIZE;
	const int   tileHeight = (gridHeight + TILESIZE - 1) / TILESIZE;
	const float tilecs = TILESIZE * cellSize;

	// ナビメッシュにタイルを追加
	DirectX::XMFLOAT3 lastBuildTileMin, lastBuildTileMax;
	for (int y = 0; y < tileHeight; ++y)
	{
		for (int x = 0; x < tileWigth; ++x)
		{
			lastBuildTileMin.x = boundMin.x + x * tilecs;
			lastBuildTileMin.y = boundMin.y;
			lastBuildTileMin.z = boundMin.z + y * tilecs;

			lastBuildTileMax.x = boundMin.x + (x + 1) * tilecs;
			lastBuildTileMax.y = boundMax.y;
			lastBuildTileMax.z = boundMin.z + (y + 1) * tilecs;

			int dataSize = 0;
			unsigned char* data = buildTileMeshData(x, y, lastBuildTileMin, lastBuildTileMax, dataSize);

			if (!data) continue;

			// 過去のデータを削除する
			navMesh->removeTile(navMesh->getTileRefAt(x, y, 0), 0, 0);
			dtStatus status = navMesh->addTile(data, dataSize, DT_TILE_FREE_DATA, 0, 0);

			if (dtStatusFailed(status))
				dtFree(data);
		}
	}
}

unsigned char* TileMesh::buildTileMeshData(const int tx, const int ty, const DirectX::XMFLOAT3& bmin, const DirectX::XMFLOAT3& bmax, int& dataSize)
{
	// 三角形の領域タイプを保持できる配列を確保する。
	// 処理するメッシュが複数ある場合は
	// 処理する必要のある三角形の最大数を確保する必要がある。
	int triCount = static_cast<int>(indices.size() / 3);
	std::unique_ptr<u_char[]> triAreas = std::make_unique<u_char[]>(triCount);
	_ASSERT_EXPR_A(triAreas, "buildNavigation: Out of memory 'm_triareas");

	// 勾配から歩行可能な三角形を見つけ、ラスタライズする。
	// 入力データが複数のメッシュの場合、ここでメッシュを変換し、
	// それぞれのメッシュのareタイプを計算し、ラスタライズすることができる。
	memset(triAreas.get(), 0, triCount * sizeof(u_char));
	float* verts = &vertices.at(0).x;
	int    vertCount = static_cast<int>(vertices.size());
	int* tris = indices.data();

	// コンフィグ設定
	memset(&config, 0, sizeof(config));
	// セルのサイズ、高さ
	config.cs = cellSize;
	config.ch = cellHeight;

	// 登れる最大傾斜
	config.walkableSlopeAngle = agentMaxSlope;
	// 移動できる高さ
	config.walkableHeight = (int)ceilf(agentHeight / config.ch);
	// 登れる高さ?
	config.walkableClimb = (int)floorf(agentMaxClimb / config.ch);
	// 
	config.walkableRadius = (int)ceilf(agentRadius / config.cs);
	// メッシュの境界に沿った輪郭エッジの最大許容長。
	config.maxEdgeLen = (int)(edgeMaxLen / cellSize);
	// 簡略化された輪郭の境界が、元の生の輪郭からずれる最大距離。
	config.maxSimplificationError = edgeMaxError;
	// 
	config.minRegionArea = (int)rcSqr(regionMinSize);
	//
	config.mergeRegionArea = (int)rcSqr(regionMergeSize);
	// 
	config.maxVertsPerPoly = (int)vertsPerPoly;
	// タイルのサイズ
	config.tileSize = TILESIZE;
	config.borderSize = config.walkableRadius + 3;
	// 
	config.width = config.tileSize + config.borderSize * 2;
	//
	config.height = config.tileSize + config.borderSize * 2;
	// 詳細メッシュを生成する際に使用するサンプリング距離
	config.detailSampleDist = detailSampleDist < 0.9f ? 0 : cellSize * detailSampleDist;
	// 詳細メッシュのサーフェスがheightfieldから逸脱する最大距離
	config.detailSampleMaxError = cellHeight * detailSampleMaxError;

	// ナビメッシュを構築するエリアの設定。
	// ここでは入力メッシュの境界が使用されているが、面積はユーザー定義のボックスなどで指定することもできる。
	// 領域はユーザー定義ボックスなどで指定することもできる。
	rcVcopy(config.bmin, &boundMin.x);
	rcVcopy(config.bmax, &boundMax.x);
	config.bmin[0] -= config.borderSize * config.cs;
	config.bmin[2] -= config.borderSize * config.cs;
	config.bmax[0] += config.borderSize * config.cs;
	config.bmax[2] += config.borderSize * config.cs;

	bool result;
	// ハイトフィールドの割り当て
	// 戻り値 : 初期化された heightField、失敗時は null
	heightField = rcAllocHeightfield();
	_ASSERT_EXPR_A(heightField, "buildNavigation: Out of memory 'solid'.");

	// ハイトフィールドの生成
	result = rcCreateHeightfield(&context, *heightField, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not create solid heightfield.");

	rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, verts, vertCount, indices.data(), triCount, triAreas.get());
	// インデックス付き三角形メッシュを指定されたハイトフィールドにラスタライズします。 : 重い
	result = rcRasterizeTriangles(&context, verts, vertCount, tris, triAreas.get(), triCount, *heightField, config.walkableClimb);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not rasterize triangles.");

	// 歩行可能かどうかを考える(わからん)
	rcFilterLowHangingWalkableObstacles(&context, config.walkableClimb, *heightField);
	rcFilterLedgeSpans(&context, config.walkableHeight, config.walkableClimb, *heightField);
	rcFilterWalkableLowHeightSpans(&context, config.walkableHeight, *heightField);

	// 静的ハイトフィールドの割り当て
	compactHeightField = rcAllocCompactHeightfield();
	_ASSERT_EXPR_A(compactHeightField, "buildNavigation: Out of memory 'chf'.");

	// 静的ハイトフィールドの生成
	result = rcBuildCompactHeightfield(&context, config.walkableHeight, config.walkableClimb, *heightField, *compactHeightField);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build compact data.");

	// 歩行可能な場所の生成(多分)
	result = rcErodeWalkableArea(&context, config.walkableRadius, *compactHeightField);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not erode.");

	// 歩行可能な表面に沿って距離フィールドを計算することによって、領域分割の準備をする。
	result = rcBuildDistanceField(&context, *compactHeightField);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build distance field.");

	// 歩行可能な表面を穴のない単純な領域に分割する。
	result = rcBuildRegions(&context, *compactHeightField, 0, config.minRegionArea, config.mergeRegionArea);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build watershed regions.");

	// ナビメッシュの輪郭を割り当て
	contourSet = rcAllocContourSet();
	_ASSERT_EXPR_A(contourSet, "buildNavigation: Out of memory 'cset'.");

	// 輪郭の生成
	result = rcBuildContours(&context, *compactHeightField, config.maxSimplificationError, config.maxEdgeLen, *contourSet);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not create contours.");

	// ポリゴンメッシュの割り当て
	polyMesh = rcAllocPolyMesh();
	_ASSERT_EXPR_A(polyMesh, "buildNavigation: Out of memory 'pmesh'.");

	// ポリゴンメッシュの生成
	result = rcBuildPolyMesh(&context, *contourSet, config.maxVertsPerPoly, *polyMesh);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not triangulate contours.");

	// 詳細ポリゴンメッシュの割り当て
	polyMeshDetail = rcAllocPolyMeshDetail();
	_ASSERT_EXPR_A(polyMeshDetail, "buildNavigation: Out of memory 'pmdtl'.");

	// TODO : 詳細ポリゴンメッシュの生成 めっちゃ重い
	result = rcBuildPolyMeshDetail(&context, *polyMesh, *compactHeightField, config.detailSampleDist, config.detailSampleMaxError, *polyMeshDetail);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build detail mesh.");


	// ポリゴンの数ループ
	// 地形の属性によってフラグを変更する（今回は全て歩けるように）
	for (int i = 0; i < polyMesh->npolys; ++i)
	{
		// ポリゴンメッシュのエリアの番号が歩行可能出ない場合 continue
		if (polyMesh->areas[i] != RC_WALKABLE_AREA) continue;

		polyMesh->flags[i] = Walk;
	}

	// ナビメッシュのパラメータ設定
	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));

	params.verts            = polyMesh->verts;		   // 頂点
	params.vertCount        = polyMesh->nverts;		   // 頂点番号
	params.polys            = polyMesh->polys;		   // ポリゴン
	params.polyAreas        = polyMesh->areas;		   // ポリゴンのエリアID
	params.polyFlags        = polyMesh->flags;		   // ポリゴンのフラグ
	params.polyCount        = polyMesh->npolys;        // ポリゴン番号
	params.nvp              = polyMesh->nvp;           // ポリゴンあたりの頂点の最大数
	params.detailMeshes     = polyMeshDetail->meshes;  // サブメッシュデータ
	params.detailVerts      = polyMeshDetail->verts;   // メッシュの頂点
	params.detailVertsCount = polyMeshDetail->nverts;  // 頂点番号
	params.detailTris       = polyMeshDetail->tris;    // メッシュの三角
	params.detailTriCount   = polyMeshDetail->ntris;   // 三角番号
	params.offMeshConVerts  = nullptr;				   // オフメッシュの接続頂点
	params.offMeshConRad    = nullptr;				   // オフメッシュの接続半径
	params.offMeshConDir    = nullptr;				   // 
	params.offMeshConAreas  = nullptr;				   // オフメッシュ接続に割り当てられたユーザー定義のエリアID
	params.offMeshConFlags  = nullptr;				   // オフメッシュ接続に割り当てられたユーザー定義のフラグ
	params.offMeshConUserID = nullptr;
	params.offMeshConCount  = 0;
	params.walkableHeight   = agentHeight;			   // エージェントの高さ
	params.walkableRadius   = agentRadius;			   // エージェントの半径
	params.walkableClimb    = agentMaxClimb;		   // エージェントの最大トラバース可能レッジ
	params.tileX            = tx;
	params.tileY            = ty;
	params.tileLayer        = 0;

	// バウンディングボックス
	rcVcopy(params.bmin, polyMesh->bmin);
	rcVcopy(params.bmax, polyMesh->bmax);
	params.cs = config.cs;			   // XZ平面のセルサイズ
	params.ch = config.ch;			   // セルの高さ
	params.buildBvTree = true;					   // タイルに対してバウンディングボリュームツリーを構築する

	unsigned char* navData = 0;
	int navDataSize = 0;

	// ナビメッシュの生成
	result = dtCreateNavMeshData(&params, &navData, &navDataSize);
	_ASSERT_EXPR_A(result, "Could not build Detour navmesh");

	return navData;
}