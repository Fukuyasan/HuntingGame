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

#pragma region �X�e�[�^�X�ݒ�
    // �����ʒu
    areaNum = 1;
    DirectX::XMFLOAT3 firstPos = MetaAI::Instance().GetAreaData(areaNum).areaPos;
    gameObject->transform.SetPosition(firstPos);

    // �X�P�[��
    gameObject->transform.SetScale(0.012f);

    // ����
    height = 1.3f;

    // HP
    health = maxHealth = 150;

    // �X�^�~�i
    stamina = maxStamina = 150.0f;
#pragma endregion
}

void Player::Start()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

    // �R���|�[�l���g�擾
    movement = gameObject->GetComponent<Movement>();
    animator = gameObject->GetComponent<Animator>();

    // �A�j���[�V�����̃C�x���g�f�[�^
    eventInfo.Deserialize("Data/Model/Player/HunterTest.event");

    // �X�e�[�g�}�V�[���ݒ�
    RegisterStateMachine();

    // �R���g���[���[
    controller = std::make_unique<PlayerContorller>(0);

    // �q�b�g�G�t�F�N�g
    //hitEffect[HitEffect::Weak]  = std::make_unique<Effect>("Data/Effect/Hit.efk");
    //hitEffect[HitEffect::Clack] = std::make_unique<Effect>("Data/Effect/Blow11.efk");
    //hitEffect[HitEffect::Hard]  = std::make_unique<Effect>("Data/Effect/HitWeakPoint.efk");

    // UI������
    //ui.Initialize();
}

void Player::Update()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();
    float elapsedTime = TimeManager::Instance().GetDeltaTime();

    // �R���g���[���[�X�V
    controller->Update();

    // �X�^�~�i��
    stamina += recoveryStamina * elapsedTime;
    stamina = Mathf::Clamp(stamina, 0.0f, maxStamina);

    // UI�X�V
    //ui.Update(elapsedTime);

    // �q�b�g�X�g�b�v�X�V
    float time = elapsedTime;

    // ����q�b�g�X�g�b�v�͎��Ԃ��~�߂�^�C�v�Ȃ̂�
    //  bool �� 1 : 0 �̓������g���Ď������Ă���B
    time *= HitStop();

    // �X�e�[�g�}�V�[���X�V
    stateMachine->Update(time);

    // �N�[���^�C���X�V
    UpdateCoolTimer(time);

    // �v���C���[�ƃG�l�~�[�̉����o������
    CollisionPlayerVsEnemies();

    // ���͏����X�V
    movement->UpdateVelocity(time);

    // ���G���ԍX�V
    UpdateInvincibleTimer(time);

    gameObject->transform.m_position.y = (std::max)(gameObject->transform.m_position.y, 0.0f);

    // �I�u�W�F�N�g�s����X�V
    gameObject->transform.UpdateTransform();

    // ���f���A�j���[�V�����X�V
    animator->UpdateAnimation(time);
    animator->UpdateRootMotion(gameObject->transform);

    // ���f���s��X�V
    gameObject->GetModel()->UpdateTransform(gameObject->transform.m_transform);
}


// �v���C���[�̃X�e�[�g�}�V����ݒ�
void Player::RegisterStateMachine()
{
    stateMachine = std::make_unique<StateMachine<Player>>();

    stateMachine->RegisterState(std::make_shared<IdleState>(this));
    stateMachine->RegisterState(std::make_shared<WalkState>(this));
    stateMachine->RegisterState(std::make_shared<RunState>(this));
    stateMachine->RegisterState(std::make_shared<DashState>(this));
    stateMachine->RegisterState(std::make_shared<AvoidState>(this));

    //  ��U���X�e�[�g
    stateMachine->RegisterState(std::make_shared<ComboA_1State>(this));
    stateMachine->RegisterState(std::make_shared<ComboA_2State>(this));
    stateMachine->RegisterState(std::make_shared<ComboA_3State>(this));

    //   ���U���X�e�[�g
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

    // �n�߂̃X�e�[�g��ݒ�
    stateMachine->SetState(static_cast<int>(State::State_Idle));
}

// ����̕�����ݒ�
void Player::SetAvoidVec()
{
    // �X�e�B�b�N�̓��͂��Ȃ���΃v���C���[�̑O�x�N�g�����������ɐݒ�
    avoidVec = controller->GetMoveVec();

    float lengthSq = avoidVec.x * avoidVec.x + avoidVec.z * avoidVec.z;    
    if (lengthSq == 0.0f)
    {
        avoidVec = gameObject.lock()->transform.GetForward();
    }

    // ����̖��G���Ԑݒ�
    invincibleFrame = 10.0f;

    // �X�^�~�i�̏���
    stamina -= consumeAvoid;
}

// �������
void Player::PlayAvoid(const float& elapsedTime)
{
#if 0
    float currentTime = gameObject.lock()->GetAnimator()->GetAnimationSeconds();
    float totalTime   = animator->GetAnimationLength();

    // ����̃X�s�[�h���C�[�W���O���g���Ă��������ɂ���
    avoidSpeed *= Easing::OutQuart(totalTime - currentTime, totalTime);

    movement->Move(avoidVec.x, avoidVec.z, avoidSpeed);
#endif
    movement->Turn(elapsedTime, avoidVec.x, avoidVec.z, turnSpeed);
}

#pragma region �f�o�b�O
// �f�o�b�O�pGUI�`��
void Player::OnDebugGUI()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // �p�����[�^
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

// �f�o�b�O�p�v���~�e�B�u�`��
void Player::OnDebugPrimitive()
{
    DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

#if 1
    // �v���C���[�̓��ʒu�擾
    DirectX::XMFLOAT3 head = gameObject.lock()->transform.GetPosition();
    head.y += height;

    // �J�v�Z���`��
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
//      ���͏���
//                      
// *********************

// �ړ����͏���
void Player::InputMove(const float& elapsedTime)
{
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = controller->GetMoveVec();

    // �ړ�����
    movement->Move(moveVec.x, moveVec.z, movement->GetMaxMoveSpeed());

    // ���񏈗�
    movement->Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

// �_�b�V�����͏���
bool Player::InputDash()
{
    GamePad gamePad = controller->GetGamePad();
    if (gamePad.GetButton() & GamePad::BTN_RIGHT_SHOULDER)
    {
        // �A�Ŗh�~
        return CoolTimer();
    }

    return false;
}

// �����͏���
bool Player::InputAvoid(int frame)
{
    // ����X�s�[�h�̏�����
    avoidSpeed = maxAvoidSpeed;

    if (controller->GetKeyPrecede(Key::KeyA, frame))
    {
        // �X�^�~�i������ł��邮�炢����� true
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
//      �Փˏ���   
//                      
// **************************************

// �v���C���[�ƃG�l�~�[�̏Փˏ��� : �����o��
void Player::CollisionPlayerVsEnemies()
{
#if 0
    // �v���C���[�̓��ʒu�擾
    DirectX::XMFLOAT3 head = gameObject.lock()->m_transform.GetPosition();
    head.y += height;

    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���    
    const int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        // �G�̃q�b�g����̐��擾
        for(auto& data : enemy->hitinformation.hitDatas)
        {
            // �����蔻��̏��擾
            DirectX::XMFLOAT3 center = enemy->GetModel()->GetNodePos(data.nodeName.c_str());

            // �����o��
            Collision::IntersectSphereVsCapsule(
                center,
                data.radius,
                100.0f,  // �d��
                transform.m_position,
                head,
                radius,
                0.1f     // �d��
            );
        }
    }
#endif
}

// ���ƓG�̏Փˏ���
void Player::CollisionNodeVSEnemies(float power, int hitStop, float invisible)
{
#if 0
    // ��炢����̐ݒ�
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

    // ���̐�[�A���{�擾
    const Model::Node& apexNode = sword->GetModel()->GetNodes().at(sword->GetSwordNode(Sword::SWOED_NODE::APEX));
    const Model::Node& rootNode = sword->GetModel()->GetNodes().at(sword->GetSwordNode(Sword::SWOED_NODE::ROOT));

    DirectX::XMFLOAT3 swordApexPos, swordRootPos;
    swordApexPos = GameMath::GetPositionFrom4x4(apexNode.worldTransform);
    swordRootPos = GameMath::GetPositionFrom4x4(rootNode.worldTransform);

    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    EnemyManager& enemyManager = EnemyManager::Instance();
    const int enemyCount       = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        const int hitDataCount = static_cast<int>(enemy->hitinformation.hitDatas.size()) - 1;

        // �S�ẴJ�v�Z���Ƒ�������ŏՓˏ���
        for (int i = 0; i < hitDataCount; i += 2)
        {
            // �����蔻��̏��擾
            const HitInformation::HitData& startHitData = enemy->hitinformation.hitDatas[i];
            const HitInformation::HitData& goalHitData  = enemy->hitinformation.hitDatas[i + 1];

            DirectX::XMFLOAT3 startPosition{}, goalPosition{};
            float hitRadius = 0.0f;
            setHitData(startPosition, hitRadius, enemy->GetModel(), startHitData);
            setHitData(goalPosition,  hitRadius, enemy->GetModel(), goalHitData);

            // �G�̃q�b�g����ƏՓˏ���
            if (Collision::IntersectCapsuleVSCapsule(
                swordApexPos,  swordRootPos, sword->GetRadius(),
                startPosition, goalPosition, hitRadius
            ))
            {
                // �퓬�ɂȂ�
                enemy->MakeCondition(Dragon::Condition::C_Find);

                // �����������ʂ�������f�[�^�擾
                HitInformation::PartData& partdata = enemy->hitinformation.partDatas[startHitData.hitPart];

                // ���� / 100
                float hitZone = partdata.hitZone * 0.01f;

                // �_���[�W�v�Z
                const float magnifcation = sword->GetMagnification();
                const float correction   = sword->GetCorrection();
                const float defenceRate  = enemy->GetDefenceRate();
                float finalPower = (magnifcation * power * correction * hitZone) * defenceRate;

                // �_���[�W��^����
                if (!enemy->ApplyDamage(static_cast<int>(finalPower), invisible)) return;

#pragma region ���ʂɃ_���[�W��^����
                // ��������ł����珈�������Ȃ�
                if (enemy->condition & Dragon::Condition::C_Die) return;

                // �ϋv�l���[�������܂Ȃ���ԂȂ�
                partdata.durability -= static_cast<int>(finalPower * 0.1f);
                if (partdata.durability <= 0 && enemy->IsNotFlinch())
                {
                    enemy->MakeClack(partdata.clackPart);
                    enemy->MakeCondition(Dragon::Condition::C_Flinch);
                    partdata.durability = 800;

                    // ���ʔj�󎞂̃G�t�F�N�g�Đ�
                    hitEffect[HitEffect::Clack]->Play(swordApexPos);
                    PlaySE(SE::Clack);
                }
#pragma endregion

                // �q�b�g�X�g�b�v���Z�b�g
                this->hitStopFrame = static_cast<float>(hitStop) * hitZone;

                // �G�t�F�N�g
                // ��_�O
                const float weakness = 0.45f;
                if (hitZone < weakness) { hitEffect[HitEffect::Weak]->Play(swordApexPos); }
                // ��_
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
//    //// ���̃G�t�F�N�g
//    //const float scale = 0.3f;
//
//    //smokeEffect->Play(transform.GetPosition(), scale);
//}

// �_���[�W���󂯂����ɌĂ΂��
void Player::OnDamaged()
{
    Audio::Instance().PlayAudio(AudioList::PlayerGetHit);

    // �_���[�W�X�e�[�g�؂�ւ�
    ChangeState(State::State_Damage);
}

// ���S�������ɌĂ΂��
void Player::OnDead()
{
    Audio::Instance().PlayAudio(AudioList::PlayerDie);

    // ���S�����̂� false ��Ԃ�
    isAlive = false;

    // ���S�X�e�[�g�؂�ւ�
    ChangeState(State::State_Death);
}

// ���G����
bool Player::Invincible()
{
    // ���G���Ȃ� true
    if (invincible) return true;

    // ���G���Ԃ����� true : �Ȃ� false
    invincibleFrame = (std::max)(--invincibleFrame, 0.0f);
    return invincibleFrame > 0.0f;
}

// �q�b�g�X�g�b�v
bool Player::HitStop()
{
    hitStopFrame = (std::max)(--hitStopFrame, 0.0f);
    return hitStopFrame <= 0.0f;
}