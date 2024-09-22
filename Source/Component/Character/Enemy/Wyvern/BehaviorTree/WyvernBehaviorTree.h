#pragma once
#include "AI/BehaviorTree/BehaviorTree.h"
#include "AI/BehaviorTree/BehaviorData.h"
#include "AI/BehaviorTree/NodeBase.h"

#ifdef Wyvern

class Wyvern;

class WyvernBehaviorTree
{
private:
	enum Behavior_Root
	{
		B_Non = -1,
		B_Root,

		B_Die,
		B_Flinch,

		// ���[�g
		B_Battle,
		B_Find,
		B_AreaMove,
		B_Scout,

		// �G���A�ړ�
		B_Leave,
		B_Move,

		// �o�g��
		B_Angry,
		B_Turn,

		B_Close,
		B_Far,
		B_Back,

		// �s��
		B_Basic,
		B_Claw,
		B_Horn,

		B_Triple,

		B_Jump,
		B_Shot,
		B_ShotAngry,
		B_Rush,

		B_Run,
		B_WallFlinch,

		B_Intimidate,
		B_Pursuit,
		B_BackWalk,

		// ����
		B_Roar,

		// �p�j�ړ�
		B_Wander,
		B_Idle
	};

	// �r�w�C�r�A�c���[�̗D�揇
	enum Priority
	{
		Root,
		Die,
		Flinch,
		AreaMove,
		Battle,
		Find,
		Scout,
	};

public:
	void RegisterBehaviorTree(Wyvern* owner);
	void UpdateBehaviorTree(const float elapsedTime);

	void DebugBehaviorTree() {}

private:
	BehaviorTree<Wyvern>* aiTree       = nullptr;
	BehaviorData<Wyvern>* behaviorData = nullptr;
	NodeBase<Wyvern>*     activeNode   = nullptr;
};

#endif
