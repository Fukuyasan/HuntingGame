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
    // �C���X�^���X�擾
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    // �X�V����
    void Update(const float& elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // �G�l�~�[�o�^
    void Register(Enemy* enemy);

    // �G�l�~�[�폜
    void Remove(Enemy* enemy);

    // �G�l�~�[�S�폜
    void Clear();

    // �f�o�b�O�`��
    void DrawDebugPrimitive();
    void DrawDebugGUI();

    // �G�l�~�[���擾
    int GetEnemyCount() const { return static_cast<int>(m_enemyPool.size()); }

    // �G�l�~�[�擾
    Enemy* GetEnemy(int index) { return m_enemyPool[index]; }

private:
    std::vector<Enemy*> m_enemyPool;
    std::set<Enemy*>    m_removePool;
};

#endif // DEBUG