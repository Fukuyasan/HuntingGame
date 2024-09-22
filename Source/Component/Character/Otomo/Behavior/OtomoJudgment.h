#pragma once
#include "AI/BehaviorTree/JudgmentBase.h"
#include "Component/Character/Otomo/Otomo.h"

class J_OtomoCondition : public JudgmentBase<Otomo>
{
public:
	J_OtomoCondition(Otomo* otomo, const int condition) : JudgmentBase<Otomo>(otomo), condition(condition) {}
	// ”»’è
	bool Judgment() override;

private:
	int condition = 0;
};

class J_SkillGauge : public JudgmentBase<Otomo>
{
public:
	J_SkillGauge(Otomo* otomo) : JudgmentBase<Otomo>(otomo) {}
	// ”»’è
	bool Judgment() override;
};

class J_OtomoWander : public JudgmentBase<Otomo>
{
public:
	J_OtomoWander(Otomo* otomo, const float range) : JudgmentBase<Otomo>(otomo), range(range) {}
	// ”»’è
	bool Judgment() override;

private:
	float range;
};