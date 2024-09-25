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
class GameObjectManager;

// ラベル
enum class Layer
{
	Player,
	Otomo,
	Enemy,
	Weapon,
	Stage,
	Non
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
	void SetName(std::string name);

	const char* GetName() { return m_name.c_str(); }

	// ラベル
	void SetLayer(Layer label) { m_layer = label; }
	Layer GetLayer() { return m_layer; }

	// モデル取得
	void LoadModel(const char* filename) { m_model = std::make_unique<Model>(filename); }
	Model* GetModel() { return m_model.get(); }

	Transform GetTransform() { return transform; }

	void SetID(const int id) { ID = id; }
	const int GetID() { return ID; }

	/// <summary>
	/// コンポーネント取得
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
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

	/// <summary>
	/// コンポーネント追加
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<class T, class... Args>
	std::shared_ptr<T> AddComponent(Args... args)
	{
		std::shared_ptr<T> p = std::make_shared<T>(args...);

		p->SetGameObject(shared_from_this());
		m_components.emplace_back(p);

		p->Awake();

		return p;
	}

	void SetParent(std::string parentName);

private:
	void AddChildren(std::shared_ptr<GameObject> children)
	{
		m_children.emplace_back(children);
	}

public:
	Transform transform;
	std::weak_ptr<GameObject> m_parent;

private:
	std::string m_name;
	Layer m_layer = Layer::Non;

	std::unique_ptr<Model> m_model;
	std::vector<std::shared_ptr<Component>> m_components;

	int ID = 0;

	std::vector<std::shared_ptr<GameObject>> m_children;
};