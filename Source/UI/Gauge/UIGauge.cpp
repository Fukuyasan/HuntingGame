#include "UIGauge.h"
#include "System\Math\Mathf.h"

void UIGauge::Update(const float& elapsedTime)
{
	// �ω��l���}�C�i�X�ɍs���Ȃ��悤�ɕ␳
	index = Mathf::Clamp(index, 0.0f, max);

	const float width  = static_cast<float>(sprite->GetTextureWidth());
	const float height = static_cast<float>(sprite->GetTextureHeight());

	// �Q�[�W�̔{�������߂�
	const float rate = index / max;

	SpriteObject::SetTexSize({ {rate * width}, height });
	SpriteObject::Update(elapsedTime);
}

void UIGauge::DrawDebugGUI()
{

}