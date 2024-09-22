#pragma once
#include "Component/Character/Character.h"
#include "Behavior/OtomoBehaviorTree.h"

class Otomo : public Character
{
public:
    enum Condition
    {
        C_Battle	  = (1 << 1),  // 戦闘
        C_KnockDown   = (1 << 4),  // 怯み
        C_ALL         = C_Battle | C_KnockDown
    };

	enum class Animation
	{
		Idle,
		Run,
		KnockDown,
		GetUp,
		Attack_A,
		Skill_A,
		Skill_B
	};

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	const char* GetName() const override { return "Otomo"; }

private:
#pragma region ビヘイビアベースAI
	std::unique_ptr<OtomoBehaviorTree> otomoTree;
#pragma endregion

public:
	UINT condition = 0;
};