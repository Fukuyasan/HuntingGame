#pragma once
#include "AI/BehaviorTree/BehaviorTree.h"
#include "AI/BehaviorTree/BehaviorData.h"
#include "AI/BehaviorTree/NodeBase.h"

class Dragon;

class DragonBehaviorTree
{
private:
	enum Behavior_Root
	{
		B_Non = -1,
		B_Root,

		B_Movie,
		B_Die,
		B_Flinch,

		// ルート
		B_Battle,
		B_Find,
		B_AreaMove,
		B_Scout,

		// エリア移動
		B_Leave,
		B_Move,

		// バトル
		B_Angry,
		B_Turn,

		B_Close,
		B_Far,
		B_Back,

		// 行動
		B_Basic,
		B_Claw,
		B_Horn,

		B_Triple,

		B_Jump,
		B_Shot,
		B_ShotAngry,
		B_RushFar,
		B_RushClose,

		B_Run,
		B_WallFlinch,

		B_Intimidate,
		B_Pursuit,
		B_BackWalk,
		B_Tired,

		// 発見
		B_Roar,

		// 徘徊移動
		B_Wander,
		B_Idle
	};

	// ビヘイビアツリーの優先順
	enum Priority
	{
		Root,
		Movie,
		Die,
		Flinch,
		AreaMove,
		Battle,
		Find,
		Scout,
	};

public:
	DragonBehaviorTree() {}
	~DragonBehaviorTree();

	// ビヘイビアツリー設定
	void RegisterBehaviorTree(Dragon* owner);
	void UpdateBehaviorTree(const float elapsedTime);

	void DebugBehaviorTree();

private:
	BehaviorTree<Dragon>* aiTree       = nullptr;
	BehaviorData<Dragon>* behaviorData = nullptr;
	NodeBase<Dragon>*	  activeNode   = nullptr;

	bool startAction = false;
};
