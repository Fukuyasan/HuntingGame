#pragma once
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <DirectXMath.h>

#include "Graphics/Model/Model.h"
#include "Transform/Transform.h"

#include <assert.h>

// 前方宣言
class Component;

// ラベル
enum class Layer
{
	Default,
	Player,
	Otomo,
	Enemy,
	Weapon,
	Stage,
	Max
};

// コンポーネントを追加するための箱
class GameObject : public std::enable_shared_from_this<GameObject> // shared_from_thisを使えるようにする
{
public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Start();
	virtual void Update();
	virtual void OnGUI();

	// 名前
	void SetName(const char* name) { m_name = name; }
	const char* GetName() { return m_name.c_str(); }
	std::string GetNameToString() { return m_name; }

	void SetParent(const char* name) { m_parentName = name; }
	const char* GetParentName() { return m_parentName.c_str(); }

	// ラベル
	void SetLayer(Layer layer) { m_layer = layer; }
	Layer GetLayer() { return m_layer; }

	// モデル取得
	void LoadModel(const char* filename) { m_model = std::make_unique<Model>(filename); }
	Model* GetModel() { return m_model.get(); }

	Transform GetTransform() { return transform; }

	// コンポーネント取得
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		for (auto& component : m_components)
		{
			auto p = std::dynamic_pointer_cast<T>(component);

			if (!p) continue;
			return p;
		}

		return nullptr;
	}

	// コンポーネント追加
	template<class T, class... Args>
	std::shared_ptr<T> AddComponent(Args... args)
	{
		std::shared_ptr<T> p = std::make_shared<T>(args...);

		p->SetGameObject(shared_from_this());
		m_components.emplace_back(p);

		p->Awake();

		return p;
	}

public:
	Transform transform;

private:
	std::string m_name;
	std::string m_parentName;
	Layer m_layer;

	std::unique_ptr<Model> m_model;

	std::vector<std::shared_ptr<Component>> m_components;
};