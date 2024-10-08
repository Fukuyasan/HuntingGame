#pragma once
#include "AI/BehaviorTree/JudgmentBase.h"
#include "Component/Character/Enemy/Dragon/Dragon.h"

class J_DragonFar : public JudgmentBase<Dragon>
{
public:
	J_DragonFar(Dragon* enemy, const float _range) : JudgmentBase<Dragon>(enemy), range(_range) {};
	// 判定
	bool Judgment()override;

private:
	float range = 0.0f;
};

// 距離判定
class J_DragonWander : public JudgmentBase<Dragon>
{
public:
	J_DragonWander(Dragon* enemy, const float _range) : JudgmentBase<Dragon>(enemy), range(_range) {};
	// 判定
	bool Judgment()override;

private:
	float range = 0.0f;
};

// 振り向き行動に遷移できるか判定
class J_DragonTurn : public JudgmentBase<Dragon>
{
public:
	J_DragonTurn(Dragon* enemy) : JudgmentBase<Dragon>(enemy) {};
	// 判定
	bool Judgment()override;
};

// 怒り状態に遷移できるか判定
class J_DragonAngry : public JudgmentBase<Dragon>
{
public:
	J_DragonAngry(Dragon* enemy) : JudgmentBase<Dragon>(enemy) {};
	// 判定
	bool Judgment()override;
private:
	// 一度通ると次は通さない
	bool passible = false;
};

// 体力が一定以下なら遷移できる
class J_DragonHealth : public JudgmentBase<Dragon>
{
public:
	J_DragonHealth(Dragon* enemy, float health) : JudgmentBase<Dragon>(enemy), healthPercent(health){};
	// 判定
	bool Judgment()override;
private:
	// 体力の割合
	float healthPercent = 0;
};

// 状態によって遷移できるか
class J_DragonCondition : public JudgmentBase<Dragon>
{
public:
	J_DragonCondition(Dragon* enemy, const int condition) : JudgmentBase<Dragon>(enemy), condition(condition) {};
	bool Judgment() override;
private:
	int condition = 0;
};