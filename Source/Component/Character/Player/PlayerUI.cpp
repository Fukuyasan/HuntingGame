#include "PlayerUI.h"
#include "Player.h"
#include "UI/UIManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <types/vector.hpp>
#include <types/string.hpp>
#include <archives/json.hpp>
#include <imgui.h>

#include "Cereal/Cereal.h"
#include "GameObject/GameObjectManager.h"


#define UIFLG 1

template<class Archive>
inline void PlayerUI::GaugeParam::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(position),
		CEREAL_NVP(scale)
	);
}

void PlayerUI::Initialize()
{
#if UIFLG
	UIManager& uiManager = UIManager::Instance();
	auto player = GameObjectManager::Instance().Find("Player")->GetComponent<Player>();

	float index = static_cast<float>(player->GetMaxHealth());

	// UIパラメータの読み込み
	LoadUIParam();

#pragma region アイコン
	playerIcon = std::make_unique<UIButton>("Data/Sprite/UI/Icon.png");
	playerIcon->SetPosition(IconParam.position);
	playerIcon->SetScale(IconParam.scale);

	uiManager.RegisterUI(playerIcon.get());
#pragma endregion
	
#pragma region HP
	playerGaugeBox[0] = std::make_unique<UIButton>("Data/Sprite/UI/PlayerHPGaugeBox.png");
	playerGaugeBox[0]->SetPosition(HPBoxParam.position);
	playerGaugeBox[0]->SetScale(HPBoxParam.scale);
	playerGaugeBox[0]->SetCenter(Center::Middle);

	playerHP = std::make_unique<UIGauge>("Data/Sprite/UI/PlayerHPGauge.png");
	playerHP->SetPosition(HPParam.position);
	playerHP->SetScale(HPParam.scale);
	playerHP->SetMax(index);
	playerHP->SetCenter(Center::Middle);

	uiManager.RegisterUI(playerGaugeBox[0].get());
	uiManager.RegisterUI(playerHP.get());
#pragma endregion

	index = static_cast<float>(player->GetMaxStamina());

#pragma region スタミナ
	playerGaugeBox[1] = std::make_unique<UIButton>("Data/Sprite/UI/PlayerStaminaGaugeBox.png");
	playerGaugeBox[1]->SetPosition(StaminaBoxParam.position);
	playerGaugeBox[1]->SetScale(StaminaBoxParam.scale);
	playerGaugeBox[1]->SetCenter(Center::Middle);

	playerStamina = std::make_unique<UIGauge>("Data/Sprite/UI/PlayerStaminaGauge.png");
	playerStamina->SetPosition(StaminaParam.position);
	playerStamina->SetScale(StaminaParam.scale);
	playerStamina->SetMax(index);
	playerStamina->SetCenter(Center::Middle);

	uiManager.RegisterUI(playerGaugeBox[1].get());
	uiManager.RegisterUI(playerStamina.get());
#pragma endregion
#endif
}

void PlayerUI::Update(float elapsedTime)
{
#if UIFLG
	auto player = GameObjectManager::Instance().Find("Player")->GetComponent<Player>();

	playerHP->SetIndex(static_cast<float>(player->GetHealth()));
	playerStamina->SetIndex(static_cast<float>(player->GetStamina()));
#endif
}

void PlayerUI::DrawGUI()
{
	// パラメータ
	if (ImGui::CollapsingHeader("UI", ImGuiTreeNodeFlags_DefaultOpen))
	{
#pragma region HP
		DirectX::XMFLOAT2 HPPosition = playerHP->GetPosition();
		if(ImGui::DragFloat2("HPPosition", &HPPosition.x, 1.0f, 0.0f, 1920.0f, "%.2f"))
		{
			playerHP->SetPosition(HPPosition);
		}

		HPPosition = playerGaugeBox[0]->GetPosition();
		if (ImGui::DragFloat2("HPBoxPosition", &HPPosition.x, 1.0f, 0.0f, 1920.0f, "%.2f"))
		{
			playerGaugeBox[0]->SetPosition(HPPosition);
		}
#pragma endregion
	
#pragma region スタミナ
		DirectX::XMFLOAT2 staminaPosition = playerStamina->GetPosition();
		if(ImGui::DragFloat2("StaminaPosition", &staminaPosition.x, 1.0f, 0.0f, 1920.0f, "%.2f"))
		{
			playerStamina->SetPosition(staminaPosition);
		}

		staminaPosition = playerGaugeBox[1]->GetPosition();
		if (ImGui::DragFloat2("StaminaBoxPosition", &staminaPosition.x, 1.0f, 0.0f, 1920.0f, "%.2f"))
		{
			playerGaugeBox[1]->SetPosition(staminaPosition);
		}
#pragma endregion

#pragma region アイコン
		DirectX::XMFLOAT2 iconPosition = playerIcon->GetPosition();
		if (ImGui::DragFloat2("IconPosition", &iconPosition.x, 1.0f, 0.0f, 1920.0f, "%.2f"))
		{
			playerIcon->SetPosition(iconPosition);
		}

		// スケール
		if (ImGui::InputFloat("UIScale", &uiScale.x))
		{
			uiScale.y = uiScale.x;

			playerHP->SetScale(uiScale);
			playerGaugeBox[0]->SetScale(uiScale);
			playerStamina->SetScale(uiScale);
			playerGaugeBox[1]->SetScale(uiScale);
		}

		if (ImGui::InputFloat("IconScale", &iconScale.x))
		{
			iconScale.y = iconScale.x;
			playerIcon->SetScale(iconScale);
		}

		// パラメータの保存
		if (ImGui::Button("Save"))
		{
			SaveUIParam();
		}
	}
}

void PlayerUI::SaveUIParam()
{
	auto SaveParam = [this](GaugeParam& param, UIBase* base)
		{
			param.position = base->GetPosition();
			param.scale = base->GetScale();
		};

	// HP
	SaveParam(HPParam, playerHP.get());
	SaveParam(HPBoxParam, playerGaugeBox[0].get());

	// スタミナ
	SaveParam(StaminaParam, playerStamina.get());
	SaveParam(StaminaBoxParam, playerGaugeBox[1].get());

	// アイコン
	SaveParam(IconParam, playerIcon.get());

	std::ofstream ofs("Data/Json/UIParamData.json", std::ios::out);
	cereal::JSONOutputArchive archive(ofs);

	if (ofs.is_open())
	{
		archive(
			CEREAL_NVP(HPParam),
			CEREAL_NVP(HPBoxParam),
			CEREAL_NVP(StaminaParam),
			CEREAL_NVP(StaminaBoxParam),
			CEREAL_NVP(IconParam)
		);
	}
}

void PlayerUI::LoadUIParam()
{
	std::ifstream ifs("Data/Json/UIParamData.json", std::ios::in);
	cereal::JSONInputArchive archive(ifs);

	if (ifs.is_open())
	{
		archive(
			CEREAL_NVP(HPParam),
			CEREAL_NVP(HPBoxParam),
			CEREAL_NVP(StaminaParam),
			CEREAL_NVP(StaminaBoxParam),
			CEREAL_NVP(IconParam)
		);
	}
}
