#pragma once
#include "AI/BehaviorTree/JudgmentBase.h"
#include "Component/Character/Enemy/Dragon/Dragon.h"

class J_DragonFar : public JudgmentBase<Dragon>
{
public:
	J_DragonFar(Dragon* enemy, const float _range) : JudgmentBase<Dragon>(enemy), range(_range) {};
	// ”»’è
	bool Judgment()override;

private:
	float range = 0.0f;
};

// ‹——£”»’è
class J_DragonWander : public JudgmentBase<Dragon>
{
public:
	J_DragonWander(Dragon* enemy, const float _range) : JudgmentBase<Dragon>(enemy), range(_range) {};
	// ”»’è
	bool Judgment()override;

private:
	float range = 0.0f;
};

// U‚èŒü‚«s“®‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class J_DragonTurn : public JudgmentBase<Dragon>
{
public:
	J_DragonTurn(Dragon* enemy) : JudgmentBase<Dragon>(enemy) {};
	// ”»’è
	bool Judgment()override;
};

// “{‚èó‘Ô‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class J_DragonAngry : public JudgmentBase<Dragon>
{
public:
	J_DragonAngry(Dragon* enemy) : JudgmentBase<Dragon>(enemy) {};
	// ”»’è
	bool Judgment()override;
private:
	// ˆê“x’Ê‚é‚ÆŸ‚Í’Ê‚³‚È‚¢
	bool passible = false;
};

// ‘Ì—Í‚ªˆê’èˆÈ‰º‚È‚ç‘JˆÚ‚Å‚«‚é
class J_DragonHealth : public JudgmentBase<Dragon>
{
public:
	J_DragonHealth(Dragon* enemy, float health) : JudgmentBase<Dragon>(enemy), healthPercent(health){};
	// ”»’è
	bool Judgment()override;
private:
	// ‘Ì—Í‚ÌŠ„‡
	float healthPercent = 0;
};

// ó‘Ô‚É‚æ‚Á‚Ä‘JˆÚ‚Å‚«‚é‚©
class J_DragonCondition : public JudgmentBase<Dragon>
{
public:
	J_DragonCondition(Dragon* enemy, const int condition) : JudgmentBase<Dragon>(enemy), condition(condition) {};
	bool Judgment() override;
private:
	int condition = 0;
};