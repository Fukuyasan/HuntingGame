#pragma once
#include "AI/NavMesh/NavMesh.h"

class TileMesh : public NavMesh
{
public:
	TileMesh(Model* model);
	~TileMesh();

	void BuildMesh(Model* model) override;

private:
	void BuildAllTiles();

	unsigned char* buildTileMeshData(
		const int tx, const int ty,
		const DirectX::XMFLOAT3& bmin, const DirectX::XMFLOAT3& bmax,
		int& dataSize
	);
};
