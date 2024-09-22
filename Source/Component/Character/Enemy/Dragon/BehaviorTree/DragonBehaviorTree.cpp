#include "DragonBehaviorTree.h"
#include "Component/Character/Enemy/Dragon/Judgment/DragonJudgment.h"
#include "Component/Character/Enemy/Dragon/Action/DragonAction.h"

#include <imgui.h>

DragonBehaviorTree::~DragonBehaviorTree()
{
	delete aiTree;
	delete behaviorData;
}

void DragonBehaviorTree::RegisterBehaviorTree(Dragon* owner)
{
	behaviorData = new BehaviorData<Dragon>();
	aiTree		 = new BehaviorTree<Dragon>(owner);

	aiTree->AddNode(B_Non, B_Root, Priority::Root, SelectRule::Priority);

#pragma region �U�����󂯂��Ƃ��̍s��
	aiTree->AddNode(B_Root, B_Movie,  Priority::Movie,  SelectRule::Sequence, 0, new J_DragonCondition(owner, Dragon::Condition::C_Movie));
	aiTree->AddNode(B_Root, B_Die,    Priority::Die,    SelectRule::Non,	  0, new J_DragonCondition(owner, Dragon::Condition::C_Die),    new A_Die(owner));
	aiTree->AddNode(B_Root, B_Flinch, Priority::Flinch, SelectRule::Non,	  0, new J_DragonCondition(owner, Dragon::Condition::C_Flinch), new A_Flinch(owner));
#pragma endregion

#pragma region ���[�r�[
	aiTree->AddNode(B_Movie, B_Claw, 0, SelectRule::Non, 0, nullptr, new A_MovieClaw(owner));
	aiTree->AddNode(B_Movie, B_Roar, 0, SelectRule::Non, 0, nullptr, new A_MovieRoar(owner));
#pragma endregion

#pragma region ���[�g
	aiTree->AddNode(B_Root, B_AreaMove, Priority::AreaMove, SelectRule::Sequence, 0, new J_DragonCondition(owner, Dragon::Condition::C_AreaMove));
	aiTree->AddNode(B_Root, B_Battle,   Priority::Battle,   SelectRule::Priority, 0, new J_DragonCondition(owner, Dragon::Condition::C_Battle));
	aiTree->AddNode(B_Root, B_Find,     Priority::Find,     SelectRule::Priority, 0, new J_DragonCondition(owner, Dragon::Condition::C_Find));
	aiTree->AddNode(B_Root, B_Scout,	Priority::Scout,	SelectRule::Priority);
#pragma endregion

#pragma region �G���A�ړ�
	aiTree->AddNode(B_AreaMove, B_Leave, 0, SelectRule::Non, 0, nullptr, new A_Intimidate(owner));
	aiTree->AddNode(B_AreaMove, B_Move,  0, SelectRule::Non, 0, nullptr, new A_AreaMove(owner));
#pragma endregion

#pragma region �o�g��
	// �{��
	aiTree->AddNode(B_Battle, B_Angry,	    0, SelectRule::Sequence, 0, new J_DragonAngry(owner));
	// �{�莞�̊m��s��
	aiTree->AddNode(B_Angry,  B_Roar,	    0, SelectRule::Non,	0, nullptr, new A_Roar(owner,  10.0f));
	aiTree->AddNode(B_Angry,  B_BackWalk,   0, SelectRule::Non,	0, nullptr, new A_Back(owner,  3.0f));
	aiTree->AddNode(B_Angry,  B_Run,        0, SelectRule::Non,	0, nullptr, new A_Rush(owner,  true, 1.5f, 3));
	aiTree->AddNode(B_Angry,  B_Roar,		0, SelectRule::Non,	0, nullptr, new A_Roar(owner,  50.0f));
	aiTree->AddNode(B_Angry,  B_Tired,	    0, SelectRule::Non,	0, nullptr, new A_Tired(owner, 5.0f));
	aiTree->AddNode(B_Angry,  B_Intimidate, 0, SelectRule::Non,	0, nullptr, new A_Intimidate(owner));

	// �v���C���[�Ƃ̋����őJ��
	aiTree->AddNode(B_Battle, B_Far,   1, SelectRule::Probability, 0,  new J_DragonFar(owner, owner->GetAttackRange()));
	aiTree->AddNode(B_Battle, B_Close, 2, SelectRule::Probability);

#pragma region �������s��
	aiTree->AddNode(B_Far, B_Pursuit, 0, SelectRule::Non, 30, nullptr, new A_Pursuit(owner));

	// �O�W�����v
	aiTree->AddNode(B_Far, B_Jump,    0, SelectRule::Non, 20, nullptr, new A_Jump(owner, 50.0f));

	// ��
	aiTree->AddNode(B_Far,  B_Shot,       0, SelectRule::Sequence, 15);
	aiTree->AddNode(B_Shot, B_Claw,       0, SelectRule::Non,		0, nullptr, new A_Claw(owner));
	aiTree->AddNode(B_Shot, B_Intimidate, 0, SelectRule::Non,		0, nullptr, new A_Intimidate(owner));

	// �ːi
	aiTree->AddNode(B_Far,     B_RushFar,    0, SelectRule::Sequence, 10);
	aiTree->AddNode(B_RushFar, B_BackWalk,   0, SelectRule::Non,       0, nullptr, new A_Back(owner));
	aiTree->AddNode(B_RushFar, B_Run,        0, SelectRule::Non,       0, nullptr, new A_Rush(owner, false, 1.5f));
	aiTree->AddNode(B_RushFar, B_Intimidate, 0, SelectRule::Non,       0, nullptr, new A_Intimidate(owner));

#pragma endregion

#pragma region �ߋ����s��
	// �U������s��
	aiTree->AddNode(B_Close, B_Turn, 0, SelectRule::Non, 50, new J_DragonTurn(owner), new A_Turn(owner));
	aiTree->AddNode(B_Close, B_Horn, 0, SelectRule::Non, 20, new J_DragonTurn(owner), new A_Horn(owner));

	// ���݂��U��
	aiTree->AddNode(B_Close,  B_Triple, 0, SelectRule::Sequence, 14);
	aiTree->AddNode(B_Triple, B_Basic,	0, SelectRule::Non,		  0, nullptr, new A_Bite(owner, 0.4f, true));
	aiTree->AddNode(B_Triple, B_Basic,	0, SelectRule::Non,		  0, nullptr, new A_Bite(owner, 0.5f, true));

	// �W�����v
	aiTree->AddNode(B_Close, B_Jump, 0, SelectRule::Non, 7, nullptr, new A_JumpAttack(owner));
	aiTree->AddNode(B_Close, B_Jump, 0, SelectRule::Non, 5, nullptr, new A_Jump(owner, -50.0f));

	// �ːi
	aiTree->AddNode(B_Close,     B_RushClose,  0, SelectRule::Sequence, 12);
	aiTree->AddNode(B_RushClose, B_BackWalk,   0, SelectRule::Non,		 0, nullptr, new A_Back(owner, 3.0f));
	aiTree->AddNode(B_RushClose, B_Run,		   0, SelectRule::Non,		 0, nullptr, new A_Rush(owner, false, 2.0f, 1));
	aiTree->AddNode(B_RushClose, B_Intimidate, 0, SelectRule::Non,		 0, nullptr, new A_Intimidate(owner));

	// ��2�A->�Њd
	aiTree->AddNode(B_Close,	 B_ShotAngry,  0, SelectRule::Sequence, 10, new J_DragonCondition(owner, Dragon::Condition::C_Angry));
	aiTree->AddNode(B_ShotAngry, B_BackWalk,   0, SelectRule::Non,	     0, nullptr, new A_Back(owner, 3.0f));
	aiTree->AddNode(B_ShotAngry, B_Run,		   0, SelectRule::Non,       0, nullptr, new A_Rush(owner, true, 1.5f, 2));
	aiTree->AddNode(B_ShotAngry, B_Claw,       0, SelectRule::Non,       0, nullptr, new A_Claw(owner));
	aiTree->AddNode(B_ShotAngry, B_Claw,       0, SelectRule::Non,       0, nullptr, new A_Claw(owner));
	aiTree->AddNode(B_ShotAngry, B_Tired,	   0, SelectRule::Non,	     0, nullptr, new A_Tired(owner, 5.0f));
	aiTree->AddNode(B_ShotAngry, B_Intimidate, 0, SelectRule::Non,       0, nullptr, new A_Intimidate(owner));

#pragma endregion
#pragma endregion

#pragma region ����
	aiTree->AddNode(B_Find, B_Turn, 0, SelectRule::Non, 0, new J_DragonTurn(owner), new A_Turn(owner));
	aiTree->AddNode(B_Find, B_Roar, 1, SelectRule::Non, 0, nullptr, new A_Roar(owner, 10.0f));
#pragma endregion

#pragma region �p�j�ړ�
	aiTree->AddNode(B_Scout, B_Wander, 0, SelectRule::Non, 0, new J_DragonWander(owner, owner->GetRadius()), new A_Wander(owner));
	aiTree->AddNode(B_Scout, B_Idle,   1, SelectRule::Non, 0, nullptr, new A_Idle(owner));
#pragma endregion

	// ���荞�݃m�[�h
	aiTree->AddInterruptionNode(B_Die,    new J_DragonCondition(owner, Dragon::Condition::C_Die));
	aiTree->AddInterruptionNode(B_Flinch, new J_DragonCondition(owner, Dragon::Condition::C_Flinch));
}

// �r�w�C�r�A�c���[�̍X�V
void DragonBehaviorTree::UpdateBehaviorTree(const float elapsedTime)
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

void DragonBehaviorTree::DebugBehaviorTree()
{
	if (ImGui::CollapsingHeader("BehaviorTree", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int currenNode = -1;
		if (activeNode != nullptr)
		{
			currenNode = activeNode->GetName();
		}

		ImGui::RadioButton("Leave", &currenNode, Behavior_Root::B_Leave);
		ImGui::RadioButton("Move", &currenNode, Behavior_Root::B_Move);

		ImGui::RadioButton("Angry", &currenNode, Behavior_Root::B_Angry);

		ImGui::RadioButton("Basic", &currenNode, Behavior_Root::B_Basic);
		ImGui::RadioButton("Claw", &currenNode, Behavior_Root::B_Claw);
		ImGui::RadioButton("Horn", &currenNode, Behavior_Root::B_Horn);

		ImGui::RadioButton("Jump", &currenNode, Behavior_Root::B_Jump);
		ImGui::RadioButton("Shot", &currenNode, Behavior_Root::B_Shot);
		ImGui::RadioButton("Run", &currenNode, Behavior_Root::B_Run);

		ImGui::RadioButton("Intimidate", &currenNode, Behavior_Root::B_Intimidate);
		ImGui::RadioButton("Pursuit", &currenNode, Behavior_Root::B_Pursuit);

		ImGui::RadioButton("Wander", &currenNode, Behavior_Root::B_Wander);
		ImGui::RadioButton("Idle", &currenNode, Behavior_Root::B_Idle);

		ImGui::RadioButton("Flinch", &currenNode, Behavior_Root::B_Flinch);
		ImGui::RadioButton("Die", &currenNode, Behavior_Root::B_Die);
	}
}