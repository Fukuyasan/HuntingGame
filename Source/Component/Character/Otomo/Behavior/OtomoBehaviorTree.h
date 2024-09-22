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

		// ���[�g
		B_KonckDown,
		B_Battle,
		B_Scout,

		B_Attack,

		// �������
		B_Down,
		B_GetUp,

		// �o�g��
		B_Basic,
		B_Skill_A,
		B_Skill_B,

		// �p�j�ړ�
		B_Pursuit,
		B_Idle
	};

	// �r�w�C�r�A�c���[�̗D�揇
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
