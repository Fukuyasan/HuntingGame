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

void GameObject::SetName(const char* name)
{
	m_name = name;
	GameObjectManager::Instance().SetObjectID(name);
}
