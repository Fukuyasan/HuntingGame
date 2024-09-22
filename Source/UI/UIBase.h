#pragma once
#include "SpriteObject\SpriteObject.h"

// UIの基底クラス
class UIBase : public SpriteObject
{
public:
	UIBase() {}
	UIBase(const char* filename);
	virtual ~UIBase() = default;

	virtual void DrawDebugGUI() {};

#pragma region 透明度のセッター ゲッター
	void SetAlpha(float alpha) { this->color.w = alpha; }

	void AddAlpha(float alpha, float maxAlpha = 1.0f) {
		if (this->color.w <= maxAlpha)
			this->color.w += alpha;
	}

	float GetAlpha() { return color.w; }
#pragma endregion
};