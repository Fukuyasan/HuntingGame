#pragma once
#include "UI\UIBase.h"

class UIButton : public UIBase
{
public:
	UIButton(const char* filename) : UIBase(filename) {}
	~UIButton() {}

	void DrawDebugGUI() override;
};

