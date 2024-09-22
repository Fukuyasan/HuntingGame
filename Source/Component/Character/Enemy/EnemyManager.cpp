#include "EnemyManager.h"
#include "System/Collision/Collision.h"

#ifdef DEBUG

// �X�V����
void EnemyManager::Update(const float& elapsedTime)
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->Update(elapsedTime);
    }

    // �j������
    for (auto& enemy : m_removePool)
    {
        // std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
        auto it = std::find(m_enemyPool.begin(), m_enemyPool.end(), enemy);

        // �G�����݂��Ă���Δj��
        if (it != m_enemyPool.end())
        {
            delete enemy;
            m_enemyPool.erase(it);
        }
    }
    m_removePool.clear();
}

// �`�揈��
void EnemyManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->Render(dc, shader);
    }
}

// �G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
    m_enemyPool.emplace_back(enemy);
}

// �G�l�~�[�폜
void EnemyManager::Remove(Enemy* enemy)
{
    // �j�����X�g�ɒǉ� 
    m_removePool.insert(enemy);
}

// �G�l�~�[�S�폜
void EnemyManager::Clear()
{
    for (auto& enemy : m_enemyPool)
    {
        delete enemy;
    }
    m_enemyPool.clear();
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager::DrawDebugPrimitive()
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->DrawDebugPrimitive();
    }
}

// IMGUi�`��
void EnemyManager::DrawDebugGUI()
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->DrawDebugGUI();
    }
}

#endif // DEBUG