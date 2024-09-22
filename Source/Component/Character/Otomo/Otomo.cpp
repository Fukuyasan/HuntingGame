#include "Otomo.h"
#include "GameObject/GameObjectManager.h"

void Otomo::Awake()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

#pragma region �X�e�[�^�X�ݒ�
    // �����ʒu
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("Player");
    gameObject->transform.SetPosition(player->GetTransform().GetPosition());

    // �X�P�[��
    gameObject->transform.SetScale(0.012f);

    // ����
    height = 0.3f;

    // HP
    health = maxHealth = 150;
#pragma endregion
}

void Otomo::Start()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();

    // �R���|�[�l���g�擾
    movement = gameObject->GetComponent<Movement>();
    animator = gameObject->GetComponent<Animator>();

    // �G�t�F�N�g
    //smokeEffect = std::make_unique<Effect>("Data/Effect/smokeWalk.efkefc");

    // �A�j���[�V�����̃C�x���g�f�[�^
    eventInfo.Deserialize("Data/Model/SDUnityChan/SDUnityChan.mdl/SDUnityChan.event");

    otomoTree = std::make_unique<OtomoBehaviorTree>();
    otomoTree->RegisterBehaviorTree(this);
}

void Otomo::Update()
{
    std::shared_ptr<GameObject> gameObject = GetGameObject();
    float elapsedTime = TimeManager::Instance().GetDeltaTime();

    otomoTree->UpdateBehaviorTree(elapsedTime);

    // ���͏����X�V
    movement->UpdateVelocity(elapsedTime);

    gameObject->transform.m_position.y = (std::max)(gameObject->transform.m_position.y, 0.0f);

    // �I�u�W�F�N�g�s����X�V
    gameObject->transform.UpdateTransform();

    // ���f���A�j���[�V�����X�V
    animator->UpdateAnimation(elapsedTime);
    animator->UpdateRootMotion(gameObject->transform);

    // ���f���s��X�V
    gameObject->GetModel()->UpdateTransform(gameObject->transform.m_transform);
}
