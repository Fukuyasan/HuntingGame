#include "NowLoading.h"
#include "UI/UIManager.h"

NowLoading::NowLoading(const float texNum, const float count)
{
	const float texSize = 64;

	// �摜�̓ǂݍ��݁A�ݒ�
	loadingWords = std::make_unique<UIButton>("Data/Sprite/NowLoading.png");
	loadingWords->SetTexPos({  texSize * texNum, 0 });
	loadingWords->SetTexSize({ texSize, texSize });
	loadingWords->SetPosition({ 1200 + (texSize * texNum), 900 });
	
	// �x���W�̏����ʒu��ݒ�
	fastPositionY = loadingWords->GetPosition().y;

	// �^�C�}�[�ݒ�
	maxtimer = count;
}

void NowLoading::Update(const float& elapsedTime)
{
	loadingWords->Update(elapsedTime);	

	// 
	maxtimer -= elapsedTime;
	if (maxtimer > 0) return;

	sinTimer += elapsedTime;

	// ���n�����Ƃ��̏d�݂��Č�����������(std::min)���g���ċ����I�Ɏ~�߂Ă���
	loadingWords->SetPositionY((std::min)(fastPositionY, fastPositionY - (moveSpeed * sinf(sinSpeed * sinTimer))));	
}

void NowLoading::Render(ID3D11DeviceContext* dc, SpriteShader* shader)
{
	loadingWords->Render(dc, shader);
}
