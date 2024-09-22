#include "AI/BehaviorTree/JudgmentBase.h"
#include "Component/Character/Enemy/Wyvern/Wyvern.h"

#ifdef DEBUG



// ó‘Ô‚É‚æ‚Á‚Ä‘JˆÚ
class J_WyvernCondition : public JudgmentBase<Wyvern>
{
public:
	J_WyvernCondition(Wyvern* wyvern, const unsigned int condition) : JudgmentBase<Wyvern>(wyvern), condition(condition) {}
	bool Judgment() override;

private:
	unsigned int condition = 0;
};

// ‹——£”»’è
class J_WyvernWander : public JudgmentBase<Wyvern>
{
public:
	J_WyvernWander(Wyvern* wyvern, const float range) : JudgmentBase<Wyvern>(wyvern), range(range) {};
	// ”»’è
	bool Judgment() override;

private:
	float range = 0.0f;
};
#endif // DEBUG
