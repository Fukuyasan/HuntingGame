#pragma once
#include "AI/NavMesh/NavMesh.h"
#include "Component/Stage/StageMain.h"

class SoloMesh : public NavMesh
{
public:
	SoloMesh(Model* stage);
	SoloMesh(const char* filepath);
	~SoloMesh();

	void BuildMesh(Model* stage);
};
