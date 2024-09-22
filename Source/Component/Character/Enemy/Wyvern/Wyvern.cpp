#include "Wyvern.h"

#include "AI/MetaAI/MetaAI.h"

#include "Graphics/CreateShader.h"
#include "Graphics/ShaderState.h"
#include "Graphics/Renderer/DebugRenderer.h"

#ifdef Wyvern

Wyvern::Wyvern()
{
#pragma region ���f���ݒ�
	model    = std::make_unique<Model>("Data/Model/Dragon/Wyvern/Great Wyvern.mdl");
	model->LoadAnimation("Data/Model/Dragon/Wyvern/Great Wyvern.anim");

	animator = std::make_unique<Animator>(*model.get());
	animator->SetupRootMotion("Esqueleto");	

	// �傫���Ƀ����_�������������邱�ƂŐ����炵������������_��
	const int minS = 10;
	const int maxS = 12;
	int scale = Mathf::RandomRange(minS, maxS);

	// ���f�����傫���̂ŁA0.01 ~ 0.012�̊ԂɎ��߂�
	transform.SetScale(0.001f * scale);
		
	headIndex  = model->FindNodeIndex("Cabeza");
	rHandIndex = model->FindNodeIndex("Ala_Izq");
	lHandIndex = model->FindNodeIndex("Ala_Der");
#pragma endregion

#pragma region �p�����[�^�ݒ�
	// �̗͐ݒ�
	health = maxHealth = 5000;

	// �G���A�̑؍ݎ���
	stayTime = 60.0f;

	// �����l�ݒ�
	areaNum = 0;
	MetaAI::AreaData areaData = MetaAI::Instance().GetAreaData(areaNum);

	DirectX::XMFLOAT3 firstPos   = areaData.areaPos;
	float			  firstRange = areaData.areaRange;

	transform.SetPosition(firstPos);
	SetTerritory(firstPos, firstRange);

	// �I�u�W�F�N�g�s��X�V
	transform.UpdateTransform();
	// ���f���s��X�V
	model->UpdateTransform(transform.GetTransform());

	// �����o�����a
	penentrationRadius = 1.5f;
	searchRange = 35.0f;

	// �����蔻���JsonFIle����擾
	hitinformation.Decerialize("Data/Json/Wyvern/");

	// �r�w�C�r�A�c���[
	wyvernTree = std::make_unique<WyvernBehaviorTree>();
	wyvernTree->RegisterBehaviorTree(this);
#pragma endregion
}

Wyvern::~Wyvern()
{
}

void Wyvern::Update(const float& elapsedTime)
{
	// ��Ԑݒ�
	SetCondition();

	// �r�w�C�r�A�c���[�X�V
	if (debugFlg[Debug::PlayBehabior])
	{
		wyvernTree->UpdateBehaviorTree(elapsedTime);
	}

	// �e�ƃv���C���[�̓����蔻��
	//ProjectileVSPlayer();

	// ���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	// �A�j���[�V�����X�V
	animator->UpdateAnimation(elapsedTime);
	animator->UpdateRootMotion(transform);

	// ���͏����X�V
	movement.UpdateVelocity(elapsedTime);

	// �I�u�W�F�N�g�s��X�V
	transform.UpdateTransform();

	// ���f���s��X�V
	model->UpdateTransform(transform.GetTransform());
}

void Wyvern::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

#pragma region �f�o�b�O
// �f�o�b�O�`��
void Wyvern::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

	// �q�b�g����̃J�v�Z���`��
	if (debugFlg[Debug::HitCapsule]) {
		for (HitInformation::HitData& hit : hitinformation.hitDatas) {
			DrawDebugHitData(
				debugRenderer,
				hit
			);
		}
	}
}

void Wyvern::DrawDebugGUI()
{
	hitinformation.DrawDebugGUI();
	hitinformation.EditHitData(model.get());

	ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Wyvern", nullptr, ImGuiWindowFlags_None))
	{
		// �r�w�C�r�A�c���[
		wyvernTree->DebugBehaviorTree();

		// ���
		DebugCondition();

		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// �ʒu
			DirectX::XMFLOAT3 position = transform.GetPosition();
			ImGui::InputFloat3("Position", &position.x);
			transform.SetPosition(position);

			// ��]
			DirectX::XMFLOAT3 a{};
			DirectX::XMFLOAT3 angle = transform.GetAngle();
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);

			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			transform.SetAngle(angle);

			// �X�P�[��
			float s = transform.GetScale().x;
			ImGui::InputFloat("Scale", &s);

			transform.SetScale(s);
		}

		//�X�e�[�^�X
		if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::SliderInt("HP", &health, 0, maxHealth);
		}

		// �r�w�C�r�A�J�n
		ImGui::Checkbox("PlayBehavior",   &debugFlg[Debug::PlayBehabior]);
		ImGui::Checkbox("ShowHitCapsule", &debugFlg[Debug::HitCapsule]);
		ImGui::Checkbox("Penetration",    &debugFlg[Debug::Penetration]);
		ImGui::Checkbox("LookAtIK",		  &debugFlg[Debug::AimIK]);

		if (ImGui::Button("PlayAreaMove"))
		{
			stayTime = 0.0f;
		}

		if (ImGui::Button("PlayAngry"))
		{
			condition |= Condition::C_Angry;
		}

		// �p�[�e�B�N��
		if (ImGui::Button("Breath"))
		{
			//breath->Initialize(transform.position);
		}
		if (ImGui::Button("eruption"))
		{
			//eruption->Initialize(transform.position);
		}

		if (ImGui::Button("Save"))
		{
			hitinformation.Serialize("Data/Json/Wyvern/");
		}
	}
	ImGui::End();
}

void Wyvern::DebugBehaviorTree()
{
}

void Wyvern::DebugCondition()
{
}
#pragma endregion

void Wyvern::CollisionNodeVSPlayer(const char* nodeName, float boneRadius)
{
}

void Wyvern::CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position, float boneRadius)
{
}

void Wyvern::SetCondition()
{
	// ����
	if (SearchTarget(searchRange, Player::Instance().transform.GetPosition()))
		condition |= Condition::C_Find;

	// �G���A�ړ�
	if (stayTime <= 0.0f)
		condition |= Condition::C_AreaMove;

	// �{��
	if (health < maxHealth * 0.5f)
	{
		condition |= Condition::C_Angry;
		magnification = 1.5f;
	}
}

void Wyvern::OnDead()
{
	condition &= !Wyvern::Condition::C_ALL;
	condition |= Condition::C_Die;
}

#endif