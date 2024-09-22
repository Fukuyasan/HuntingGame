#pragma once
#include <vector>
#include "State.h"

template<class Owner>class StateMachine
{
public:
	// コンストラクタでownerを登録
	StateMachine() {}
	~StateMachine() 
	{
		statePool.clear();
	}

	// ステート更新処理
	void Update(const float& elapsedTime) {
		state->Execute(elapsedTime);
	}

	// 動かすステートをセット
	void SetState(int newState)
	{
		state = statePool[newState];
		state->Enter();
	}

	// 現在のステートを変更
	void ChangeState(int newState)
	{
		// 以前のステートを取得(便利そうだから)
		beforeState = GetStateNum();

		state->Exit();
		SetState(newState);
	}

	// ステート登録
	void RegisterState(std::shared_ptr<State<Owner>> state)
	{
		statePool.emplace_back(state);
	}

	// 現在のステート番号を取得
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

	// 過去のステート番号取得
	int GetBeforState() { return beforeState; }

private:
	std::vector<std::shared_ptr<State<Owner>>> statePool;

	// 現在のステート
	std::shared_ptr<State<Owner>> state = nullptr;

	// 前のステートを取得
	int beforeState = -1;
};
