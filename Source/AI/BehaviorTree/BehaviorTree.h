#pragma once
#include <string>
#include "JudgmentBase.h"
#include "ActionBase.h"

#define USE_STRING 0

template<class Owner> class NodeBase;
template<class Owner> class BehaviorData;

// �I�����[��
enum class SelectRule
{
	Non,				// �������[�m�[�h�p
	Priority,			// �D�揇��
	Sequence,			// �V�[�P���X
	Random,				// �����_��
	Random_OnOff,       // �����_���@�I���I�t�@�@
	Probability,        // �m���@���[���b�g�@
};

template<class Owner>class BehaviorTree
{
public:
	BehaviorTree(Owner* enemy) :root(nullptr) , owner(enemy){}

	~BehaviorTree() {
		NodeAllClear(root);
	}

	// ���s�m�[�h�𐄘_����
	NodeBase<Owner>* ActiveNodeInference(BehaviorData<Owner>* data)
	{
		// �f�[�^�����Z�b�g���ĊJ�n
		data->Init();
		return root->Inference(data);
	}

	// �V�[�P���X�m�[�h���琄�_�J�n
	NodeBase<Owner>* SequenceBack(NodeBase<Owner>* sequenceNode, BehaviorData<Owner>* data)
	{
		return sequenceNode->Inference(data);
	}

	// �m�[�h�ǉ�
	void AddNode(int parentName, int entryName, int priority, SelectRule selectRule, int probability = 0, JudgmentBase<Owner>* judgment = nullptr, ActionBase<Owner>* action = nullptr)
	{
		// Root(��ԏ�̃��[�g)����Ȃ��ꍇ
		if (parentName != rootNon)
		{
			// �e�m�[�h��T��
			NodeBase<Owner>* parentNode = root->SearchNode(parentName);

			if (parentNode != nullptr)
			{
				NodeBase<Owner>* sibling = parentNode->GetLastChild();
				NodeBase<Owner>* addNode = new NodeBase<Owner>(entryName, parentNode, sibling, priority, selectRule, probability, judgment, action, parentNode->GetHirerchyNo() + 1);

				parentNode->AddChild(addNode);
			}
		}
		// Root(��ԏ�̃��[�g)�̏ꍇ
		else
		{
			if (root == nullptr)
			{
				root = new NodeBase<Owner>(entryName, nullptr, nullptr, priority, selectRule, probability, judgment, action, 1);
			}
		}
	}

	// ���荞�݃m�[�h�ǉ�
	void AddInterruptionNode(int entryName, JudgmentBase<Owner>* judgment, ActionBase<Owner>* action = nullptr)
	{
		NodeBase<Owner>* interruptionNode = new NodeBase<Owner>(entryName, nullptr, nullptr, 0, SelectRule::Non, 0, judgment, action, 2);
		root->AddInterruption(interruptionNode);
	}

	// ���s�J�n����
	bool Start(NodeBase<Owner>* actionNode)
	{
		return actionNode->Start();
	}

	// ���s
	NodeBase<Owner>* Run(NodeBase<Owner>* actionNode, BehaviorData<Owner>* data, const float& elapsedTime)
	{
		// �m�[�h���s
		ActionState state;
		
		// �m�[�h���s
		state = actionNode->Run(elapsedTime);

		// ���荞��
		int interruptionCount = root->GetInterruptionSize();
		for (int i = 0; i < interruptionCount; ++i)
		{
			NodeBase<Owner>* inte = root->GetInterruption(i);

			if (!inte->judgment->Judgment()) continue;
			if (inte->name == actionNode->name) continue;
			
			state = actionNode->Stop();
		}
				
		// ����I��
		if (state == ActionState::Complete)
		{
			// �V�[�P���X�̓r�����𔻒f
			NodeBase<Owner>* sequenceNode = data->PopSequenceNode();

			// �r���Ȃ炻������n�߂�
			if (sequenceNode)
			{
				return SequenceBack(sequenceNode, data);
			}			

			// �Ȃ��Ȃ�I��
			return nullptr;
		}
		// ���s�͏I��
		if (state == ActionState::Failed)
		{
			return nullptr;
		}

		// ����ێ�
		return actionNode;
	}
private:
	// �m�[�h�S�폜
	void NodeAllClear(NodeBase<Owner>* _node)
	{
		for (NodeBase<Owner>* node : _node->children)
		{
			NodeAllClear(node);
		}

		for (NodeBase<Owner>* node : _node->interruption)
		{
			NodeAllClear(node);
		}

		delete _node;
	}
private:
	// ���[�g�m�[�h
	NodeBase<Owner>* root = nullptr;
	Owner* owner          = nullptr;

	const int rootNon = -1;
};