#pragma once
#include "Graphics/Model/Model.h"

#include <DetourNavMesh.h>
#include <Recast.h>

class NavMesh
{
public:
	// �i�r���b�V����bin�t�@�C���ɕۑ����邽�߂̍\����
	struct NavmeshSetHeader
	{
		int magic;
		int version;
		int numTiles;
		dtNavMeshParams params;
	};

	// �^�C�����b�V���̏���bin�t�@�C���ɕۑ����邽�߂̍\����
	struct NavMeshTileHeader
	{
		dtTileRef tileRef;
		int dataSize;
	};

	// �|���S���̃t���O
	enum PolyFlags
	{
		Walk = (1 << 0)
	};

public:
	NavMesh() {}
	virtual ~NavMesh() {}

	// �r���h����
	virtual void BuildMesh(Model* model) = 0;

	// ���f�����璸�_�𒊏o
	virtual void DetectedModel(Model* stage);

	// �N���[������
	void Clean();

	// NavMesh�̃Z�[�u�A���[�h
	virtual void SaveNavmesh(const char* filename, const dtNavMesh* mesh);
	virtual dtNavMesh* LoadNavmesh(const char* filename);

public:
	// �i�r���b�V���擾
	dtNavMesh* GetNavMesh()const { return navMesh; }

	// �i�r���b�V���N�G���擾
	dtNavMeshQuery* GetNavMeshQuery()const { return navMeshQuery; }

	// �ڍ׃|���S�����b�V���擾
	rcPolyMeshDetail* GetPolyMeshQuery()const { return polyMeshDetail; }

	// GUI
	void DrawGUI(/*Model* model*/);

protected:
	// �i�r���b�V��
	dtNavMesh* navMesh = nullptr;
	// �i�r���b�V���N�G��
	dtNavMeshQuery* navMeshQuery = nullptr;
	// �|���S�����b�V��
	rcPolyMesh* polyMesh = nullptr;
	// �ڍ׃|���S�����b�V��
	rcPolyMeshDetail* polyMeshDetail = nullptr;
	// 
	rcContourSet* contourSet = nullptr;
	// �R���p�N�g�ȐÓI�n�C�g�t�B�[���h
	rcCompactHeightfield* compactHeightField = nullptr;
	// �_�C�i�~�b�N�ȃn�C�g�t�B�[���h
	rcHeightfield* heightField = nullptr;
	// �R���t�B�O
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

	// ���f�����璸�_�𒊏o
	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<int> indices;

	// �o�E���f�B���O�{�b�N�X
	DirectX::XMFLOAT3 boundMin = {}, boundMax = {};
};