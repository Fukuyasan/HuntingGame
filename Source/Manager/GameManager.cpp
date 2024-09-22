#include "GameManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <types/string.hpp>
#include <archives/json.hpp>

#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"
#include "Scene/SceneLoding.h"
#include "Scene/SceneManager.h"

#include <imgui.h>

CEREAL_CLASS_VERSION(GameManager::QuestData, 1)

template<class Archive>
inline void GameManager::QuestData::serialize(Archive& archive)
{
	archive
	(
		CEREAL_NVP(monsterName),
		CEREAL_NVP(huntingCount)
	);
}

void GameManager::Initialize()
{
	returnTitle = false;

	//miniMap = std::make_unique<MiniMap>();

	questData.monsterName  = "Dragon";
	questData.huntingCount = 1;
}

void GameManager::Finalize()
{

}

void GameManager::Update(const float& elapsedTime)
{

}

void GameManager::DrawGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("GameManager", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::Checkbox("ReturnTitle", &returnTitle);
		//miniMap->DrawGUI();

		if (ImGui::Button("Save"))
		{
			Save();
		}
	}
	ImGui::End();
}

void GameManager::Save()
{
	// ファイル出力
	std::ofstream ofs("QuestData.json", std::ios::out);
	cereal::JSONOutputArchive archiveFile(ofs);
	questData.serialize(archiveFile);
}
