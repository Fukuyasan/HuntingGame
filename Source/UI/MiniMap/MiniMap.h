#pragma once
#include "UI/Button/UIButton.h"

#ifdef DEBUG

class MiniMap
{
public:
	MiniMap();
	~MiniMap() {}

	void Update(const float& elapsedTime);
	void DrawGUI();

private:
	std::unique_ptr<UIButton> miniMap	 = nullptr;
	std::unique_ptr<UIButton> miniMapBox = nullptr;

	std::unique_ptr<UIButton> playerIcon  = nullptr;
	std::unique_ptr<UIButton> EnemyIcon   = nullptr;

	DirectX::XMFLOAT2 maxWorldPosition = { 300, 300 };

private:
	DirectX::XMFLOAT2 miniMapPos = { 100.0f, 600.0f };
	DirectX::XMFLOAT2 boxOffset  = {  12.5f,  12.5f };
	DirectX::XMFLOAT2 mapScale   = {   0.6f,   0.6f };

};

#endif // DEBUG