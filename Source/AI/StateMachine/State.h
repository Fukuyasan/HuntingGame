#pragma once
#include <vector>

// ステート
template<class Owner>class State
{
public:
	State(Owner* owner) : owner(owner) {}
	virtual ~State() {}

	// ステートに入った時のメゾット
	virtual void Enter() = 0;

	// ステートで実装するメゾット
	virtual void Execute(const float& elapsedTime) = 0;

	// ステートから出ていくときのメゾット
	virtual void Exit() {}

protected:
	//オーナーのキャッシュ　
	// ※実際にはこのクラスがオーナーが削除することなく、
	// オーナーが削除された後にこのクラスに対して操作されることがないため、
	// 生ポインタで保管しておく
	Owner* owner = nullptr;
};