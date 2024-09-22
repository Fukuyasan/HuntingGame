#pragma once
#include "NodeBase.h"

class SelectRule
{
public:
	// �D�揇�ʑI��
	static NodeBase* SelectPriority(const std::vector<NodeBase*>& nodePool)
	{
		NodeBase* selectNode = nullptr;
		UINT priority = INT_MAX;

		// ��ԗD�揇�ʂ������m�[�h��T����selectNode�Ɋi�[
		for (NodeBase* node : nodePool)
		{
			if (node->priority >= priority) continue;

			priority = node->priority;
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
		const int nodePoolCount = nodePool.size();

		std::vector<int> roulette;
		for (int i = 0; i < nodePoolCount; i++)
		{
			total += nodePool[i]->probability;
			roulette.emplace_back(nodePool[i]->probability);
		}

		// ���[���b�g�̔ԍ����擾
		int number = Mathf::RandomRange(0, total);

		total = 0;
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
};