#include "Otomo.h"
#include "GameObject/GameObjectManager.h"

void Otomo::Awake()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

#pragma region ステータス設定
    // 初期位置
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("Player");
    gameObject->transform.SetPosition(player->GetTransform().GetPosition());

    // スケール
    gameObject->transform.SetScale(0.012f);

    // 高さ
    height = 0.3f;

    // HP
    health = maxHealth = 150;
#pragma endregion
}

void Otomo::Start()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

    // コンポーネント取得
    movement = gameObject->GetComponent<Movement>();
    animator = gameObject->GetComponent<Animator>();

    // エフェクト
    //smokeEffect = std::make_unique<Effect>("Data/Effect/smokeWalk.efkefc");

    // アニメーションのイベントデータ
    eventInfo.Deserialize("Data/Model/SDUnityChan/SDUnityChan.mdl/SDUnityChan.event");

    otomoTree = std::make_unique<OtomoBehaviorTree>();
    otomoTree->RegisterBehaviorTree(this);
}

void Otomo::Update()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();
    float elapsedTime = TimeManager::Instance().GetDeltaTime();

    otomoTree->UpdateBehaviorTree(elapsedTime);

    // 速力処理更新
    movement->UpdateVelocity(elapsedTime);

    gameObject->transform.m_position.y = (std::max)(gameObject->transform.m_position.y, 0.0f);

    // オブジェクト行列を更新
    gameObject->transform.UpdateTransform();

    // モデルアニメーション更新
    animator->UpdateAnimation(elapsedTime);
    animator->UpdateRootMotion(gameObject->transform);

    // モデル行列更新
    gameObject->GetModel()->UpdateTransform(gameObject->transform.m_transform);
}
