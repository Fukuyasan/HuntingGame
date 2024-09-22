#include "AI/BehaviorTree/JudgmentBase.h"
#include "Component/Character/Enemy/Wyvern/Wyvern.h"

#ifdef DEBUG



// ��Ԃɂ���đJ��
class J_WyvernCondition : public JudgmentBase<Wyvern>
{
public:
	J_WyvernCondition(Wyvern* wyvern, const unsigned int condition) : JudgmentBase<Wyvern>(wyvern), condition(condition) {}
	bool Judgment() override;

private:
	unsigned int condition = 0;
};

// ��������
class J_WyvernWander : public JudgmentBase<Wyvern>
{
public:
	J_WyvernWander(Wyvern* wyvern, const float range) : JudgmentBase<Wyvern>(wyvern), range(range) {};
	// ����
	bool Judgment() override;

private:
	float range = 0.0f;
};
#endif // DEBUG
