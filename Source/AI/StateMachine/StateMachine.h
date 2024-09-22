#pragma once
#include <vector>
#include "State.h"

template<class Owner>class StateMachine
{
public:
	// �R���X�g���N�^��owner��o�^
	StateMachine() {}
	~StateMachine() 
	{
		statePool.clear();
	}

	// �X�e�[�g�X�V����
	void Update(const float& elapsedTime) {
		state->Execute(elapsedTime);
	}

	// �������X�e�[�g���Z�b�g
	void SetState(int newState)
	{
		state = statePool[newState];
		state->Enter();
	}

	// ���݂̃X�e�[�g��ύX
	void ChangeState(int newState)
	{
		// �ȑO�̃X�e�[�g���擾(�֗�����������)
		beforeState = GetStateNum();

		state->Exit();
		SetState(newState);
	}

	// �X�e�[�g�o�^
	void RegisterState(std::shared_ptr<State<Owner>> state)
	{
		statePool.emplace_back(state);
	}

	// ���݂̃X�e�[�g�ԍ����擾
	int GetStateNum()
	{
		int currentState = 0;
		for (auto state : statePool)
		{
			if (this->state == state) return currentState;
			++currentState;
		}

		return -1;
	}

	// �ߋ��̃X�e�[�g�ԍ��擾
	int GetBeforState() { return beforeState; }

private:
	std::vector<std::shared_ptr<State<Owner>>> statePool;

	// ���݂̃X�e�[�g
	std::shared_ptr<State<Owner>> state = nullptr;

	// �O�̃X�e�[�g���擾
	int beforeState = -1;
};
