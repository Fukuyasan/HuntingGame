#pragma once
#include "AI/BehaviorTree/JudgmentBase.h"
#include "Component/Character/Enemy/Dragon/Dragon.h"

class J_DragonFar : public JudgmentBase<Dragon>
{
public:
	J_DragonFar(Dragon* enemy, const float _range) : JudgmentBase<Dragon>(enemy), range(_range) {};
	// ����
	bool Judgment()override;

private:
	float range = 0.0f;
};

// ��������
class J_DragonWander : public JudgmentBase<Dragon>
{
public:
	J_DragonWander(Dragon* enemy, const float _range) : JudgmentBase<Dragon>(enemy), range(_range) {};
	// ����
	bool Judgment()override;

private:
	float range = 0.0f;
};

// �U������s���ɑJ�ڂł��邩����
class J_DragonTurn : public JudgmentBase<Dragon>
{
public:
	J_DragonTurn(Dragon* enemy) : JudgmentBase<Dragon>(enemy) {};
	// ����
	bool Judgment()override;
};

// �{���ԂɑJ�ڂł��邩����
class J_DragonAngry : public JudgmentBase<Dragon>
{
public:
	J_DragonAngry(Dragon* enemy) : JudgmentBase<Dragon>(enemy) {};
	// ����
	bool Judgment()override;
private:
	// ��x�ʂ�Ǝ��͒ʂ��Ȃ�
	bool passible = false;
};

// �̗͂����ȉ��Ȃ�J�ڂł���
class J_DragonHealth : public JudgmentBase<Dragon>
{
public:
	J_DragonHealth(Dragon* enemy, float health) : JudgmentBase<Dragon>(enemy), healthPercent(health){};
	// ����
	bool Judgment()override;
private:
	// �̗͂̊���
	float healthPercent = 0;
};

// ��Ԃɂ���đJ�ڂł��邩
class J_DragonCondition : public JudgmentBase<Dragon>
{
public:
	J_DragonCondition(Dragon* enemy, const int condition) : JudgmentBase<Dragon>(enemy), condition(condition) {};
	bool Judgment() override;
private:
	int condition = 0;
};