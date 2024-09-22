#pragma once
#include "Graphics/Shader.h"
#include "Graphics/Shader/SpriteShader/TrailShader.h"

#include "Component/Component.h"

class Sword : public Component
{
public:
	enum class SWOED_NODE
	{
		APEX,  // ��[
		ROOT   // ���{
	};

public:
	Sword() = default;
	~Sword() override = default;

	void Start() override;
	void Update() override;

	const char* GetName() const override { return "Sword"; }

	void TrailRender();

	// �g�����X�t�H�[���v�Z
	void ComputeTransform();

	void OnDebugGUI() override;
	void OnDebugPrimitive() override;

public:
	const float& GetRadius() { return radius; }

	// ���̃m�[�h�擾
	const int& GetSwordNode(SWOED_NODE node) { return swordNodeIndex[static_cast<int>(node)]; }

	// �g���C���擾
	TrailShader* GetTrail() { return trail.get(); }

	// �g���C���̏o��
	void SetTrail(bool _trail) { this->isTrail = _trail; }
	void SetTrail();

	// �{��
	float GetMagnification() { return this->swordMagnification;	}
	float GetCorrection()    { return this->swordCorrection;	}

private:
	float radius = 0.5f;

	DirectX::XMFLOAT4X4 transform = 
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	DirectX::XMFLOAT3 scale     = {1, 1, 1};
	DirectX::XMFLOAT3 angle     = {};
	DirectX::XMFLOAT3 translate = { 10, 5, 5 };

	int swordNodeIndex[2] = {};

	std::unique_ptr<TrailShader> trail = nullptr;
	bool isTrail = false;

	float trailOffset[2] = {-30, -170};

	// ����{��
	float swordMagnification = 160.0f;

	// ����␳
	float swordCorrection = 0.8f;
};