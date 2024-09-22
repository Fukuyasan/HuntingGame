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

	// �f�o�b�O�`��
	virtual void OnDebugGUI() {}
	virtual void OnDebugPrimitive() {}

	// �A�N�^�[�ݒ�
	void SetGameObject(std::shared_ptr<GameObject> actor) { this->gameObject = actor; }

	// �A�N�^�[�擾
	std::shared_ptr<GameObject> GetGameObject() { return gameObject.lock(); }

	// ���O�擾
	virtual const char* GetName() const = 0;

protected:
	std::weak_ptr<GameObject> gameObject;
};