#include "SpriteObject.h"

void SpriteObject::Update(const float& elapsedTime)
{
	if (sprite) sprite->Update(position, scale, texPos, texSize, center, angle, color);
}

void SpriteObject::Render(ID3D11DeviceContext* dc, SpriteShader* shader)
{
	if (sprite) shader->Draw(dc, sprite.get());
}

void SpriteObject::SetCenter(Center center)
{
	switch (center)
	{
	case Center::Zero:
		this->center = { 0,0 };
		break;
	case Center::Middle:
		// テクスチャの幅、高さから中心点を求める
		DirectX::XMFLOAT2 centerTexPos{};
		centerTexPos.x = sprite->GetTextureWidth()  * 0.5f;
		centerTexPos.y = sprite->GetTextureHeight() * 0.5f;

		this->center = centerTexPos;
		break;
	}
}
