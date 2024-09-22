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


#pragma region ルート
	aiTree->AddNode(B_Root, B_KonckDown, Priority::KnockDown, SelectRule::Sequence, 0, new J_OtomoCondition(owner, Otomo::Condition::C_KnockDown));
	aiTree->AddNode(B_Root, B_Battle,    Priority::Battle,    SelectRule::Priority, 0, new J_OtomoCondition(owner, Otomo::Condition::C_Battle));
	aiTree->AddNode(B_Root, B_Scout,     Priority::Scout,     SelectRule::Priority);
#pragma endregion

#pragma region 攻撃を受けたときの行動
	aiTree->AddNode(B_KonckDown, B_Down,  0, SelectRule::Non, 0, nullptr, new A_OtomoKnockDown(owner));
	aiTree->AddNode(B_KonckDown, B_GetUp, 0, SelectRule::Non, 0, nullptr, new A_OtomoGetUp(owner));
#pragma endregion

#pragma region バトル

	aiTree->AddNode(B_Battle, B_Attack,  0, SelectRule::Probability);
	aiTree->AddNode(B_Attack, B_Basic,   0, SelectRule::Non,   50, nullptr, new A_OtomoAttack(owner));
	aiTree->AddNode(B_Attack, B_Skill_A, 0, SelectRule::Non,   35, nullptr, new A_OtomoSkill_A(owner));
	aiTree->AddNode(B_Attack, B_Skill_B, 0, SelectRule::Non,   15, nullptr, new A_OtomoSkill_B(owner));

#pragma region 徘徊移動
	aiTree->AddNode(B_Scout, B_Pursuit, 0, SelectRule::Non, 0, new J_OtomoWander(owner, 5.0f), new A_OtomoWander(owner));
	aiTree->AddNode(B_Scout, B_Idle,    1, SelectRule::Non, 0, nullptr, new A_OtomoIdle(owner));
#pragma endregion

	// 割り込みノード
	aiTree->AddInterruptionNode(B_KonckDown, new J_OtomoCondition(owner, Otomo::Condition::C_KnockDown));
}

void OtomoBehaviorTree::UpdateBehaviorTree(const float elapsedTime)
{
	// 現在実行するノードがあれば
	if (activeNode != nullptr)
	{
		// ビヘイビアツリーからノードを実行。
		activeNode = aiTree->Run(activeNode, behaviorData, elapsedTime);
	}
	// 現在実行するノードが無ければ
	else
	{
		// 次に実行するノードを推論する。
		activeNode = aiTree->ActiveNodeInference(behaviorData);
	}
}
