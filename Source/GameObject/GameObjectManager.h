#pragma once
#include <map>

#include "GameObject.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Shader.h"

class GameObjectManager final
{
private:
	GameObjectManager() {}
	~GameObjectManager() {}

public:
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager(GameObjectManager&&)		= delete;
	GameObjectManager operator=(const GameObjectManager&) = delete;
	GameObjectManager operator=(GameObjectManager&&)	  = delete;

	static GameObjectManager& Instance()
	{
		static GameObjectManager instance;
		return instance;
	}

	std::shared_ptr<GameObject> Create();
	void Remove(std::shared_ptr<GameObject> actor);

	void Update();
	void Render(const RenderContext& rc, Shader* shader);
	void OnGUI();

	void OnHierarchy();
	void OnInspector();

	std::shared_ptr<GameObject> Find(std::string name);

	void SetObjectID(std::string name, int id);

private:
	void StartObjects();
	void UpdateObjects();
	void RemoveObjects();

	void UpdateTransform();

public:
	std::vector<std::shared_ptr<GameObject>> m_startObjects;
	std::vector<std::shared_ptr<GameObject>> m_updateObjects;
	std::set<std::shared_ptr<GameObject>>    m_removeObjects;
	std::weak_ptr<GameObject> m_selectedObject;

	std::vector<std::shared_ptr<GameObject>> m_findObjects;

	std::map<std::string, int> m_objectsID;


public:
	static inline int m_objectCount = 0;
};