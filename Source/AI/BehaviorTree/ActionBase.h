#pragma once

// 実行情報
enum class ActionState
{
	Run,		// 実行中
	Failed,		// 実行失敗
	Complete,	// 実行成功
};

// 行動処理基底クラス
template<class Owner>class ActionBase
{
public:
	ActionBase(Owner* enemy) : owner(enemy){}
	virtual ~ActionBase() {}
	
	// 初期設定
	virtual bool Start() { return true; }

	// 実行処理(純粋仮想関数) 
	virtual ActionState Run(const float& elapsedTime) { return ActionState::Complete; }

protected:
	Owner* owner;
};