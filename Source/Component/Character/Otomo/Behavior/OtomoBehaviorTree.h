#pragma once
#include "AI/BehaviorTree/BehaviorTree.h"
#include "AI/BehaviorTree/BehaviorData.h"
#include "AI/BehaviorTree/NodeBase.h"

class Otomo;

class OtomoBehaviorTree
{
private:
	enum Behavior_Root
	{
		B_Non = -1,
		B_Root,

		// ルート
		B_KonckDown,
		B_Battle,
		B_Scout,

		B_Attack,

		// 吹き飛び
		B_Down,
		B_GetUp,

		// バトル
		B_Basic,
		B_Skill_A,
		B_Skill_B,

		// 徘徊移動
		B_Pursuit,
		B_Idle
	};

	// ビヘイビアツリーの優先順
	enum Priority
	{
		Root,
		KnockDown,
		Battle,
		Scout,
	};
public:
	~OtomoBehaviorTree();

	void RegisterBehaviorTree(Otomo* otomo);
	void UpdateBehaviorTree(const float elapsedTime);

private:
	BehaviorTree<Otomo>* aiTree       = nullptr;
	BehaviorData<Otomo>* behaviorData = nullptr;
	NodeBase<Otomo>*	 activeNode   = nullptr;

};
