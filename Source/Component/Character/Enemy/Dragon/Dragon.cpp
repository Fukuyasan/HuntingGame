#include "Dragon.h"

#include "AI/MetaAI/MetaAI.h"

#include "Graphics/CreateShader.h"
#include "Graphics/ShaderState.h"
#include "Graphics/Renderer/DebugRenderer.h"
#include "Graphics/Texture.h"

#include "System/Math/Mathf.h"
#include "System/Math/Easing.h"
#include "System/Input/Input.h"

#include "Component/Character/Player/player.h"
#include "Component/Projectile/ProjectileStraite.h"
#include "Component/Projectile/ProjectileManager.h"

#include "Component/Stage/StageMain.h"

#include "Camera/Camera.h"
#include "Camera/CameraController.h"

void Dragon::Awake()
{
	auto object = gameObject.lock();

#pragma region �p�����[�^�ݒ�
	// �̗͐ݒ�
	health = maxHealth = 26000;

	// �G���A�̑؍ݎ���
	stayTime = 120.0f;

	// �����l�ݒ�
	areaNum = 0;
	MetaAI::UseAreaData areaData = MetaAI::Instance().GetAreaData(areaNum);

	DirectX::XMFLOAT3 firstPos = areaData.areaPos;
	float			  firstRange = areaData.areaRange;

	object->GetTransform().SetPosition(firstPos);
	SetTerritory(firstPos, firstRange);

	// �����o�����a
	penentrationRadius = 1.0f;
	searchRange = 35.0f;

	// �����蔻���JsonFIle����擾
	hitinformation.Decerialize("Data/Json/Dragon/");

	condition |= Condition::C_Movie;
#pragma endregion
}

void Dragon::Start()
{
	auto object = gameObject.lock();

	// �R���|�[�l���g�擾
	movement = gameObject.lock()->GetComponent<Movement>();
	animator = gameObject.lock()->GetComponent<Animator>();

#pragma region ���f���ݒ�
	// �傫���Ƀ����_�������������邱�ƂŐ����炵������������_��
	const int minS = 10;
	const int maxS = 12;
	int scale = Mathf::RandomRange(minS, maxS);

	// ���f�����傫���̂ŁA0.01 ~ 0.012�̊ԂɎ��߂�
	object->GetTransform().SetScale(0.001f * scale);
#pragma endregion

	eventInfo.Deserialize("Data/Model/Dragon/Dragon.event");

#pragma region �m�[�h�֌W
	// LookAtIK�p�̃m�[�h��ݒ� : ��
	object->GetModel()->SetLookAtIK("Neck02");

	// �m�[�h�ԍ��擾
	headIndex   = object->GetModel()->FindNodeIndex("Head");
	handIndex   = object->GetModel()->FindNodeIndex("R_Hand");
	tongueIndex = object->GetModel()->FindNodeIndex("Tongue01");
#pragma endregion

	// �r�w�C�r�A�c���[
	dragonTree = std::make_unique<DragonBehaviorTree>();
	dragonTree->RegisterBehaviorTree(this);

#pragma region �G�t�F�N�g
	//smokeEffect = std::make_unique<Effect>("Data/Effect/smoke.efkefc");
	//fileEffect  = std::make_unique<Effect>("Data/Effect/fire.efkproj");

	// �p�[�e�B�N���ݒ�
	// �u���X�@���@�{�莞�̃p�[�e�B�N��
	breath = std::make_unique<BreathParticle>(300000);
	breath->Load("BreathParticle");

	// ����
	eruption = std::make_unique<BreathParticle>(300000);
	eruption->Load("EruptionParticle");
#pragma endregion

#pragma region ���ʔj��
	ID3D11Device* device = Graphics::Instance().GetDevice();
	CreateShader::CreateCB(device, sizeof(CBClack), cbClack.ReleaseAndGetAddressOf());

	// ���ʔj��e�N�X�`��
	Texture::LoadTexture("Data/Sprite/clack.png", device, clackTexture.GetAddressOf());
#pragma endregion
}

void Dragon::Update()
{
	auto object = gameObject.lock();

	float elapsedTime = TimeManager::Instance().GetDeltaTime();

	// ��Ԑݒ�
	SetCondition();

	// �r�w�C�r�A�c���[�X�V
	if (debugFlg[Debug::PlayBehabior])
	{
		dragonTree->UpdateBehaviorTree(elapsedTime);
	}

	// �e�ƃv���C���[�̓����蔻��
	ProjectileVSPlayer();

	// ���͏����X�V
	movement->UpdateVelocity(elapsedTime);

	// ���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	// �N�[���^�C���X�V
	UpdateCoolTimer(elapsedTime);
	
	// �A�j���[�V�����X�V
	animator->UpdateAnimation(elapsedTime);

	// �I�u�W�F�N�g�s��X�V
	object->GetTransform().UpdateTransform();

	// ������������
	if (lookFlg)
	{
		underTime += elapsedTime * 2.0f;
		underRotate = Easing::OutQuart((std::min)(underTime, 1.0f), 1.0f, maxUnderRotate);  // ���X�ɉ�������

		Model::Node& aimNode = object->GetModel()->GetAimNode();

		// Z������]
		DirectX::XMStoreFloat4(&aimNode.rotate, DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0, 0, 1, 0), underRotate));
	}
	else
	{
		underTime = 0.0f;
	}

	// ���f���s��X�V
	object->GetModel()->UpdateTransform(object->GetTransform().GetTransform());

	// �p�[�e�B�N���X�V
	breath->Update(elapsedTime);
	eruption->Update(elapsedTime);

	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
		
	// ���ʔj��̒萔�o�b�t�@���X�V
	clack.clackFlg = clackFlg;
	dc->UpdateSubresource(cbClack.Get(), 0, 0, &clack, 0, 0);
	dc->PSSetConstantBuffers(5, 1, cbClack.GetAddressOf());
	
	// ���ʔj��p�̃e�N�X�`�����V�F�[�_�[���ɑ���
	dc->PSSetShaderResources(22, 1, clackTexture.GetAddressOf());
}

//// �`�揈��
//void Dragon::Render(ID3D11DeviceContext* dc, Shader* shader)
//{
//	
//
//	shader->Draw(dc, model.get());
//}

//// ���n�����ꍇ�ɌĂ΂��
//void Dragon::OnLanding()
//{
//	Audio::Instance().PlayAudio(AudioList::DragonLand);
//
//	CameraController::Instance().SetShakeMode(0.3f, 5.0f, 5.0f);
//	smokeEffect->Play(gameObject.lock()->GetTransform().GetPosition(), 0.5f);
//}

#pragma region �_���[�W���󂯂��Ƃ��̏���
// ���S�������ɌĂ΂��
void Dragon::OnDead()
{
	Audio::Instance().PlayAudio(AudioList::DragonRoar);

	condition &= !Dragon::Condition::C_ALL;
	condition |= Condition::C_Die;

	isAlive = false;
}

// �_���[�W���󂯂��Ƃ��ɌĂ΂��
void Dragon::OnDamaged()
{

}
#pragma endregion

// �v���C���[�ƃm�[�h�̓����蔻��
void Dragon::CollisionNodeVSPlayer(const unsigned int nodeIndex)
{
	//// �m�[�h�̈ʒu�Ɠ����蔻����s��
	//const Model::Node& node = model->GetNodes().at(nodeIndex);

	//// �m�[�h�̃��[���h���W
	//DirectX::XMFLOAT3 nodePosition = GameMath::GetPositionFrom4x4(node.worldTransform);

	//const float attackPower = eventInfo.GetAttackData().attackPower;
	//SetAttackPower(attackPower * GetMagnification());

	//CollisionNodeVSPlayer(nodePosition);
}

void Dragon::CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position)
{
	//// �����蔻��\��
	//ShaderState::Instance().GetDebugRenderer()->AddSphere(
	//	position, attackRadius, DirectX::XMFLOAT4(1, 0, 0, 1)
	//);

	//// �v���C���[�����G�Ȃ瓖���蔻�肵�Ȃ�
	//if (player.Invincible()) return;

	//Transform playerT = player.transform;

	//DirectX::XMFLOAT3 goalPos =
	//{
	//	playerT.GetPosition().x, 
	//	playerT.GetPosition().y + player.GetHeight(), 
	//	playerT.GetPosition().z 
	//};

	//// �J�v�Z���Ɖ~�̓����蔻��
	//if (Collision::IntersectSphereVsCapsule(
	//	position,
	//	attackRadius,
	//	playerT.GetPosition(),
	//	goalPos,
	//	player.GetRadius()
	//))
	//{
	//	// �_���[�W��^����
	//	if (player.ApplyDamage((int)attackPower, 0.5f))
	//	{
	//		// ������΂��������v�Z
	//		DirectX::XMFLOAT3 vec;
	//		const float power = 40.0f;

	//		GameMath::Impulse(power, position, goalPos, vec);

	//		vec.y = 10.0f;

	//		// ������΂�������ݒ�
	//		//player.SetImpulseVec(vec);

	//		// ������΂�
	//		//player.movement.AddImpulse(vec);

	//		bossSEs[SE::FootStep]->Play(false);

	//	}
	//}
}

void Dragon::ProjectileVSPlayer()
{
	//Player& player = Player::Instance();

	//DirectX::XMFLOAT3 startPos = player.transform.GetPosition();
	//DirectX::XMFLOAT3 goalPos  = player.transform.GetPosition();
	//goalPos.y += player.GetHeight();

	//// �S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	//int projectileCount = ProjectileManager::Instance().GetProjectileCount();
	//for (int i = 0; i < projectileCount; ++i)
	//{
	//	// �e�̃^�C�v���Ⴄ�� continue
	//	Projectile* projectile = ProjectileManager::Instance().GetProjectile(i);
	//	if (projectile->GetType() != Type::Dragon) continue;

	//	// �Փˏ���
	//	if (Collision::IntersectSphereVsCapsule(
	//		projectile->GetPosition(),
	//		projectile->GetRadius(),
	//		startPos,
	//		goalPos,
	//		player.GetRadius()
	//	))
	//	{
	//		// �_���[�W��^����
	//		if (!player.ApplyDamage((int)attackPower, 3.0f)) return;

	//		// ������΂��������v�Z
	//		DirectX::XMFLOAT3 vec;
	//		const float power = 50.0f;

	//		DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&projectile->GetDirection()));
	//		DirectX::XMStoreFloat3(&vec, DirectX::XMVectorScale(Dir, power));
	//		vec.y = 10.0f;

	//		// ������΂�������ݒ�
	//		player.SetImpulseVec(vec);

	//		// ������΂�
	//		player.movement.AddImpulse(vec);

	//		bossSEs[SE::Explosion]->Play(false);

	//		projectile->Destroy();
	//	}
	//}
}

#pragma region �f�o�b�O
// �f�o�b�O�`��
void Dragon::OnDebugPrimitive()
{
	//DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

	//// �꒣��͈͂��f�o�b�O�~���`��
	//debugRenderer->AddCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

	//// �^�[�Q�b�g�ʒu���f�o�b�O���`��
	//debugRenderer->AddSphere(targetPosition, radius, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

	//// ���G�͈͂��f�o�b�O�~���`��
	//debugRenderer->AddCylinder(transform.GetPosition(), searchRange, 1.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	//// �U���͈͂��f�o�b�O�~���`��
	//debugRenderer->AddCylinder(transform.GetPosition(), attackRange, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	//
	//// �q�b�g����̃J�v�Z���`��
	//if (debugFlg[Debug::HitCapsule]) {
	//	for (HitInformation::HitData& hit : hitinformation.hitDatas) {
	//		DrawDebugHitData(
	//			debugRenderer,
	//			hit
	//		);
	//	}
	//}
}

// �f�o�b�O�G�l�~�[���\��
void Dragon::OnDebugGUI()
{
	//hitinformation.DrawDebugGUI();
	//hitinformation.EditHitData(model.get());

	//ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	//if (ImGui::Begin("Dragon", nullptr, ImGuiWindowFlags_None))
	//{
	//	// �r�w�C�r�A�c���[
	//	dragonTree->DebugBehaviorTree();

	//	// ���
	//	DebugCondition();

	//	//�X�e�[�^�X
	//	if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		ImGui::SliderInt("HP", &health, 0, maxHealth);
	//	}

	//	if (ImGui::CollapsingHeader("Look", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		ImGui::DragFloat("UnderRotate", &underRotate, 0.1f, -5.0f, 5.0f, "%.2f");
	//		ImGui::Checkbox("LookAt", &lookFlg);
	//	}

	//	// �r�w�C�r�A�J�n
	//	ImGui::Checkbox("PlayBehavior",   &debugFlg[Debug::PlayBehabior]);
	//	ImGui::Checkbox("ShowHitCapsule", &debugFlg[Debug::HitCapsule]);
	//	ImGui::Checkbox("Penetration",    &debugFlg[Debug::Penetration]);
	//	ImGui::Checkbox("LookAtIK",       &debugFlg[Debug::AimIK]);

	//	if (ImGui::Button("PlayAreaMove"))
	//	{
	//		stayTime = 0.0f;
	//	}

	//	if (ImGui::Button("PlayAngry"))
	//	{
	//		condition |= Condition::C_Angry;
	//	}

	//	// �p�[�e�B�N��
	//	if (ImGui::Button("Breath"))
	//	{
	//		breath->Initialize(transform.m_position);
	//	}
	//	if (ImGui::Button("eruption"))
	//	{
	//		eruption->Initialize(transform.m_position);
	//	}

	//	if (ImGui::Button("Save"))
	//	{
	//		hitinformation.Serialize("Data/Json/Dragon/");
	//	}
	//}
	//ImGui::End();
}

void Dragon::DebugCondition()
{
	if (ImGui::CollapsingHeader("Condition", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool find = (condition & Condition::C_Find);
		ImGui::Checkbox("Find", &find);

		bool areaMove = (condition & Condition::C_AreaMove);
		ImGui::Checkbox("AreaMove", &areaMove);

		bool angry = (condition & Condition::C_Angry);
		ImGui::Checkbox("Angry", &angry);

		bool flinch = (condition & Condition::C_Flinch);
		ImGui::Checkbox("Flinch", &flinch);

		bool die = (condition & Condition::C_Die);
		ImGui::Checkbox("Die", &die);
	}
}
#pragma endregion

// ��
void Dragon::RockShot(
	std::shared_ptr<ModelResource> resource,
	const DirectX::XMFLOAT3& position,
	const int rockCount
)
{
	//if (stopRockShot) return;

	//bossSEs[SE::Explosion]->Play(false);

	//// �v���C���[��Y���W + ����
	//float pPosY = Player::Instance().transform.GetPosition().y;
	//pPosY += Player::Instance().GetHeight();

	//// �O�����ɔ�΂�
	//DirectX::XMVECTOR Forward, Right;
	//Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&transform.GetForward()));
	//Right   = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&transform.GetRight()));

	//// �p�����[�^�ݒ�
	//StraiteParam param{};
	//param.position = position;
	//param.speed    = 50.0f;
	//param.radius   = 1.0f;
	//param.type     = Type::Dragon;

	//// ���ɔ�΂�
	//const float shotRate = 0.1f;
	//for (int i = 1; i <= rockCount; ++i)
	//{
	//	int a = (i & 1);	   // � 1 : ���� 0
	//	int b = 2 * a - 1;	   // � 1 : ���� -1
	//	int c = (i - a) >> 1;  // 0, 2, 2, 4... �� 2 �Ŋ���̂Ɠ��`

	//	// ��΂�����(�܂�����)
	//	DirectX::XMVECTOR Dir = DirectX::XMVectorLerp(Forward, DirectX::XMVectorScale(Right, (float)b), c * shotRate);
	//	DirectX::XMFLOAT3 dir;
	//	DirectX::XMStoreFloat3(&dir, Dir);

	//	// �v���C���[�̍����ɔ��ł����悤�ɂ���
	//	float distY = pPosY - transform.GetPosition().y;
	//	dir.y += DirectX::XMConvertToRadians(distY);
	//	param.direction = dir;

	//	// ����
	//	ProjectileStraite* projectile_straite = new ProjectileStraite(resource);
	//	ProjectileManager::Instance().Register(projectile_straite);
	//	projectile_straite->Launch(param);
	//}

	//stopRockShot = true;
}

void Dragon::SetCondition()
{
	//// ����
	//if (SearchTarget(searchRange, Player::Instance().transform.GetPosition()))
	//	condition |= Condition::C_Find;

	//// �G���A�ړ�
	//if(stayTime <= 0.0f)
	//	condition |= Condition::C_AreaMove;
	//
	//// �{��
	//if (health < maxHealth * 0.5f)
	//{
	//	condition |= Condition::C_Angry;
	//	magnification = 1.5f;
	//}
}

void Dragon::SetAnimationSpeed(float normal, float angry)
{
	bool Angry = (condition & Condition::C_Angry) != 0;

	float animSpeed = (Angry) ? angry : normal;
	animator->SetAnimSpeed(animSpeed);
}