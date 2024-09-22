#pragma once
#include <vector>
#include "UI\UIBase.h"

class UIManager final
{
private:
	UIManager() {}
	~UIManager() {}

public:
	UIManager(UIManager&&)		= delete;
	UIManager(const UIManager&) = delete;
	UIManager operator=(UIManager&&)	  = delete;
	UIManager operator=(const UIManager&) = delete;

public:
	// インスタンス取得
	static UIManager& Instance()
	{
		static UIManager instance;
		return instance;
	}

	void RegisterUI(UIBase* ui);

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, SpriteShader* shader);
	void DrawDebugGUI();

	void Clear();

private:
	std::vector<UIBase*> UIPool;
};