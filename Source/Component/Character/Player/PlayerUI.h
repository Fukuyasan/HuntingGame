#pragma once
#include <memory>
#include <vector>

#include "UI/Gauge/UIGauge.h"
#include "UI/Button/UIButton.h"

class PlayerUI
{
public:
	struct GaugeParam
	{
		DirectX::XMFLOAT2 position = { 0,0 };
		DirectX::XMFLOAT2 scale	   = { 1,1 };

		template <class Archive>
		void serialize(Archive& archive);
	};

public:
	void Initialize();
	void Update(float elapsedTime);
	void DrawGUI();

private:
	void SaveUIParam();
	void LoadUIParam();

private:
	// UI
	std::unique_ptr<UIGauge>  playerHP          = nullptr;
	std::unique_ptr<UIGauge>  playerStamina     = nullptr;
	std::unique_ptr<UIButton> playerGaugeBox[2] = {};
	std::unique_ptr<UIButton> playerIcon        = nullptr;

private:
	GaugeParam HPParam;
	GaugeParam HPBoxParam;
	GaugeParam StaminaParam;
	GaugeParam StaminaBoxParam;
	GaugeParam IconParam;

	DirectX::XMFLOAT2 uiScale;
	DirectX::XMFLOAT2 iconScale;
};