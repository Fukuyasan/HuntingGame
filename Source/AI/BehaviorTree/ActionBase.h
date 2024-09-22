#pragma once

// ���s���
enum class ActionState
{
	Run,		// ���s��
	Failed,		// ���s���s
	Complete,	// ���s����
};

// �s���������N���X
template<class Owner>class ActionBase
{
public:
	ActionBase(Owner* enemy) : owner(enemy){}
	virtual ~ActionBase() {}
	
	// �����ݒ�
	virtual bool Start() { return true; }

	// ���s����(�������z�֐�) 
	virtual ActionState Run(const float& elapsedTime) { return ActionState::Complete; }

protected:
	Owner* owner;
};