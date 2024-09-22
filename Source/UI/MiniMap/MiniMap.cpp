#include "MiniMap.h"
#include "UI/UIManager.h"
#include "Component/Character/Player/player.h"
#include "Component/Character/Enemy/EnemyManager.h"

#include <imgui.h>

#ifdef DEBUG

MiniMap::MiniMap()
{
	// �}�b�v
	{
		// �g
		miniMapBox = std::make_unique<UIButton>("Data/Sprite/miniMapBox.png");
		miniMapBox->SetPosition(miniMapPos);
		miniMapBox->SetScale(mapScale);

		// �}�b�v
		const float mapAlpha = 0.8f;
		miniMap = std::make_unique<UIButton>("Data/Sprite/miniMap.png");
		
		miniMapPos.x += boxOffset.x;
		miniMapPos.y += boxOffset.y;
		
		miniMap->SetPosition(miniMapPos);
		miniMap->SetScale(mapScale);
		miniMap->SetAlpha(mapAlpha);

		UIManager::Instance().RegisterUI(miniMapBox.get());
		UIManager::Instance().RegisterUI(miniMap.get());
	}

	// �h���S���̃A�C�R��
	{
		EnemyIcon = std::make_unique<UIButton>("Data/Sprite/DragonLogo.png");
		EnemyIcon->SetCenter({ 110, 100 });
		EnemyIcon->SetScale(0.3f);
		UIManager::Instance().RegisterUI(EnemyIcon.get());
	}

	// �v���C���[�̃A�C�R��
	{
		playerIcon = std::make_unique<UIButton>(nullptr);
		playerIcon->SetTexSize({ 20, 20 });
		playerIcon->SetCenter({ 10, 10 });
		playerIcon->SetColor({ 0, 0, 1, 1 });
		UIManager::Instance().RegisterUI(playerIcon.get());
	}
}

void MiniMap::Update(const float& elapsedTime)
{
	// ���[���h���W���擾
	DirectX::XMFLOAT2 playerPosition = { Player::Instance().transform.GetPosition().x, Player::Instance().transform.GetPosition().z };

	Enemy* dragon = EnemyManager::Instance().GetEnemy(0);
	DirectX::XMFLOAT2 dragonPosition = { dragon->transform.GetPosition().x, dragon->transform.GetPosition().z };

	// ���[���h���W���}�b�v�̃X�N���[�����W�ɕϊ�
	auto convertMiniMap = [this](const DirectX::XMFLOAT2& worldPosition) -> DirectX::XMFLOAT2
	{
		// �~�j�}�b�v���
		const DirectX::XMFLOAT2 mapPos     = miniMap->GetPosition();
		const DirectX::XMFLOAT2 mapCenter  = miniMap->GetCenter();
		const DirectX::XMFLOAT2 mapTexSize = miniMap->GetTexSize();
		const float				mapScale   = miniMap->GetScale().x;

		// �v���C���[�̈ʒu�� 1 ~ 0 �̋�Ԃɕϊ�
		DirectX::XMFLOAT2 texPosition = { (1 - (worldPosition.x / maxWorldPosition.x)), ((worldPosition.y / maxWorldPosition.y)) };

		texPosition.x *= mapTexSize.x;
		texPosition.y *= mapTexSize.y;

		DirectX::XMFLOAT2 miniMapPos{};
		// �}�b�v�̒[ : (mapPos - mapCenter)
		miniMapPos.x = (mapPos.x - mapCenter.x) + texPosition.x * mapScale;
		miniMapPos.y = (mapPos.y - mapCenter.y) + texPosition.y * mapScale;

		return miniMapPos;
	};

	DirectX::XMFLOAT2 playerMiniMapPosition = convertMiniMap(playerPosition);
	DirectX::XMFLOAT2 dragonMiniMapPosition = convertMiniMap(dragonPosition);

	playerIcon->SetPosition(playerMiniMapPosition);
	EnemyIcon->SetPosition(dragonMiniMapPosition);
}

void MiniMap::DrawGUI()
{
	// �p�����[�^
	if (ImGui::CollapsingHeader("MiniMap", ImGuiTreeNodeFlags_DefaultOpen))
	{

		// �ʒu
		if (ImGui::InputFloat2("MapPosition", &miniMapPos.x))
		{
			miniMap->SetPosition(miniMapPos);

			miniMapPos.x += boxOffset.x;
			miniMapPos.y += boxOffset.y;
			miniMapBox->SetPosition(miniMapPos);
		}

		// �X�P�[��
		if (ImGui::InputFloat("MapScale", &mapScale.x))
		{
			mapScale.y = mapScale.x;

			miniMap->SetScale(mapScale);
			miniMapBox->SetScale(mapScale);
		}		
	}
}

#endif // DEBUG