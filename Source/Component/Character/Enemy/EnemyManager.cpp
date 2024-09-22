#include "EnemyManager.h"
#include "System/Collision/Collision.h"

#ifdef DEBUG

// 更新処理
void EnemyManager::Update(const float& elapsedTime)
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->Update(elapsedTime);
    }

    // 破棄処理
    for (auto& enemy : m_removePool)
    {
        // std::vectorから要素を削除する場合はイテレーターで削除しなければならない
        auto it = std::find(m_enemyPool.begin(), m_enemyPool.end(), enemy);

        // 敵が存在していれば破棄
        if (it != m_enemyPool.end())
        {
            delete enemy;
            m_enemyPool.erase(it);
        }
    }
    m_removePool.clear();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->Render(dc, shader);
    }
}

// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
    m_enemyPool.emplace_back(enemy);
}

// エネミー削除
void EnemyManager::Remove(Enemy* enemy)
{
    // 破棄リストに追加 
    m_removePool.insert(enemy);
}

// エネミー全削除
void EnemyManager::Clear()
{
    for (auto& enemy : m_enemyPool)
    {
        delete enemy;
    }
    m_enemyPool.clear();
}

// デバッグプリミティブ描画
void EnemyManager::DrawDebugPrimitive()
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->DrawDebugPrimitive();
    }
}

// IMGUi描画
void EnemyManager::DrawDebugGUI()
{
    for (auto& enemy : m_enemyPool)
    {
        enemy->DrawDebugGUI();
    }
}

#endif // DEBUG