#pragma once

#include <vector>
#include <set>

#include "Projectile.h"

// 弾丸マネージャー
class ProjectileManager final 
{
private:
    ProjectileManager()  = default;  // コンストラクタ
    ~ProjectileManager() = default;  // デストラクタ

public:
    // ムーブコンストラクタ、コピーコンストラクタは delete 指定しておく
    ProjectileManager(ProjectileManager&&)      = delete;
    ProjectileManager(const ProjectileManager&) = delete;
    // ムーブ代入演算子、コピー代入演算子も delete 指定しておく
    ProjectileManager operator= (ProjectileManager&&)      = delete;
    ProjectileManager operator= (const ProjectileManager&) = delete;

public:
    // シングルトンにはしないといったな、仕様変更だ。
    static ProjectileManager& Instance()
    {
        static ProjectileManager instance;
        return instance;
    }

    // 更新処理
    void Update(const float& elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* context, Shader* shader);

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // 弾丸登録
    void Register(Projectile* projectile);

    // 弾丸全削除
    void Clear();
    
    // 弾丸削除
    void Remove(Projectile* projectile);

#pragma region ゲッター
    // 弾丸数取得
    int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

    // 弾丸取得
    Projectile* GetProjectile(int index) { return projectiles.at(index); }  
#pragma endregion
private:
    // 破棄処理
    void Destruction();

private:
    std::vector<Projectile*> projectiles;
    std::set<Projectile*>    removes;

    // ※ std::setは同じ要素が重複しない
    //    std::vectorみたいなもの。
};
