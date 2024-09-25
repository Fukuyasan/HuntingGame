#include "GameObject.h"
#include "GameObjectManager.h"

#include "Component/Component.h"

void GameObject::Start()
{
	for (auto& component : m_components)
	{
		component->Start();
	}
}

void GameObject::Update()
{
	for (auto& component : m_components)
	{
		component->Update();
	}
}

void GameObject::OnGUI()
{
	for (auto& component : m_components)
	{
		component->OnDebugPrimitive();
		component->OnDebugGUI();
	}
}

void GameObject::SetName(std::string name)
{
	m_name = name;
	int id = shared_from_this()->GetID();

	GameObjectManager::Instance().SetObjectID(name, id);
}

void GameObject::SetParent(std::string parentName)
{
	auto parent = GameObjectManager::Instance().Find(parentName);

	parent->AddChildren(shared_from_this());
	m_parent = parent;
}
