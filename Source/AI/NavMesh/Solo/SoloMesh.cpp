#include "SoloMesh.h"
#include <map>
#include <DetourNavMeshBuilder.h>
#include "Graphics/Graphics.h" 
#include "System/Misc.h" 

SoloMesh::SoloMesh(Model* stage)
{
	BuildMesh(stage);
}

SoloMesh::SoloMesh(const char* filepath)
{
	dtFreeNavMesh(navMesh);
	navMesh = LoadNavmesh(filepath);

	// ナビメッシュクエリ割り当て
	navMeshQuery = dtAllocNavMeshQuery();
	navMeshQuery->init(navMesh, 2048);
}

SoloMesh::~SoloMesh()
{
	Clean();
}

void SoloMesh::BuildMesh(Model* model)
{
	Clean();

	// モデルから頂点を抽出
	DetectedModel(model);

	// ナビメッシュ
	bool result;

	// コンフィグ設定
	memset(&config, 0, sizeof(config));
	// セルのサイズ, 高さ
	config.cs = cellSize;
	config.ch = cellHeight;
	// Agentが移動できる範囲
	config.walkableSlopeAngle = agentMaxSlope;
	config.walkableHeight = (int)ceilf(agentHeight / config.ch);
	config.walkableClimb = (int)floorf(agentMaxClimb / config.ch);
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
	// 詳細メッシュを生成する際に使用するサンプリング距離
	config.detailSampleDist = detailSampleDist < 0.9f ? 0 : cellSize * detailSampleDist;
	// 詳細メッシュのサーフェスがheightfieldから逸脱する最大距離
	config.detailSampleMaxError = cellHeight * detailSampleMaxError;

	// ナビメッシュを構築するエリアの設定。
	// ここでは入力メッシュの境界が使用されているが、面積はユーザー定義のボックスなどで指定することもできる。
	// 領域はユーザー定義ボックスなどで指定することもできる。
	rcVcopy(config.bmin, &boundMin.x);
	rcVcopy(config.bmax, &boundMax.x);
	rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

	// ハイトフィールドの割り当て
	// 戻り値 : 初期化された heightField、失敗時は null
	heightField = rcAllocHeightfield();
	_ASSERT_EXPR_A(heightField, "buildNavigation: Out of memory 'solid'.");

	// ハイトフィールドの生成
	result = rcCreateHeightfield(&context, *heightField, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not create solid heightfield.");

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
	int* tris = indices.data();
	int vertCount = static_cast<int>(vertices.size());

	rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, verts, vertCount, indices.data(), triCount, triAreas.get());
	// インデックス付き三角形メッシュを指定されたハイトフィールドにラスタライズします。 : 重い
	result = rcRasterizeTriangles(&context, verts, vertCount, tris, triAreas.get(), triCount, *heightField, config.walkableClimb);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not rasterize triangles.");

	// 歩きやすい路面をフィルタリングする。
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

	//  歩行可能な表面を穴のない単純な領域に分割する。
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
	// バウンディングボックス
	rcVcopy(params.bmin, polyMesh->bmin);
	rcVcopy(params.bmax, polyMesh->bmax);
	params.cs               = config.cs;			   // XZ平面のセルサイズ
	params.ch               = config.ch;			   // セルの高さ
	params.buildBvTree      = true;					   // タイルに対してバウンディングボリュームツリーを構築する

	unsigned char* navData = 0;
	int navDataSize = 0;

	// ナビメッシュの生成
	result = dtCreateNavMeshData(&params, &navData, &navDataSize);
	_ASSERT_EXPR_A(result, "Could not build Detour navmesh");

	// ナビメッシュの割り当て
	navMesh = dtAllocNavMesh();
	_ASSERT_EXPR_A(navMesh, "Could not create Detour navmesh");

	dtStatus status;

	// ナビメッシュ初期化
	status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh");

	// ナビメッシュクエリ割り当て
	navMeshQuery = dtAllocNavMeshQuery();

	// ナビメッシュクエリの生成
	status = navMeshQuery->init(navMesh, 2048);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh query");
}