#include "OtomoBehaviorTree.h"
#include "OtomoJudgment.h"
#include "OtomoAction.h"

OtomoBehaviorTree::~OtomoBehaviorTree()
{
	delete aiTree;
	delete behaviorData;
}

void OtomoBehaviorTree::RegisterBehaviorTree(Otomo* owner)
{
	behaviorData = new BehaviorData<Otomo>();
	aiTree = new BehaviorTree<Otomo>(owner);

	aiTree->AddNode(B_Non, B_Root, Priority::Root, SelectRule::Priority);


#pragma region ���[�g
	aiTree->AddNode(B_Root, B_KonckDown, Priority::KnockDown, SelectRule::Sequence, 0, new J_OtomoCondition(owner, Otomo::Condition::C_KnockDown));
	aiTree->AddNode(B_Root, B_Battle,    Priority::Battle,    SelectRule::Priority, 0, new J_OtomoCondition(owner, Otomo::Condition::C_Battle));
	aiTree->AddNode(B_Root, B_Scout,     Priority::Scout,     SelectRule::Priority);
#pragma endregion

#pragma region �U�����󂯂��Ƃ��̍s��
	aiTree->AddNode(B_KonckDown, B_Down,  0, SelectRule::Non, 0, nullptr, new A_OtomoKnockDown(owner));
	aiTree->AddNode(B_KonckDown, B_GetUp, 0, SelectRule::Non, 0, nullptr, new A_OtomoGetUp(owner));
#pragma endregion

#pragma region �o�g��

	aiTree->AddNode(B_Battle, B_Attack,  0, SelectRule::Probability);
	aiTree->AddNode(B_Attack, B_Basic,   0, SelectRule::Non,   50, nullptr, new A_OtomoAttack(owner));
	aiTree->AddNode(B_Attack, B_Skill_A, 0, SelectRule::Non,   35, nullptr, new A_OtomoSkill_A(owner));
	aiTree->AddNode(B_Attack, B_Skill_B, 0, SelectRule::Non,   15, nullptr, new A_OtomoSkill_B(owner));

#pragma region �p�j�ړ�
	aiTree->AddNode(B_Scout, B_Pursuit, 0, SelectRule::Non, 0, new J_OtomoWander(owner, 5.0f), new A_OtomoWander(owner));
	aiTree->AddNode(B_Scout, B_Idle,    1, SelectRule::Non, 0, nullptr, new A_OtomoIdle(owner));
#pragma endregion

	// ���荞�݃m�[�h
	aiTree->AddInterruptionNode(B_KonckDown, new J_OtomoCondition(owner, Otomo::Condition::C_KnockDown));
}

void OtomoBehaviorTree::UpdateBehaviorTree(const float elapsedTime)
{
	// ���ݎ��s����m�[�h�������
	if (activeNode != nullptr)
	{
		// �r�w�C�r�A�c���[����m�[�h�����s�B
		activeNode = aiTree->Run(activeNode, behaviorData, elapsedTime);
	}
	// ���ݎ��s����m�[�h���������
	else
	{
		// ���Ɏ��s����m�[�h�𐄘_����B
		activeNode = aiTree->ActiveNodeInference(behaviorData);
	}
}
