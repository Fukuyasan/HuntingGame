#pragma once

#include <vector>
#include <stack>
#include <map>
#include "NodeBase.h"

// Behavior�ۑ��f�[�^
template<class Owner>class BehaviorData
{
public:
	// �R���X�g���N�^
	BehaviorData() { Init(); }
	// �V�[�P���X�m�[�h�̃v�b�V��
	void PushSequenceNode(NodeBase<Owner>* node) { sequenceStack.push(node); }
	
	// �V�[�P���X�m�[�h�̃|�b�v
	NodeBase<Owner>* PopSequenceNode()
	{
		// ��Ȃ�NULL
		if (sequenceStack.empty()) return nullptr;
		
		NodeBase<Owner>* node = sequenceStack.top();
		if (node)
		{
			// ���o�����f�[�^���폜
			sequenceStack.pop();
		}
		return node;
	}

	// �V�[�P���X�X�e�b�v�̃Q�b�^�[
	int GetSequenceStep(int name)
	{
		if (runSequenceStepMap.count(name) == 0)
		{
			runSequenceStepMap.insert(std::make_pair(name, 0));
		}

		return runSequenceStepMap[name];
	}

	// �V�[�P���X�X�e�b�v�̃Z�b�^�[
	void SetSequenceStep(int name, int step) { runSequenceStepMap[name] = step; }

	// ������
	void Init()
	{
		runSequenceStepMap.clear();
		while (sequenceStack.size() > 0)
		{
			sequenceStack.pop();
		}
	}
private:
	std::stack<NodeBase<Owner>*> sequenceStack;  // ���s���钆�ԃm�[�h���X�^�b�N
	std::map<int, int> runSequenceStepMap;		 // ���s���̒��ԃm�[�h�̃X�e�b�v���L�^
};
