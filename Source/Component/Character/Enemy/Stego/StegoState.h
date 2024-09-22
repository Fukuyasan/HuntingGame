#pragma once

#include "Stego.h"
#include "AI/StateMachine/StateMachine.h"
#include "AI/StateMachine/State.h"

#ifdef Stego

// ************************************************************
// 
//		�X�e�[�g
// 
// ************************************************************

// �ҋ@�X�e�[�g
class StegoIdleState : public State<Stego>
{
public:
	StegoIdleState(Stego* stego) : State<Stego>(stego) {}
	~StegoIdleState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

// ����X�e�[�g
class StegoRunState : public State<Stego>
{
public:
	StegoRunState(Stego* stego) : State<Stego>(stego) {}
	~StegoRunState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;

private:
	DirectX::XMFLOAT3 areaPos = {};
};

// ���S�X�e�[�g
class StegoDeathState : public State<Stego>
{
public:
	StegoDeathState(Stego* stego) : State<Stego>(stego) {}
	~StegoDeathState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

#endif