#pragma once

// ���s����
template<class Owner>class JudgmentBase
{
public:
	JudgmentBase(Owner* enemy) :owner(enemy) {}
	virtual ~JudgmentBase() = default;
	virtual bool Judgment() { return false; };

protected:
	Owner* owner;
};