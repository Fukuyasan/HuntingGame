#pragma once

#include <vector>
#include <string>
#include <random>

#include "BehaviorTree.h"
#include "System/Math/Mathf.h"

// ���������[�N�����p
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

// �m�[�h
template<class Owner>class NodeBase
{
public:
	// �R���X�g���N�^
	NodeBase(int name, NodeBase* parent, NodeBase* sibling, int priority, SelectRule selectRule,
		int Probability, JudgmentBase<Owner>* judgment, ActionBase<Owner>* action, int hierarchyNo) :
		name(name), parent(parent), sibling(sibling), priority(priority), 
		selectRule(selectRule), probability(Probability), judgment(judgment), action(action), hierarchyNo(hierarchyNo),
		children(NULL) {}

	// �f�X�g���N�^
	~NodeBase() 
	{
		delete judgment;
		delete action;
	}

public:
#pragma region �Q�b�^�[ �Z�b�^�[
	// ���O�Q�b�^�[
	int GetName() { return name; }
	// �e�m�[�h�Q�b�^�[
	NodeBase* GetParent() { return parent; }
	// �q�m�[�h�Q�b�^�[
	NodeBase* GetChild(int index) {	return (index < children.size()) ? children.at(index) : nullptr; }
	// �q�m�[�h�Q�b�^�[(����)
	NodeBase* GetLastChild() { return (!children.empty()) ? children.at(children.size() - 1) : nullptr; }
	// �q�m�[�h�Q�b�^�[(�擪)
	NodeBase* GetTopChild()	{ return  (!children.empty()) ? children.at(0) : nullptr; }
	// �Z��m�[�h�Q�b�^�[
	NodeBase* GetSibling() { return sibling; }
	// �K�w�ԍ��Q�b�^�[
	int GetHirerchyNo() { return hierarchyNo; }
	// �D�揇�ʃQ�b�^�[
	int GetPriority() { return priority; }
	// �e�m�[�h�Z�b�^�[
	void SetParent(NodeBase* parent) { this->parent = parent; }
	// �q�m�[�h�ǉ�
	void AddChild(NodeBase* child) { children.emplace_back(child); }
	// �Z��m�[�h�Z�b�^�[
	void SetSibling(NodeBase* sibling) {this->sibling = sibling;}
	// �s���f�[�^�������Ă��邩
	bool HasAction() { return action; }
	int ReSetStep() { return action->step = 0; }
	int GetStep() { return action->step; }
	// ���s�۔���
	// judgment�����邩���f�B����΃����o�֐�Judgment()���s�������ʂ����^�[���B
	bool Judgment()	{ return (judgment) ? judgment->Judgment() : true; }

	// ���荞�݃m�[�h
	void AddInterruption(NodeBase* child) { interruption.emplace_back(child); }
	NodeBase* GetInterruption(int index) { return interruption[index]; }
	int GetInterruptionSize() { return static_cast<int>(interruption.size()); }
#pragma endregion

public:
	// �D�揇�ʑI��
	NodeBase* SelectPriority(const std::vector<NodeBase*>& nodePool)
	{
		NodeBase* selectNode  = nullptr;
		UINT priority         = INT_MAX;

		// ��ԗD�揇�ʂ������m�[�h��T����selectNode�Ɋi�[
		for (NodeBase* node : nodePool)
		{
			if (node->priority >= priority) continue;

			priority   = node->priority;
			selectNode = node;
		}

		return selectNode;
	}

	// �����_���I��
	NodeBase* SelectRandom(const std::vector<NodeBase*>& nodePool)
	{
		int selectNo = 0;
		// nodePool�̃T�C�Y�ŗ������擾����selectNo�Ɋi�[
		selectNo = Mathf::RandomRange(0, static_cast<int>(nodePool.size()));

		// nodePool��selectNo�Ԗڂ̎��Ԃ����^�[��
		return nodePool.at(selectNo);
	}

	// �V�[�P���X�I��
	NodeBase* SelectSequence(const std::vector<NodeBase*>& nodePool, BehaviorData<Owner>* data)
	{
		int step = 0;

		// �w�肳��Ă��钆�ԃm�[�h�̃V�[�P���X���ǂ��܂Ŏ��s���ꂽ���擾����
		step = data->GetSequenceStep(name);

		// ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ
		if (step >= children.size())
		{
			// ���[����BehaviorTree::SelectRule::Sequence�̂Ƃ��͎��Ɏ��s�ł���m�[�h���Ȃ����߁Anullptr�����^�[��
			if (selectRule == SelectRule::Sequence) {
				return nullptr;
			}
		}

		// ���s�\���X�g�ɓo�^����Ă���f�[�^�̐��������[�v���s��
		for (auto itr = nodePool.begin(); itr != nodePool.end(); itr++)
		{
			// �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩
			if ((*itr)->GetName() != children[step]->GetName()) continue;
			
			// ���݂̎��s�m�[�h�̕ۑ��A���Ɏ��s����X�e�b�v�̕ۑ����s������A
			// ���݂̃X�e�b�v�ԍ��̃m�[�h�����^�[��
			// �@�X�^�b�N�ɂ�data->PushSequenceNode�֐����g�p����B�ۑ�����f�[�^�͎��s���̒��ԃm�[�h�B
			data->PushSequenceNode(this);

			// �A�܂��A���Ɏ��s���钆�ԃm�[�h�ƃX�e�b�v����ۑ�����
			//  �ۑ��ɂ�data->SetSequenceStep�֐����g�p�B
			//  �ۑ��f�[�^�͒��ԃm�[�h�̖��O�Ǝ��̃X�e�b�v��(step + 1)
			data->SetSequenceStep(name, step + 1);

			// �B�X�e�b�v�ԍ��ڂ̎q�m�[�h�����s�m�[�h�Ƃ��ă��^�[������
			return children.at(step);
		}
		// �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�m�[�h���Ȃ��̂�nullptr�����^�[������
		return nullptr;
	}

	// on-off �����_���I��
	NodeBase* SelectRandom_OnOff(const std::vector<NodeBase*>& nodePool)
	{
		// nodePool�̃T�C�Y����vector�̃R���e�i�����
		std::vector<int> rand;
		for (int i = 0; i < nodePool.size(); i++)
		{
			rand.emplace_back(i);
		}

		// nodePool�̐������V���b�t������
		// ���ׂĂ�����ۂ��̂ō��܂���
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());
		std::shuffle(rand.begin(), rand.end(), engine);

		// �O��̐����Ɣ�r
		int selectNo = *rand.begin();
		if (selectNo == OnOffNo)
		{
			// �����������ꍇ��ԑO�̐���������
			rand.erase(rand.begin());
			// �c������ԑO�̐����� selectNo �ɑ��
			selectNo = *rand.begin();
		}

		OnOffNo = selectNo;
		return nodePool.at(selectNo);
	}

	// ���[���b�g�I��
	NodeBase* SelectProbability(const std::vector<NodeBase*>& nodePool)
	{
		// nodePool�̃T�C�Y����vector�̃R���e�i�����
		int total = 0;
		const size_t nodePoolCount = nodePool.size();

		std::vector<int> roulette;
		for (size_t i = 0; i < nodePoolCount; i++)
		{
			total += nodePool[i]->probability;
			roulette.emplace_back(nodePool[i]->probability);
		}

		// ���[���b�g�̔ԍ����擾
		int number = Mathf::RandomRange(0, total);
		total	   = 0;
		int answer = 0;
		for (int num : roulette)
		{
			total += num;

			if (total < number)
			{
				++answer;
				continue;
			}

			break;
		}

		return nodePool.at(answer);
	}

public:
	// �m�[�h����
	NodeBase* SearchNode(int searchName)
	{
		// ���O����v
		if (name == searchName)
		{
			return this;
		}
		
		// �q�m�[�h�Ō���
		for (auto itr = children.begin(); itr != children.end(); itr++)
		{
			auto result = (*itr)->SearchNode(searchName);
			if (result)
			{
				return result;
			}
		}

		return nullptr;
	}

	// �m�[�h���_
	NodeBase* Inference(BehaviorData<Owner>* data)
	{
		std::vector<NodeBase*> nodePool;
		NodeBase* result = nullptr;

		// children�̐��������[�v���s���B
		for (NodeBase* child : children)
		{
			// ����N���X���Ȃ���Ζ������ɒǉ�
			if (!child->judgment)
			{
				nodePool.emplace_back(child);
				continue;
			}

			// ����������A���̏����𖞂����Ă���Βǉ�
			if (child->judgment->Judgment()) nodePool.emplace_back(child);
		}

		// �I�����[���Ńm�[�h����
		switch (selectRule)
		{
		case SelectRule::Priority:
			result = SelectPriority(nodePool);
			break;
		case SelectRule::Random:
			result = SelectRandom(nodePool);
			break;
		case SelectRule::Sequence:
			result = SelectSequence(nodePool, data);
			break;
		case SelectRule::Random_OnOff:
			result = SelectRandom_OnOff(nodePool);
			break;
		case SelectRule::Probability:
			result = SelectProbability(nodePool);
			break;
		}

		if (result)
		{
			// result��action�m�[�h���Ȃ������ꍇ�A
			// �Ăуm�[�h���_
			if (!result->HasAction())
			{
				result = result->Inference(data);
			}
			else
			{
				result->Start();
			}
		}

		return result;
	}

	// ���s
	bool Start()
	{
		// action�����邩���f�B����΃����o�֐�Start()�����s
		if (!action) return false;

		return action->Start();
	}

	// ���s
	ActionState Run(const float& elapsedTime)
	{
		// action�����邩���f�B����΃����o�֐�Run()���s�������ʂ����^�[���B
		if (!action) return ActionState::Failed;
		
		return action->Run(elapsedTime);
	}

	// ���s���̍s�����~
	ActionState Stop()
	{
		return ActionState::Failed;
	}

	std::vector<NodeBase*> children;		    // �q�m�[�h
	std::vector<NodeBase*> interruption;		// ���f�m�[�h
public:
	int							name;			    // ���O
	SelectRule					selectRule;			// �I�����[��
	JudgmentBase<Owner>*		judgment;			// ����N���X
	ActionBase<Owner>*			action;				// ���s�N���X
	unsigned int				priority;		    // �D�揇��
	NodeBase*					parent;			    // �e�m�[�h
	NodeBase*					sibling;		    // �Z��m�[�h
	int							hierarchyNo;	    // �K�w�ԍ�
	int                         OnOffNo = INT_MAX;  // on-off�p�̐؂�ւ��ϐ�
	int                         probability;  // ���[���b�g�ɂ��m���i�傫����΂��̕��m���͍����Ȃ�j
};