#include "Graphics/Graphics.h"
#include "GameObjectManager.h"

std::shared_ptr<GameObject> GameObjectManager::Create()
{
	auto object = std::make_shared<GameObject>();
	{
		static int id = 0;
		char name[256];
		::sprintf_s(name, sizeof(name), "GameObject%d", id++);
		object->SetName(name);
	}
	m_startObjects.emplace_back(object);
	return object;
}

void GameObjectManager::Remove(std::shared_ptr<GameObject> actor)
{
	m_removeObjects.insert(actor);
}

void GameObjectManager::Update()
{
	StartObjects();
	UpdateObjects();
	RemoveObjects();
}

void GameObjectManager::Render(const RenderContext& rc, Shader* shader)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	shader->Begin(dc, rc);

	for (auto& object : m_updateObjects)
	{
		shader->Draw(dc, object->GetModel());
	}

	shader->End(dc);
}

void GameObjectManager::OnGUI()
{
	for (auto& object : m_updateObjects)
	{
		object->OnGUI();
	}
}

std::shared_ptr<GameObject> GameObjectManager::Find(const char* name)
{
	// ‚Ü‚¸‰Šú‰»”z—ñ‚Å’T‚·
	for (auto& object : m_startObjects)
	{
		if (object->GetNameToString() != name) continue;

		return object;
	}

	// ŽŸ‚ÉXV”z—ñ‚Å’T‚·
	for (auto& object : m_updateObjects)
	{
		if (object->GetNameToString() != name) continue;

		return object;
	}

	return nullptr;
}

void GameObjectManager::StartObjects()
{
	for (auto& object : m_startObjects)
	{
		object->Start();
		m_updateObjects.emplace_back(object);
	}
	m_startObjects.clear();
}

void GameObjectManager::UpdateObjects()
{
	for (auto& object : m_updateObjects)
	{
		object->Update();
	}
}

void GameObjectManager::RemoveObjects()
{
	auto eraseObject = [this](std::vector<std::shared_ptr<GameObject>>& gameObjects, const std::shared_ptr<GameObject>& object)
		{
			auto it = std::find(gameObjects.begin(), gameObjects.end(), object);
			if (it != gameObjects.end())
			{
				gameObjects.erase(it);
			}
		};

	for (auto& object : m_removeObjects)
	{
		eraseObject(m_startObjects,  object);
		eraseObject(m_updateObjects, object);
	}
	m_removeObjects.clear();
}
