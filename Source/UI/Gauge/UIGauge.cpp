#include "UIGauge.h"
#include "System\Math\Mathf.h"

void UIGauge::Update(const float& elapsedTime)
{
	// 変化値がマイナスに行かないように補正
	index = Mathf::Clamp(index, 0.0f, max);

	const float width  = static_cast<float>(sprite->GetTextureWidth());
	const float height = static_cast<float>(sprite->GetTextureHeight());

	// ゲージの倍率を求める
	const float rate = index / max;

	SpriteObject::SetTexSize({ {rate * width}, height });
	SpriteObject::Update(elapsedTime);
}

void UIGauge::DrawDebugGUI()
{

}