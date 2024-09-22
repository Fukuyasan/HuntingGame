#pragma once
#include <memory>
#include "GameObject/GameObject.h"

class Component
{
public:
	Component() {}
	virtual ~Component() {}

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}

	// デバッグ描画
	virtual void OnDebugGUI() {}
	virtual void OnDebugPrimitive() {}

	// アクター設定
	void SetGameObject(std::shared_ptr<GameObject> actor) { this->gameObject = actor; }

	// アクター取得
	std::shared_ptr<GameObject> GetGameObject() { return gameObject.lock(); }

	// 名前取得
	virtual const char* GetName() const = 0;

protected:
	std::weak_ptr<GameObject> gameObject;
};