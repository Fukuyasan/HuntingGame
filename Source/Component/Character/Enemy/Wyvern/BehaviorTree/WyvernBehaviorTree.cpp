#include "WyvernBehaviorTree.h"

#include "Component/Character/Enemy/Wyvern/Action/WyvernAction.h"
#include "Component/Character/Enemy/Wyvern/Judgment/WyvernJudgment.h"

#ifdef Wyvern

void WyvernBehaviorTree::RegisterBehaviorTree(Wyvern* owner)
{
	behaviorData = new BehaviorData<Wyvern>();
	aiTree       = new BehaviorTree<Wyvern>(owner);

	aiTree->AddNode(B_Non, B_Root, Priority::Root, SelectRule::Priority);

#pragma region 攻撃を受けたときの行動
	aiTree->AddNode(B_Root, B_Die,    Priority::Die,	SelectRule::Non, 0, new J_WyvernCondition(owner, Wyvern::C_Die),    new A_WyvernDie(owner));
	aiTree->AddNode(B_Root, B_Flinch, Priority::Flinch, SelectRule::Non, 0, new J_WyvernCondition(owner, Wyvern::C_Flinch), new A_WyvernFlinch(owner));
#pragma endregion

#pragma region ルート
	aiTree->AddNode(B_Root, B_Battle, Priority::Battle, SelectRule::Random_OnOff, 0, new J_WyvernCondition(owner, Wyvern::Condition::C_Battle));
	aiTree->AddNode(B_Root, B_Find,   Priority::Find,   SelectRule::Priority,    0, new J_WyvernCondition(owner, Wyvern::Condition::C_Find));
	aiTree->AddNode(B_Root, B_Scout,  Priority::Scout,  SelectRule::Priority);
#pragma endregion

#pragma region バトル
	aiTree->AddNode(B_Battle, B_Claw, 0, SelectRule::Non, 50, nullptr, new A_WyvernClaw(owner));
	aiTree->AddNode(B_Battle, B_Horn, 0, SelectRule::Non, 25, nullptr, new A_WyvernHorn(owner));
#pragma endregion

#pragma region 発見
	//aiTree->AddNode(B_Find, B_Turn, 0, SelectRule::Non, 0, new J_WyvernTurn(owner), new A_WyvernTurn(owner));
	aiTree->AddNode(B_Find, B_Roar, 1, SelectRule::Non, 0, nullptr, new A_WyvernRoar(owner, 10.0f));
#pragma endregion

#pragma region 徘徊移動
	aiTree->AddNode(B_Scout, B_Wander, 0, SelectRule::Non, 0, new J_WyvernWander(owner, owner->GetRadius()), new A_WyvernWander(owner));
	aiTree->AddNode(B_Scout, B_Idle,   1, SelectRule::Non, 0, nullptr,			   new A_WyvernIdle(owner));
#pragma endregion

	// 割り込みノード
	aiTree->AddInterruptionNode(B_Die,    new J_WyvernCondition(owner, Wyvern::C_Die));
	aiTree->AddInterruptionNode(B_Flinch, new J_WyvernCondition(owner, Wyvern::C_Flinch));
}

void WyvernBehaviorTree::UpdateBehaviorTree(const float elapsedTime)
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

#endif