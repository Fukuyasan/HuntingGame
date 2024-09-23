#include "player.h"
#include "PlayerState.h"

#include <algorithm>
#include <imgui.h>

#include "Graphics/Graphics.h"
#include "Graphics/ShaderState.h"

#include "AI/MetaAI/MetaAI.h"

#include "System/Math/Easing.h"
#include "System/Input/Input.h"
#include "System/Collision/Collision.h"

#include "Camera/Camera.h"

#include "Scene/SceneGame.h"

void Player::Awake()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

#pragma region ステータス設定
    // 初期位置
    areaNum = 1;
    DirectX::XMFLOAT3 firstPos = MetaAI::Instance().GetAreaData(areaNum).areaPos;
    gameObject->transform.SetPosition(firstPos);

    // スケール
    gameObject->transform.SetScale(0.012f);

    // 高さ
    height = 1.3f;

    // HP
    health = maxHealth = 150;

    // スタミナ
    stamina = maxStamina = 150.0f;
#pragma endregion
}

void Player::Start()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

    // コンポーネント取得
    movement = gameObject->GetComponent<Movement>();
    animator = gameObject->GetComponent<Animator>();

    // アニメーションのイベントデータ
    eventInfo.Deserialize("Data/Model/Player/HunterTest.event");

    // ステートマシーン設定
    RegisterStateMachine();

    // コントローラー
    controller = std::make_unique<PlayerContorller>(0);

    // ヒットエフェクト
    //hitEffect[HitEffect::Weak]  = std::make_unique<Effect>("Data/Effect/Hit.efk");
    //hitEffect[HitEffect::Clack] = std::make_unique<Effect>("Data/Effect/Blow11.efk");
    //hitEffect[HitEffect::Hard]  = std::make_unique<Effect>("Data/Effect/HitWeakPoint.efk");

    // UI初期化
    //ui.Initialize();
}

void Player::Update()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();
    float elapsedTime = TimeManager::Instance().GetDeltaTime();

    // コントローラー更新
    controller->Update();

    // スタミナ回復
    stamina += recoveryStamina * elapsedTime;
    stamina = Mathf::Clamp(stamina, 0.0f, maxStamina);

    // UI更新
    //ui.Update(elapsedTime);

    // ヒットストップ更新
    float time = elapsedTime;

    // 今回ヒットストップは時間を止めるタイプなので
    //  bool の 1 : 0 の特性を使って実装している。
    time *= HitStop();

    // ステートマシーン更新
    stateMachine->Update(time);

    // クールタイム更新
    UpdateCoolTimer(time);

    // プレイヤーとエネミーの押し出し処理
    CollisionPlayerVsEnemies();

    // 速力処理更新
    movement->UpdateVelocity(time);

    // 無敵時間更新
    UpdateInvincibleTimer(time);

    gameObject->transform.m_position.y = (std::max)(gameObject->transform.m_position.y, 0.0f);

    // オブジェクト行列を更新
    gameObject->transform.UpdateTransform();

    // モデルアニメーション更新
    animator->UpdateAnimation(time);
    animator->UpdateRootMotion(gameObject->transform);

    // モデル行列更新
    gameObject->GetModel()->UpdateTransform(gameObject->transform.m_transform);
}


// プレイヤーのステートマシンを設定
void Player::RegisterStateMachine()
{
    stateMachine = std::make_unique<StateMachine<Player>>();

    stateMachine->RegisterState(std::make_shared<IdleState>(this));
    stateMachine->RegisterState(std::make_shared<WalkState>(this));
    stateMachine->RegisterState(std::make_shared<RunState>(this));
    stateMachine->RegisterState(std::make_shared<DashState>(this));
    stateMachine->RegisterState(std::make_shared<AvoidState>(this));

    //  弱攻撃ステート
    stateMachine->RegisterState(std::make_shared<ComboA_1State>(this));
    stateMachine->RegisterState(std::make_shared<ComboA_2State>(this));
    stateMachine->RegisterState(std::make_shared<ComboA_3State>(this));

    //   強攻撃ステート
    stateMachine->RegisterState(std::make_shared<ComboB_1State>(this));
    stateMachine->RegisterState(std::make_shared<ComboB_2State>(this));
    stateMachine->RegisterState(std::make_shared<ComboB_3State>(this));
    stateMachine->RegisterState(std::make_shared<ComboB_4State>(this));

    stateMachine->RegisterState(std::make_shared<Attack_DashState>(this));

    stateMachine->RegisterState(std::make_shared<Counter_StartState>(this));
    stateMachine->RegisterState(std::make_shared<Counter_AttackState>(this));
    stateMachine->RegisterState(std::make_shared<Counter_EndState>(this));

    stateMachine->RegisterState(std::make_shared<DamageState>(this));
    stateMachine->RegisterState(std::make_shared<StandUpState>(this));
    stateMachine->RegisterState(std::make_shared<DeathState>(this));

    // 始めのステートを設定
    stateMachine->SetState(static_cast<int>(State::State_Idle));
}

// 回避の方向を設定
void Player::SetAvoidVec()
{
    // スティックの入力がなければプレイヤーの前ベクトルを回避方向に設定
    avoidVec = controller->GetMoveVec();

    float lengthSq = avoidVec.x * avoidVec.x + avoidVec.z * avoidVec.z;    
    if (lengthSq == 0.0f)
    {
        avoidVec = gameObject.lock()->transform.GetForward();
    }

    // 回避の無敵時間設定
    invincibleFrame = 10.0f;

    // スタミナの消費
    stamina -= consumeAvoid;
}

// 回避処理
void Player::PlayAvoid(const float& elapsedTime)
{
#if 0
    float currentTime = gameObject.lock()->GetAnimator()->GetAnimationSeconds();
    float totalTime   = animator->GetAnimationLength();

    // 回避のスピードをイージングを使っていい感じにする
    avoidSpeed *= Easing::OutQuart(totalTime - currentTime, totalTime);

    movement->Move(avoidVec.x, avoidVec.z, avoidSpeed);
#endif
    movement->Turn(elapsedTime, avoidVec.x, avoidVec.z, turnSpeed);
}

#pragma region デバッグ
// デバッグ用GUI描画
void Player::OnDebugGUI()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // パラメータ
        if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputInt("HP", &health);
            ImGui::InputFloat("coolTime", &coolTime);
            ImGui::InputFloat("avoidSpeed", &avoidSpeed);
        }

        if (ImGui::CollapsingHeader("EventData", ImGuiTreeNodeFlags_DefaultOpen))
        {
            AttackData attackData = eventInfo.GetAttackData();

            ImGui::InputFloat("attackPower", &attackData.attackPower);
            ImGui::InputInt("hitStopFrame", &attackData.hitStopFrame);
            ImGui::InputFloat("invisivleTime", &attackData.invincibleTime);

        }

        //ui.DrawGUI();
    }
}

// デバッグ用プリミティブ描画
void Player::OnDebugPrimitive()
{
    DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

#if 1
    // プレイヤーの頭位置取得
    DirectX::XMFLOAT3 head = gameObject.lock()->transform.GetPosition();
    head.y += height;

    // カプセル描画
    debugRenderer->AddCapsule(
        gameObject.lock()->transform.GetPosition(),
        head,
        radius,
        DirectX::XMFLOAT4(0, 0, 0, 1)
    );
#endif
}
#pragma endregion

// *********************
//                      
//      入力処理
//                      
// *********************

// 移動入力処理
void Player::InputMove(const float& elapsedTime)
{
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = controller->GetMoveVec();

    // 移動処理
    movement->Move(moveVec.x, moveVec.z, movement->GetMaxMoveSpeed());

    // 旋回処理
    movement->Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

// ダッシュ入力処理
bool Player::InputDash()
{
    GamePad gamePad = controller->GetGamePad();
    if (gamePad.GetButton() & GamePad::BTN_RIGHT_SHOULDER)
    {
        // 連打防止
        return CoolTimer();
    }

    return false;
}

// 回避入力処理
bool Player::InputAvoid(int frame)
{
    // 回避スピードの初期化
    avoidSpeed = maxAvoidSpeed;

    if (controller->GetKeyPrecede(Key::KeyA, frame))
    {
        // スタミナが回避できるぐらいあれば true
        return stamina >= consumeAvoid;
    }

    return false;
}

void Player::InputTurn()
{
    DirectX::XMFLOAT3 forward = gameObject.lock()->transform.GetForward();
    DirectX::XMFLOAT3 stick   = controller->GetMoveVec();

    DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&forward);
    DirectX::XMVECTOR Stick   = DirectX::XMLoadFloat3(&stick);

    const MoveData moveData = eventInfo.GetMoveData();

    float lerpRate = (firstAttack) ? 1.0f : moveData.lerpRate;
    lerpRate = (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Stick)) != 0) ? lerpRate : 0.0f;

    DirectX::XMVECTOR Lerp = DirectX::XMVectorLerp(Forward, Stick, lerpRate);
    DirectX::XMFLOAT3 lerp{};
    DirectX::XMStoreFloat3(&lerp, Lerp);

    gameObject.lock()->transform.SetAngle({ 0, atan2f(lerp.x, lerp.z), 0 });
}

// **************************************
//                      
//      衝突処理   
//                      
// **************************************

// プレイヤーとエネミーの衝突処理 : 押し出し
void Player::CollisionPlayerVsEnemies()
{
#if 0
    // プレイヤーの頭位置取得
    DirectX::XMFLOAT3 head = gameObject.lock()->m_transform.GetPosition();
    head.y += height;

    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理    
    const int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        // 敵のヒット判定の数取得
        for(auto& data : enemy->hitinformation.hitDatas)
        {
            // 当たり判定の情報取得
            DirectX::XMFLOAT3 center = enemy->GetModel()->GetNodePos(data.nodeName.c_str());

            // 押し出し
            Collision::IntersectSphereVsCapsule(
                center,
                data.radius,
                100.0f,  // 重さ
                transform.m_position,
                head,
                radius,
                0.1f     // 重さ
            );
        }
    }
#endif
}

// 剣と敵の衝突処理
void Player::CollisionNodeVSEnemies(float power, int hitStop, float invisible)
{
#if 0
    // 喰らい判定の設定
    auto setHitData = [this](DirectX::XMFLOAT3& position, float& radius, const Model* model, const HitInformation::HitData& hitData)
    {
        const HitInformation::HitData& startHitData = hitData;
        const int startNodeIndex = model->FindNodeIndex(startHitData.nodeName);
        const Model::Node& node  = model->GetNodes().at(startNodeIndex);

        DirectX::XMVECTOR localPosition  = DirectX::XMLoadFloat3(&hitData.position);
        DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMVECTOR worldPosition  = DirectX::XMVector3Transform(localPosition, worldTransform);

        DirectX::XMStoreFloat3(&position, worldPosition);
        radius = hitData.radius;
    };

    // 剣の先端、根本取得
    const Model::Node& apexNode = sword->GetModel()->GetNodes().at(sword->GetSwordNode(Sword::SWOED_NODE::APEX));
    const Model::Node& rootNode = sword->GetModel()->GetNodes().at(sword->GetSwordNode(Sword::SWOED_NODE::ROOT));

    DirectX::XMFLOAT3 swordApexPos, swordRootPos;
    swordApexPos = GameMath::GetPositionFrom4x4(apexNode.worldTransform);
    swordRootPos = GameMath::GetPositionFrom4x4(rootNode.worldTransform);

    // 全ての敵と総当たりで衝突処理
    EnemyManager& enemyManager = EnemyManager::Instance();
    const int enemyCount       = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        const int hitDataCount = static_cast<int>(enemy->hitinformation.hitDatas.size()) - 1;

        // 全てのカプセルと総当たりで衝突処理
        for (int i = 0; i < hitDataCount; i += 2)
        {
            // 当たり判定の情報取得
            const HitInformation::HitData& startHitData = enemy->hitinformation.hitDatas[i];
            const HitInformation::HitData& goalHitData  = enemy->hitinformation.hitDatas[i + 1];

            DirectX::XMFLOAT3 startPosition{}, goalPosition{};
            float hitRadius = 0.0f;
            setHitData(startPosition, hitRadius, enemy->GetModel(), startHitData);
            setHitData(goalPosition,  hitRadius, enemy->GetModel(), goalHitData);

            // 敵のヒット判定と衝突処理
            if (Collision::IntersectCapsuleVSCapsule(
                swordApexPos,  swordRootPos, sword->GetRadius(),
                startPosition, goalPosition, hitRadius
            ))
            {
                // 戦闘になる
                enemy->MakeCondition(Dragon::Condition::C_Find);

                // 当たった部位から肉質データ取得
                HitInformation::PartData& partdata = enemy->hitinformation.partDatas[startHitData.hitPart];

                // 肉質 / 100
                float hitZone = partdata.hitZone * 0.01f;

                // ダメージ計算
                const float magnifcation = sword->GetMagnification();
                const float correction   = sword->GetCorrection();
                const float defenceRate  = enemy->GetDefenceRate();
                float finalPower = (magnifcation * power * correction * hitZone) * defenceRate;

                // ダメージを与える
                if (!enemy->ApplyDamage(static_cast<int>(finalPower), invisible)) return;

#pragma region 部位にダメージを与える
                // もし死んでいたら処理をしない
                if (enemy->condition & Dragon::Condition::C_Die) return;

                // 耐久値がゼロかつ怯まない状態なら
                partdata.durability -= static_cast<int>(finalPower * 0.1f);
                if (partdata.durability <= 0 && enemy->IsNotFlinch())
                {
                    enemy->MakeClack(partdata.clackPart);
                    enemy->MakeCondition(Dragon::Condition::C_Flinch);
                    partdata.durability = 800;

                    // 部位破壊時のエフェクト再生
                    hitEffect[HitEffect::Clack]->Play(swordApexPos);
                    PlaySE(SE::Clack);
                }
#pragma endregion

                // ヒットストップをセット
                this->hitStopFrame = static_cast<float>(hitStop) * hitZone;

                // エフェクト
                // 弱点外
                const float weakness = 0.45f;
                if (hitZone < weakness) { hitEffect[HitEffect::Weak]->Play(swordApexPos); }
                // 弱点
                else { hitEffect[HitEffect::Hard]->Play(swordApexPos, 0.7f); }

                // SE
                playerSEs[static_cast<int>(SE::Slash)]->Play(false);
            }
        }
    }
#endif
}

void Player::CollisionNodeVSEnemies(const AttackData& data)
{
    CollisionNodeVSEnemies(data.attackPower, data.hitStopFrame, data.invincibleTime);
}

//void Player::OnLanding()
//{
//    //// 砂のエフェクト
//    //const float scale = 0.3f;
//
//    //smokeEffect->Play(transform.GetPosition(), scale);
//}

// ダメージを受けた時に呼ばれる
void Player::OnDamaged()
{
    Audio::Instance().PlayAudio(AudioList::PlayerGetHit);

    // ダメージステート切り替え
    ChangeState(State::State_Damage);
}

// 死亡した時に呼ばれる
void Player::OnDead()
{
    Audio::Instance().PlayAudio(AudioList::PlayerDie);

    // 死亡したので false を返す
    isAlive = false;

    // 死亡ステート切り替え
    ChangeState(State::State_Death);
}

// 無敵時間
bool Player::Invincible()
{
    // 無敵中なら true
    if (invincible) return true;

    // 無敵時間がある true : ない false
    invincibleFrame = (std::max)(--invincibleFrame, 0.0f);
    return invincibleFrame > 0.0f;
}

// ヒットストップ
bool Player::HitStop()
{
    hitStopFrame = (std::max)(--hitStopFrame, 0.0f);
    return hitStopFrame <= 0.0f;
}