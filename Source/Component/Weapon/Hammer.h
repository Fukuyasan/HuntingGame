#pragma once
#include "Component/Component.h"

class Hammer : public Component
{
public:
	Hammer() = default;
	~Hammer() override = default;

	void Start() override;
	void Update() override;

	const char* GetName() const override { return "Hammer"; }

	// トランスフォーム計算
	void ComputeTransform();

};