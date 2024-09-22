#pragma once

#include "player.h"
#include "AI/StateMachine/StateMachine.h"
#include "AI/StateMachine/State.h"

// ************************************************************
// 
//		ステート
// 
// ************************************************************

// 待機ステート
class IdleState : public State<Player>
{
public:
	IdleState(Player* player) : State<Player>(player) {}
	~IdleState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

// 歩きステート
class WalkState : public State<Player>
{
public:
	WalkState(Player* player) : State<Player>(player) {}
	~WalkState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override;
};

// 走りステート
class RunState : public State<Player>
{
public:
	RunState(Player* player) : State<Player>(player) {}
	~RunState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override;
};

// ダッシュステート
class DashState : public State<Player>
{
public:
	DashState(Player* player) : State<Player>(player) {}
	~DashState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override;
};

// 回避ステート
class AvoidState : public State<Player>
{
public:
	AvoidState(Player* player) : State<Player>(player) {}
	~AvoidState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

// -----------------------
// 
//		弱攻撃ステート
// 
// -----------------------

class ComboA_1State : public State<Player>
{
public:
	ComboA_1State(Player* player) : State<Player>(player) {}
	~ComboA_1State() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override;
};

class ComboA_2State : public State<Player>
{
public:
	ComboA_2State(Player* player) : State<Player>(player) {}
	~ComboA_2State() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override;
};

class ComboA_3State : public State<Player>
{
public:
	ComboA_3State(Player* player) : State<Player>(player) {}
	~ComboA_3State() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

// -----------------------
// 
//		強攻撃ステート
// 
// -----------------------

class ComboB_1State : public State<Player>
{
public:
	ComboB_1State(Player* player) : State<Player>(player) {}
	~ComboB_1State() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override;
};

class ComboB_2State : public State<Player>
{
public:
	ComboB_2State(Player* player) : State<Player>(player) {}
	~ComboB_2State() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

class ComboB_3State : public State<Player>
{
public:
	ComboB_3State(Player* player) : State<Player>(player) {}
	~ComboB_3State() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

class ComboB_4State : public State<Player>
{
public:
	ComboB_4State(Player* player) : State<Player>(player) {}
	~ComboB_4State() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

// -----------------------
//
//	 ダッシュ攻撃ステート
//
// -----------------------

class Attack_DashState : public State<Player>
{
public:
	Attack_DashState(Player* player) : State<Player>(player) {}
	~Attack_DashState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

// -----------------------
//
//	 カウンター攻撃ステート
//
// -----------------------

class Counter_StartState : public State<Player>
{
public:
	Counter_StartState(Player* player) : State<Player>(player) {}
	~Counter_StartState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

class Counter_AttackState : public State<Player>
{
public:
	Counter_AttackState(Player* player) : State<Player>(player) {}
	~Counter_AttackState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

class Counter_EndState : public State<Player>
{
public:
	Counter_EndState(Player* player) : State<Player>(player) {}
	~Counter_EndState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
};

// ダメージステート
class DamageState : public State<Player>
{
public:
	DamageState(Player* player) : State<Player>(player) {}
	~DamageState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override {}

	// プレイヤーの前方向と吹き飛ばす方向から
	// アニメーションを設定
	float ComputeDot();
private:
	float standUpTimer = 5.0f;

	float animDot = 0.0f;
};

// 起き上がりステート
class StandUpState : public State<Player>
{
public:
	StandUpState(Player* player) : State<Player>(player) {}
	~StandUpState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override {}
};

// 死亡ステート
class DeathState : public State<Player>
{
public:
	DeathState(Player* player) : State<Player>(player) {}
	~DeathState() {}
	void Enter() override;
	void Execute(const float& elapsedTime) override;
	void Exit() override {}
};