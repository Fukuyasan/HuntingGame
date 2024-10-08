#pragma once
#include "UI/Button/UIButton.h"

// NOWLOADING : 一文字
class NowLoading
{
public:
	NowLoading(const float texNum, const float count);
	~NowLoading() {}

	void Update(const float& elapsedTime);
	void Render(ID3D11DeviceContext* dc, SpriteShader* shader);

private:
	std::unique_ptr<UIButton> loadingWords;

	// 初期位置(Ｙ座標)
	float fastPositionY = 0;

	// 
	float maxtimer = 0;

	float moveSpeed = 100.0f;
	float sinSpeed  = 3.0f;
	float sinTimer  = 0;
};