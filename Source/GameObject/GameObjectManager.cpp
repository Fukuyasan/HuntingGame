#include <imgui.h>

#include "Graphics/Graphics.h"
#include "GameObjectManager.h"

std::shared_ptr<GameObject> GameObjectManager::Create()
{
	auto object = std::make_shared<GameObject>();

	object->SetID(m_objectCount);

	m_startObjects.emplace_back(object);
	m_findObjects.emplace_back(object);
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
		// モデルが存在していたら描画
		if (!object->GetModel()) continue;
		shader->Draw(dc, object->GetModel());
	}

	shader->End(dc);
}

void GameObjectManager::OnGUI()
{
	OnHierarchy();
	OnInspector();
}

void GameObjectManager::OnHierarchy()
{
	ImGui::SetNextWindowPos(ImVec2(10, 350), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoScrollWithMouse))
	{
		for (auto& object : m_updateObjects)
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;
			
			ImGui::TreeNodeEx(object.get(), nodeFlags, object->GetName());

			if (ImGui::IsItemClicked())
			{
				// 単一選択だけ対応しておく
				ImGuiIO& io = ImGui::GetIO();
				m_selectedObject = object;
			}		

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void GameObjectManager::OnInspector()
{
	ImGui::SetNextWindowPos(ImVec2(10, 350), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoScrollWithMouse))
	{
		if (m_selectedObject.lock() != nullptr)
		{
			// トランスフォーム
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 位置
				DirectX::XMFLOAT3 position = m_selectedObject.lock()->transform.GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.1f);
				m_selectedObject.lock()->transform.SetPosition(position);

				// 回転
				DirectX::XMFLOAT3 angle = m_selectedObject.lock()->transform.GetAngle();
				DirectX::XMFLOAT3 a{};
				a.x = DirectX::XMConvertToDegrees(angle.x);
				a.y = DirectX::XMConvertToDegrees(angle.y);
				a.z = DirectX::XMConvertToDegrees(angle.z);
				ImGui::DragFloat3("Angle", &a.x);
				angle.x = DirectX::XMConvertToRadians(a.x);
				angle.y = DirectX::XMConvertToRadians(a.y);
				angle.z = DirectX::XMConvertToRadians(a.z);

				m_selectedObject.lock()->transform.SetAngle(angle);

				// スケール
				float s = m_selectedObject.lock()->transform.GetScale().x;
				ImGui::DragFloat("Scale", &s, 0.1f);
				m_selectedObject.lock()->transform.SetScale(s);
			}

			m_selectedObject.lock()->OnGUI();
		}
	}

	ImGui::End();
}

std::shared_ptr<GameObject> GameObjectManager::Find(const char* name)
{
	const int findID = m_objectsID[name];

	for (auto& object : m_findObjects)
	{
		if (object->GetID() != findID) continue;

		return object;
	}

	return nullptr;
}

void GameObjectManager::SetObjectID(const char* name)
{
	m_objectsID.emplace(std::make_pair(name, m_objectCount));
	++m_objectCount;
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
