#pragma once
#include "AI/BehaviorTree/ActionBase.h"
#include "Component/Character/Otomo/Otomo.h"

class A_OtomoKnockDown : public ActionBase<Otomo>
{
public:
	A_OtomoKnockDown(Otomo* otomo) : ActionBase<Otomo>(otomo) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime) override;
};

class A_OtomoGetUp : public ActionBase<Otomo>
{
public:
	A_OtomoGetUp(Otomo* otomo) : ActionBase<Otomo>(otomo) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime) override;
};

class A_OtomoWander : public ActionBase<Otomo>
{
public:
	A_OtomoWander(Otomo* otomo) : ActionBase<Otomo>(otomo) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime) override;
};

class A_OtomoIdle : public ActionBase<Otomo>
{
public:
	A_OtomoIdle(Otomo* otomo) : ActionBase<Otomo>(otomo) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime) override;
};

class A_OtomoAttack : public ActionBase<Otomo>
{
public:
	A_OtomoAttack(Otomo* otomo) : ActionBase<Otomo>(otomo) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime) override;
};

class A_OtomoSkill_A : public ActionBase<Otomo>
{
public:
	A_OtomoSkill_A(Otomo* otomo) : ActionBase<Otomo>(otomo) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime) override;
};

class A_OtomoSkill_B : public ActionBase<Otomo>
{
public:
	A_OtomoSkill_B(Otomo* otomo) : ActionBase<Otomo>(otomo) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime) override;
};

