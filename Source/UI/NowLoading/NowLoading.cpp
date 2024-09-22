#include "NowLoading.h"
#include "UI/UIManager.h"

NowLoading::NowLoading(const float texNum, const float count)
{
	const float texSize = 64;

	// 画像の読み込み、設定
	loadingWords = std::make_unique<UIButton>("Data/Sprite/NowLoading.png");
	loadingWords->SetTexPos({  texSize * texNum, 0 });
	loadingWords->SetTexSize({ texSize, texSize });
	loadingWords->SetPosition({ 1200 + (texSize * texNum), 900 });
	
	// Ｙ座標の初期位置を設定
	fastPositionY = loadingWords->GetPosition().y;

	// タイマー設定
	maxtimer = count;
}

void NowLoading::Update(const float& elapsedTime)
{
	loadingWords->Update(elapsedTime);	

	// 
	maxtimer -= elapsedTime;
	if (maxtimer > 0) return;

	sinTimer += elapsedTime;

	// 着地したときの重みを再現したいため(std::min)を使って強制的に止めている
	loadingWords->SetPositionY((std::min)(fastPositionY, fastPositionY - (moveSpeed * sinf(sinSpeed * sinTimer))));	
}

void NowLoading::Render(ID3D11DeviceContext* dc, SpriteShader* shader)
{
	loadingWords->Render(dc, shader);
}
