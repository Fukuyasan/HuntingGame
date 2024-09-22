#pragma once
#include <DirectXMath.h>
#include "Graphics/Sprite.h"
#include "Graphics/Shader/SpriteShader/DefaultSpriteShader.h"

// 中心点の設定
enum class Center
{
    Zero = 0,  // (0,0)の位置に中心点
    Middle     // 画像の真ん中の位置に中心点
};

class SpriteObject
{
public:
    SpriteObject()          = default;
    virtual ~SpriteObject() = default;

    virtual void Update(const float& elapsedTime);
    virtual void Render(ID3D11DeviceContext* dc, SpriteShader* shader);

public:
#pragma region セッター ゲッター
    void SetPosition(const DirectX::XMFLOAT2& position) { this->position = position; }
    void SetPositionX(const float position) { this->position.x = position; }
    void SetPositionY(const float position) { this->position.y = position; }
    const DirectX::XMFLOAT2& GetPosition() { return position; }

    void SetScale(const float scale) { this->scale.x = this->scale.y = scale; }
    void SetScale(const DirectX::XMFLOAT2& scale) { this->scale = scale; }
    const DirectX::XMFLOAT2& GetScale() { return scale; }

    void SetTexPos(const DirectX::XMFLOAT2& position) { this->texPos = position; }
    const DirectX::XMFLOAT2& GetTexPos() { return texPos; }
   
    void SetTexSize(const DirectX::XMFLOAT2& size) { this->texSize = size; }
    const DirectX::XMFLOAT2& GetTexSize() { return texSize; }

    void SetCenter(Center center);
    void SetCenter(const DirectX::XMFLOAT2& center) { this->center = center; }
    const DirectX::XMFLOAT2& GetCenter() { return center; }

    void SetAngle(const float angle) { this->angle = angle; }
    const float GetAngle() { return angle; }

    void SetColor(const DirectX::XMFLOAT4& color) { this->color = color; }
    const DirectX::XMFLOAT4& GetColor() { return color; }

    void SetSprite(const char* filePath) { this->sprite = std::make_unique<Sprite>(filePath); }
    Sprite* GetSprite() { return sprite.get(); }
#pragma endregion

protected:
    std::unique_ptr<Sprite> sprite = nullptr;

    DirectX::XMFLOAT2 position = { 0,0 };
    DirectX::XMFLOAT2 scale    = { 1,1 };

    DirectX::XMFLOAT2 texPos   = { 0,0 };
    DirectX::XMFLOAT2 texSize  = { 0,0 };

    DirectX::XMFLOAT2 center   = { 0,0 };

    float angle                = 0;
    DirectX::XMFLOAT4 color    = { 1,1,1,1 };
};