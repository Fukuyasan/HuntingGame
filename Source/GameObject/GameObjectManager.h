#pragma once
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

	std::shared_ptr<GameObject> Find(const char* name);

private:
	void StartObjects();
	void UpdateObjects();
	void RemoveObjects();

private:
	std::vector<std::shared_ptr<GameObject>> m_startObjects;
	std::vector<std::shared_ptr<GameObject>> m_updateObjects;
	std::set<std::shared_ptr<GameObject>>    m_removeObjects;

};