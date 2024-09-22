#include "ProjectileManager.h"

// 更新処理
void ProjectileManager::Update(const float& elapsedTime)
{
    // 更新処理
    for (Projectile* projectile : projectiles)
    {
        projectile->Update(elapsedTime);
    }
    
    // 破棄処理
    Destruction();
}

// 描画処理
void ProjectileManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    // 描画処理
    for (Projectile* projectile : projectiles)
    {
        projectile->Render(context, shader);
    }
}

// デバッグプリミティブ描画
void ProjectileManager::DrawDebugPrimitive()
{
    // デバッグプリミティブ描画
    for (Projectile* projectile : projectiles)
    {
        projectile->DrawDebugPrimitive();
    }
}

// 弾丸登録
void ProjectileManager::Register(Projectile* projectile)
{
    // 弾丸登録
    projectiles.emplace_back(projectile);
}

// 弾丸全削除
void ProjectileManager::Clear()
{
    // 弾丸全削除
    for (Projectile* projectile : projectiles)
    {
        delete projectile;
    }
    projectiles.clear();
}

// 弾丸削除
void ProjectileManager::Remove(Projectile* projectile)
{
    // 破棄リストに追加
    removes.insert(projectile);

    // 直接、projectilesの要素を削除してしまうと
    // 範囲for文で不具合を起こすため、
    // 破棄リストに追加する
}

void ProjectileManager::Destruction()
{
    // 破棄処理
    // ※ projectilesの範囲for文中でerase()すると不具合が発生してしまうため、
    //    更新処理が終わった後に破棄リストの積まれたオブジェクトを削除する。
    for (Projectile* projectile : removes)
    {
        // std::vectorから要素を削除する場合はイテレーターで削除しなければならない
        std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);

        // std::vectorで管理されている要素を削除するにはerase()関数を使用する。
        // 破棄リストのポインタからイテレーターを検索し、erase()関数に渡す。

        if (it != projectiles.end())
        {
            projectiles.erase(it);
        }

        // 弾丸の破棄処理
        delete projectile;
    }
    // 破棄リストをクリア
    removes.clear();
}
