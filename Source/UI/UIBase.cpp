#include "UIBase.h"

UIBase::UIBase(const char* filename)
{
	SetSprite(filename);

	if (!sprite) return;

	const float width  = static_cast<float>(sprite->GetTextureWidth());
	const float height = static_cast<float>(sprite->GetTextureHeight());

	SpriteObject::SetTexSize({ width, height });
}
