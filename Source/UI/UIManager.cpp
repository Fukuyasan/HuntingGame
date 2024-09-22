#include "UIManager.h"

void UIManager::RegisterUI(UIBase* ui)
{
	UIPool.emplace_back(ui);
}

void UIManager::Update(float elapsedTime)
{
	for (UIBase* ui : UIPool)
	{
		ui->Update(elapsedTime);
	}
}

void UIManager::Render(ID3D11DeviceContext* dc, SpriteShader* shader)
{
	for (UIBase* ui : UIPool)
	{
		ui->Render(dc, shader);
	}
}

void UIManager::DrawDebugGUI()
{
	for (UIBase* ui : UIPool)
	{
		ui->DrawDebugGUI();
	}
}

void UIManager::Clear()
{
	/*for (UIBase* ui : UIPool)
	{
		if (ui) delete ui;
	}*/
	UIPool.clear();
}
