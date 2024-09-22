#include "Wyvern.h"

#include "AI/MetaAI/MetaAI.h"

#include "Graphics/CreateShader.h"
#include "Graphics/ShaderState.h"
#include "Graphics/Renderer/DebugRenderer.h"

#ifdef Wyvern

Wyvern::Wyvern()
{
#pragma region モデル設定
	model    = std::make_unique<Model>("Data/Model/Dragon/Wyvern/Great Wyvern.mdl");
	model->LoadAnimation("Data/Model/Dragon/Wyvern/Great Wyvern.anim");

	animator = std::make_unique<Animator>(*model.get());
	animator->SetupRootMotion("Esqueleto");	

	// 大きさにランダム性を持たせることで生物らしさを持たせる狙い
	const int minS = 10;
	const int maxS = 12;
	int scale = Mathf::RandomRange(minS, maxS);

	// モデルが大きいので、0.01 ~ 0.012の間に収める
	transform.SetScale(0.001f * scale);
		
	headIndex  = model->FindNodeIndex("Cabeza");
	rHandIndex = model->FindNodeIndex("Ala_Izq");
	lHandIndex = model->FindNodeIndex("Ala_Der");
#pragma endregion

#pragma region パラメータ設定
	// 体力設定
	health = maxHealth = 5000;

	// エリアの滞在時間
	stayTime = 60.0f;

	// 初期値設定
	areaNum = 0;
	MetaAI::AreaData areaData = MetaAI::Instance().GetAreaData(areaNum);

	DirectX::XMFLOAT3 firstPos   = areaData.areaPos;
	float			  firstRange = areaData.areaRange;

	transform.SetPosition(firstPos);
	SetTerritory(firstPos, firstRange);

	// オブジェクト行列更新
	transform.UpdateTransform();
	// モデル行列更新
	model->UpdateTransform(transform.GetTransform());

	// 押し出し半径
	penentrationRadius = 1.5f;
	searchRange = 35.0f;

	// 当たり判定をJsonFIleから取得
	hitinformation.Decerialize("Data/Json/Wyvern/");

	// ビヘイビアツリー
	wyvernTree = std::make_unique<WyvernBehaviorTree>();
	wyvernTree->RegisterBehaviorTree(this);
#pragma endregion
}

Wyvern::~Wyvern()
{
}

void Wyvern::Update(const float& elapsedTime)
{
	// 状態設定
	SetCondition();

	// ビヘイビアツリー更新
	if (debugFlg[Debug::PlayBehabior])
	{
		wyvernTree->UpdateBehaviorTree(elapsedTime);
	}

	// 弾とプレイヤーの当たり判定
	//ProjectileVSPlayer();

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// アニメーション更新
	animator->UpdateAnimation(elapsedTime);
	animator->UpdateRootMotion(transform);

	// 速力処理更新
	movement.UpdateVelocity(elapsedTime);

	// オブジェクト行列更新
	transform.UpdateTransform();

	// モデル行列更新
	model->UpdateTransform(transform.GetTransform());
}

void Wyvern::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

#pragma region デバッグ
// デバッグ描画
void Wyvern::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

	// ヒット判定のカプセル描画
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
		// ビヘイビアツリー
		wyvernTree->DebugBehaviorTree();

		// 状態
		DebugCondition();

		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			DirectX::XMFLOAT3 position = transform.GetPosition();
			ImGui::InputFloat3("Position", &position.x);
			transform.SetPosition(position);

			// 回転
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

			// スケール
			float s = transform.GetScale().x;
			ImGui::InputFloat("Scale", &s);

			transform.SetScale(s);
		}

		//ステータス
		if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::SliderInt("HP", &health, 0, maxHealth);
		}

		// ビヘイビア開始
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

		// パーティクル
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
	// 発見
	if (SearchTarget(searchRange, Player::Instance().transform.GetPosition()))
		condition |= Condition::C_Find;

	// エリア移動
	if (stayTime <= 0.0f)
		condition |= Condition::C_AreaMove;

	// 怒り
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