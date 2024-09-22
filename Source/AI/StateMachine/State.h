#pragma once
#include <vector>

// �X�e�[�g
template<class Owner>class State
{
public:
	State(Owner* owner) : owner(owner) {}
	virtual ~State() {}

	// �X�e�[�g�ɓ��������̃��]�b�g
	virtual void Enter() = 0;

	// �X�e�[�g�Ŏ������郁�]�b�g
	virtual void Execute(const float& elapsedTime) = 0;

	// �X�e�[�g����o�Ă����Ƃ��̃��]�b�g
	virtual void Exit() {}

protected:
	//�I�[�i�[�̃L���b�V���@
	// �����ۂɂ͂��̃N���X���I�[�i�[���폜���邱�ƂȂ��A
	// �I�[�i�[���폜���ꂽ��ɂ��̃N���X�ɑ΂��đ��삳��邱�Ƃ��Ȃ����߁A
	// ���|�C���^�ŕۊǂ��Ă���
	Owner* owner = nullptr;
};