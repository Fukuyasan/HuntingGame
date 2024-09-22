#pragma once
#include <vector>
#include <set>
#include "Enemy.h"
 
#ifdef DEBUG

class EnemyManager final
{
private:
    EnemyManager()  = default;
    ~EnemyManager() = default;

public:
    EnemyManager(EnemyManager&&)                = delete;
    EnemyManager(const EnemyManager&)           = delete;
    EnemyManager operator=(EnemyManager&&)      = delete;
    EnemyManager operator=(const EnemyManager&) = delete;

public:
    // インスタンス取得
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    // 更新処理
    void Update(const float& elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // エネミー登録
    void Register(Enemy* enemy);

    // エネミー削除
    void Remove(Enemy* enemy);

    // エネミー全削除
    void Clear();

    // デバッグ描画
    void DrawDebugPrimitive();
    void DrawDebugGUI();

    // エネミー数取得
    int GetEnemyCount() const { return static_cast<int>(m_enemyPool.size()); }

    // エネミー取得
    Enemy* GetEnemy(int index) { return m_enemyPool[index]; }

private:
    std::vector<Enemy*> m_enemyPool;
    std::set<Enemy*>    m_removePool;
};

#endif // DEBUG