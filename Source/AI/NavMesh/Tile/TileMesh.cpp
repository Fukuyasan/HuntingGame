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

	// ���f�����璸�_�𒊏o
	DetectedModel(model);

	// �i�r���b�V���̊��蓖��
	navMesh = dtAllocNavMesh();
	_ASSERT_EXPR_A(navMesh, "Could not create Detour navmesh");

	int tileBits = 14;
	int polyBits = 22 - tileBits;

	// �i�r���b�V���̃p�����[�^�ݒ�
	dtNavMeshParams params;
	rcVcopy(params.orig, &boundMin.x);
	params.tileWidth = TILESIZE * cellSize;
	params.tileHeight = TILESIZE * cellSize;
	params.maxTiles = 1 << tileBits;
	params.maxPolys = 1 << polyBits;

	dtStatus status;

	// �i�r���b�V��������
	status = navMesh->init(&params);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh");

	// �i�r���b�V���N�G�����蓖��
	navMeshQuery = dtAllocNavMeshQuery();

	// �i�r���b�V���N�G���̐���
	status = navMeshQuery->init(navMesh, 2048);
	_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh query");

	BuildAllTiles();
}

void TileMesh::BuildAllTiles()
{
	// �O���b�h�T�C�Y�̐ݒ�
	int gridWigth = 0, gridHeight = 0;
	rcCalcGridSize(&boundMin.x, &boundMax.x, cellSize, &gridWigth, &gridHeight);

	// �^�C���T�C�Y�̐ݒ�
	const int   tileWigth = (gridWigth + TILESIZE - 1) / TILESIZE;
	const int   tileHeight = (gridHeight + TILESIZE - 1) / TILESIZE;
	const float tilecs = TILESIZE * cellSize;

	// �i�r���b�V���Ƀ^�C����ǉ�
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

			// �ߋ��̃f�[�^���폜����
			navMesh->removeTile(navMesh->getTileRefAt(x, y, 0), 0, 0);
			dtStatus status = navMesh->addTile(data, dataSize, DT_TILE_FREE_DATA, 0, 0);

			if (dtStatusFailed(status))
				dtFree(data);
		}
	}
}

unsigned char* TileMesh::buildTileMeshData(const int tx, const int ty, const DirectX::XMFLOAT3& bmin, const DirectX::XMFLOAT3& bmax, int& dataSize)
{
	// �O�p�`�̗̈�^�C�v��ێ��ł���z����m�ۂ���B
	// �������郁�b�V������������ꍇ��
	// ��������K�v�̂���O�p�`�̍ő吔���m�ۂ���K�v������B
	int triCount = static_cast<int>(indices.size() / 3);
	std::unique_ptr<u_char[]> triAreas = std::make_unique<u_char[]>(triCount);
	_ASSERT_EXPR_A(triAreas, "buildNavigation: Out of memory 'm_triareas");

	// ���z������s�\�ȎO�p�`�������A���X�^���C�Y����B
	// ���̓f�[�^�������̃��b�V���̏ꍇ�A�����Ń��b�V����ϊ����A
	// ���ꂼ��̃��b�V����are�^�C�v���v�Z���A���X�^���C�Y���邱�Ƃ��ł���B
	memset(triAreas.get(), 0, triCount * sizeof(u_char));
	float* verts = &vertices.at(0).x;
	int    vertCount = static_cast<int>(vertices.size());
	int* tris = indices.data();

	// �R���t�B�O�ݒ�
	memset(&config, 0, sizeof(config));
	// �Z���̃T�C�Y�A����
	config.cs = cellSize;
	config.ch = cellHeight;

	// �o���ő�X��
	config.walkableSlopeAngle = agentMaxSlope;
	// �ړ��ł��鍂��
	config.walkableHeight = (int)ceilf(agentHeight / config.ch);
	// �o��鍂��?
	config.walkableClimb = (int)floorf(agentMaxClimb / config.ch);
	// 
	config.walkableRadius = (int)ceilf(agentRadius / config.cs);
	// ���b�V���̋��E�ɉ������֊s�G�b�W�̍ő勖�e���B
	config.maxEdgeLen = (int)(edgeMaxLen / cellSize);
	// �ȗ������ꂽ�֊s�̋��E���A���̐��̗֊s���炸���ő勗���B
	config.maxSimplificationError = edgeMaxError;
	// 
	config.minRegionArea = (int)rcSqr(regionMinSize);
	//
	config.mergeRegionArea = (int)rcSqr(regionMergeSize);
	// 
	config.maxVertsPerPoly = (int)vertsPerPoly;
	// �^�C���̃T�C�Y
	config.tileSize = TILESIZE;
	config.borderSize = config.walkableRadius + 3;
	// 
	config.width = config.tileSize + config.borderSize * 2;
	//
	config.height = config.tileSize + config.borderSize * 2;
	// �ڍ׃��b�V���𐶐�����ۂɎg�p����T���v�����O����
	config.detailSampleDist = detailSampleDist < 0.9f ? 0 : cellSize * detailSampleDist;
	// �ڍ׃��b�V���̃T�[�t�F�X��heightfield�����E����ő勗��
	config.detailSampleMaxError = cellHeight * detailSampleMaxError;

	// �i�r���b�V�����\�z����G���A�̐ݒ�B
	// �����ł͓��̓��b�V���̋��E���g�p����Ă��邪�A�ʐς̓��[�U�[��`�̃{�b�N�X�ȂǂŎw�肷�邱�Ƃ��ł���B
	// �̈�̓��[�U�[��`�{�b�N�X�ȂǂŎw�肷�邱�Ƃ��ł���B
	rcVcopy(config.bmin, &boundMin.x);
	rcVcopy(config.bmax, &boundMax.x);
	config.bmin[0] -= config.borderSize * config.cs;
	config.bmin[2] -= config.borderSize * config.cs;
	config.bmax[0] += config.borderSize * config.cs;
	config.bmax[2] += config.borderSize * config.cs;

	bool result;
	// �n�C�g�t�B�[���h�̊��蓖��
	// �߂�l : ���������ꂽ heightField�A���s���� null
	heightField = rcAllocHeightfield();
	_ASSERT_EXPR_A(heightField, "buildNavigation: Out of memory 'solid'.");

	// �n�C�g�t�B�[���h�̐���
	result = rcCreateHeightfield(&context, *heightField, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not create solid heightfield.");

	rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, verts, vertCount, indices.data(), triCount, triAreas.get());
	// �C���f�b�N�X�t���O�p�`���b�V�����w�肳�ꂽ�n�C�g�t�B�[���h�Ƀ��X�^���C�Y���܂��B : �d��
	result = rcRasterizeTriangles(&context, verts, vertCount, tris, triAreas.get(), triCount, *heightField, config.walkableClimb);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not rasterize triangles.");

	// ���s�\���ǂ������l����(�킩���)
	rcFilterLowHangingWalkableObstacles(&context, config.walkableClimb, *heightField);
	rcFilterLedgeSpans(&context, config.walkableHeight, config.walkableClimb, *heightField);
	rcFilterWalkableLowHeightSpans(&context, config.walkableHeight, *heightField);

	// �ÓI�n�C�g�t�B�[���h�̊��蓖��
	compactHeightField = rcAllocCompactHeightfield();
	_ASSERT_EXPR_A(compactHeightField, "buildNavigation: Out of memory 'chf'.");

	// �ÓI�n�C�g�t�B�[���h�̐���
	result = rcBuildCompactHeightfield(&context, config.walkableHeight, config.walkableClimb, *heightField, *compactHeightField);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build compact data.");

	// ���s�\�ȏꏊ�̐���(����)
	result = rcErodeWalkableArea(&context, config.walkableRadius, *compactHeightField);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not erode.");

	// ���s�\�ȕ\�ʂɉ����ċ����t�B�[���h���v�Z���邱�Ƃɂ���āA�̈敪���̏���������B
	result = rcBuildDistanceField(&context, *compactHeightField);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build distance field.");

	// ���s�\�ȕ\�ʂ����̂Ȃ��P���ȗ̈�ɕ�������B
	result = rcBuildRegions(&context, *compactHeightField, 0, config.minRegionArea, config.mergeRegionArea);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build watershed regions.");

	// �i�r���b�V���̗֊s�����蓖��
	contourSet = rcAllocContourSet();
	_ASSERT_EXPR_A(contourSet, "buildNavigation: Out of memory 'cset'.");

	// �֊s�̐���
	result = rcBuildContours(&context, *compactHeightField, config.maxSimplificationError, config.maxEdgeLen, *contourSet);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not create contours.");

	// �|���S�����b�V���̊��蓖��
	polyMesh = rcAllocPolyMesh();
	_ASSERT_EXPR_A(polyMesh, "buildNavigation: Out of memory 'pmesh'.");

	// �|���S�����b�V���̐���
	result = rcBuildPolyMesh(&context, *contourSet, config.maxVertsPerPoly, *polyMesh);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not triangulate contours.");

	// �ڍ׃|���S�����b�V���̊��蓖��
	polyMeshDetail = rcAllocPolyMeshDetail();
	_ASSERT_EXPR_A(polyMeshDetail, "buildNavigation: Out of memory 'pmdtl'.");

	// TODO : �ڍ׃|���S�����b�V���̐��� �߂�����d��
	result = rcBuildPolyMeshDetail(&context, *polyMesh, *compactHeightField, config.detailSampleDist, config.detailSampleMaxError, *polyMeshDetail);
	_ASSERT_EXPR_A(result, "buildNavigation: Could not build detail mesh.");


	// �|���S���̐����[�v
	// �n�`�̑����ɂ���ăt���O��ύX����i����͑S�ĕ�����悤�Ɂj
	for (int i = 0; i < polyMesh->npolys; ++i)
	{
		// �|���S�����b�V���̃G���A�̔ԍ������s�\�o�Ȃ��ꍇ continue
		if (polyMesh->areas[i] != RC_WALKABLE_AREA) continue;

		polyMesh->flags[i] = Walk;
	}

	// �i�r���b�V���̃p�����[�^�ݒ�
	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));

	params.verts            = polyMesh->verts;		   // ���_
	params.vertCount        = polyMesh->nverts;		   // ���_�ԍ�
	params.polys            = polyMesh->polys;		   // �|���S��
	params.polyAreas        = polyMesh->areas;		   // �|���S���̃G���AID
	params.polyFlags        = polyMesh->flags;		   // �|���S���̃t���O
	params.polyCount        = polyMesh->npolys;        // �|���S���ԍ�
	params.nvp              = polyMesh->nvp;           // �|���S��������̒��_�̍ő吔
	params.detailMeshes     = polyMeshDetail->meshes;  // �T�u���b�V���f�[�^
	params.detailVerts      = polyMeshDetail->verts;   // ���b�V���̒��_
	params.detailVertsCount = polyMeshDetail->nverts;  // ���_�ԍ�
	params.detailTris       = polyMeshDetail->tris;    // ���b�V���̎O�p
	params.detailTriCount   = polyMeshDetail->ntris;   // �O�p�ԍ�
	params.offMeshConVerts  = nullptr;				   // �I�t���b�V���̐ڑ����_
	params.offMeshConRad    = nullptr;				   // �I�t���b�V���̐ڑ����a
	params.offMeshConDir    = nullptr;				   // 
	params.offMeshConAreas  = nullptr;				   // �I�t���b�V���ڑ��Ɋ��蓖�Ă�ꂽ���[�U�[��`�̃G���AID
	params.offMeshConFlags  = nullptr;				   // �I�t���b�V���ڑ��Ɋ��蓖�Ă�ꂽ���[�U�[��`�̃t���O
	params.offMeshConUserID = nullptr;
	params.offMeshConCount  = 0;
	params.walkableHeight   = agentHeight;			   // �G�[�W�F���g�̍���
	params.walkableRadius   = agentRadius;			   // �G�[�W�F���g�̔��a
	params.walkableClimb    = agentMaxClimb;		   // �G�[�W�F���g�̍ő�g���o�[�X�\���b�W
	params.tileX            = tx;
	params.tileY            = ty;
	params.tileLayer        = 0;

	// �o�E���f�B���O�{�b�N�X
	rcVcopy(params.bmin, polyMesh->bmin);
	rcVcopy(params.bmax, polyMesh->bmax);
	params.cs = config.cs;			   // XZ���ʂ̃Z���T�C�Y
	params.ch = config.ch;			   // �Z���̍���
	params.buildBvTree = true;					   // �^�C���ɑ΂��ăo�E���f�B���O�{�����[���c���[���\�z����

	unsigned char* navData = 0;
	int navDataSize = 0;

	// �i�r���b�V���̐���
	result = dtCreateNavMeshData(&params, &navData, &navDataSize);
	_ASSERT_EXPR_A(result, "Could not build Detour navmesh");

	return navData;
}