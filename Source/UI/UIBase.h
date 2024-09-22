#pragma once
#include "SpriteObject\SpriteObject.h"

// UI�̊��N���X
class UIBase : public SpriteObject
{
public:
	UIBase() {}
	UIBase(const char* filename);
	virtual ~UIBase() = default;

	virtual void DrawDebugGUI() {};

#pragma region �����x�̃Z�b�^�[ �Q�b�^�[
	void SetAlpha(float alpha) { this->color.w = alpha; }

	void AddAlpha(float alpha, float maxAlpha = 1.0f) {
		if (this->color.w <= maxAlpha)
			this->color.w += alpha;
	}

	float GetAlpha() { return color.w; }
#pragma endregion
};