#pragma once
#include "UI/Button/UIButton.h"

// NOWLOADING : �ꕶ��
class NowLoading
{
public:
	NowLoading(const float texNum, const float count);
	~NowLoading() {}

	void Update(const float& elapsedTime);
	void Render(ID3D11DeviceContext* dc, SpriteShader* shader);

private:
	std::unique_ptr<UIButton> loadingWords;

	// �����ʒu(�x���W)
	float fastPositionY = 0;

	// 
	float maxtimer = 0;

	float moveSpeed = 100.0f;
	float sinSpeed  = 3.0f;
	float sinTimer  = 0;
};