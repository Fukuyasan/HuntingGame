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

#pragma region パラメータ設定
	// 体力設定
	health = maxHealth = 26000;

	// エリアの滞在時間
	stayTime = 120.0f;

	// 初期値設定
	areaNum = 0;
	MetaAI::UseAreaData areaData = MetaAI::Instance().GetAreaData(areaNum);

	DirectX::XMFLOAT3 firstPos = areaData.areaPos;
	float			  firstRange = areaData.areaRange;

	object->GetTransform().SetPosition(firstPos);
	SetTerritory(firstPos, firstRange);

	// 押し出し半径
	penentrationRadius = 1.0f;
	searchRange = 35.0f;

	// 当たり判定をJsonFIleから取得
	hitinformation.Decerialize("Data/Json/Dragon/");

	condition |= Condition::C_Movie;
#pragma endregion
}

void Dragon::Start()
{
	auto object = gameObject.lock();

	// コンポーネント取得
	movement = gameObject.lock()->GetComponent<Movement>();
	animator = gameObject.lock()->GetComponent<Animator>();

#pragma region モデル設定
	// 大きさにランダム性を持たせることで生物らしさを持たせる狙い
	const int minS = 10;
	const int maxS = 12;
	int scale = Mathf::RandomRange(minS, maxS);

	// モデルが大きいので、0.01 ~ 0.012の間に収める
	object->GetTransform().SetScale(0.001f * scale);
#pragma endregion

	eventInfo.Deserialize("Data/Model/Dragon/Dragon.event");

#pragma region ノード関係
	// LookAtIK用のノードを設定 : 首
	object->GetModel()->SetLookAtIK("Neck02");

	// ノード番号取得
	headIndex   = object->GetModel()->FindNodeIndex("Head");
	handIndex   = object->GetModel()->FindNodeIndex("R_Hand");
	tongueIndex = object->GetModel()->FindNodeIndex("Tongue01");
#pragma endregion

	// ビヘイビアツリー
	dragonTree = std::make_unique<DragonBehaviorTree>();
	dragonTree->RegisterBehaviorTree(this);

#pragma region エフェクト
	//smokeEffect = std::make_unique<Effect>("Data/Effect/smoke.efkefc");
	//fileEffect  = std::make_unique<Effect>("Data/Effect/fire.efkproj");

	// パーティクル設定
	// ブレス　※　怒り時のパーティクル
	breath = std::make_unique<BreathParticle>(300000);
	breath->Load("BreathParticle");

	// 噴火
	eruption = std::make_unique<BreathParticle>(300000);
	eruption->Load("EruptionParticle");
#pragma endregion

#pragma region 部位破壊
	ID3D11Device* device = Graphics::Instance().GetDevice();
	CreateShader::CreateCB(device, sizeof(CBClack), cbClack.ReleaseAndGetAddressOf());

	// 部位破壊テクスチャ
	Texture::LoadTexture("Data/Sprite/clack.png", device, clackTexture.GetAddressOf());
#pragma endregion
}

void Dragon::Update()
{
	auto object = gameObject.lock();

	float elapsedTime = TimeManager::Instance().GetDeltaTime();

	// 状態設定
	SetCondition();

	// ビヘイビアツリー更新
	if (debugFlg[Debug::PlayBehabior])
	{
		dragonTree->UpdateBehaviorTree(elapsedTime);
	}

	// 弾とプレイヤーの当たり判定
	ProjectileVSPlayer();

	// 速力処理更新
	movement->UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// クールタイム更新
	UpdateCoolTimer(elapsedTime);
	
	// アニメーション更新
	animator->UpdateAnimation(elapsedTime);

	// オブジェクト行列更新
	object->GetTransform().UpdateTransform();

	// 下を向く処理
	if (lookFlg)
	{
		underTime += elapsedTime * 2.0f;
		underRotate = Easing::OutQuart((std::min)(underTime, 1.0f), 1.0f, maxUnderRotate);  // 徐々に下を向く

		Model::Node& aimNode = object->GetModel()->GetAimNode();

		// Z軸を回転
		DirectX::XMStoreFloat4(&aimNode.rotate, DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0, 0, 1, 0), underRotate));
	}
	else
	{
		underTime = 0.0f;
	}

	// モデル行列更新
	object->GetModel()->UpdateTransform(object->GetTransform().GetTransform());

	// パーティクル更新
	breath->Update(elapsedTime);
	eruption->Update(elapsedTime);

	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
		
	// 部位破壊の定数バッファを更新
	clack.clackFlg = clackFlg;
	dc->UpdateSubresource(cbClack.Get(), 0, 0, &clack, 0, 0);
	dc->PSSetConstantBuffers(5, 1, cbClack.GetAddressOf());
	
	// 部位破壊用のテクスチャをシェーダー側に送る
	dc->PSSetShaderResources(22, 1, clackTexture.GetAddressOf());
}

//// 描画処理
//void Dragon::Render(ID3D11DeviceContext* dc, Shader* shader)
//{
//	
//
//	shader->Draw(dc, model.get());
//}

//// 着地した場合に呼ばれる
//void Dragon::OnLanding()
//{
//	Audio::Instance().PlayAudio(AudioList::DragonLand);
//
//	CameraController::Instance().SetShakeMode(0.3f, 5.0f, 5.0f);
//	smokeEffect->Play(gameObject.lock()->GetTransform().GetPosition(), 0.5f);
//}

#pragma region ダメージを受けたときの処理
// 死亡した時に呼ばれる
void Dragon::OnDead()
{
	Audio::Instance().PlayAudio(AudioList::DragonRoar);

	condition &= !Dragon::Condition::C_ALL;
	condition |= Condition::C_Die;

	isAlive = false;
}

// ダメージを受けたときに呼ばれる
void Dragon::OnDamaged()
{

}
#pragma endregion

// プレイヤーとノードの当たり判定
void Dragon::CollisionNodeVSPlayer(const unsigned int nodeIndex)
{
	//// ノードの位置と当たり判定を行う
	//const Model::Node& node = model->GetNodes().at(nodeIndex);

	//// ノードのワールド座標
	//DirectX::XMFLOAT3 nodePosition = GameMath::GetPositionFrom4x4(node.worldTransform);

	//const float attackPower = eventInfo.GetAttackData().attackPower;
	//SetAttackPower(attackPower * GetMagnification());

	//CollisionNodeVSPlayer(nodePosition);
}

void Dragon::CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position)
{
	//// 当たり判定表示
	//ShaderState::Instance().GetDebugRenderer()->AddSphere(
	//	position, attackRadius, DirectX::XMFLOAT4(1, 0, 0, 1)
	//);

	//// プレイヤーが無敵なら当たり判定しない
	//if (player.Invincible()) return;

	//Transform playerT = player.transform;

	//DirectX::XMFLOAT3 goalPos =
	//{
	//	playerT.GetPosition().x, 
	//	playerT.GetPosition().y + player.GetHeight(), 
	//	playerT.GetPosition().z 
	//};

	//// カプセルと円の当たり判定
	//if (Collision::IntersectSphereVsCapsule(
	//	position,
	//	attackRadius,
	//	playerT.GetPosition(),
	//	goalPos,
	//	player.GetRadius()
	//))
	//{
	//	// ダメージを与える
	//	if (player.ApplyDamage((int)attackPower, 0.5f))
	//	{
	//		// 吹っ飛ばす方向を計算
	//		DirectX::XMFLOAT3 vec;
	//		const float power = 40.0f;

	//		GameMath::Impulse(power, position, goalPos, vec);

	//		vec.y = 10.0f;

	//		// 吹き飛ばす方向を設定
	//		//player.SetImpulseVec(vec);

	//		// 吹っ飛ばす
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

	//// 全ての弾丸と全ての敵を総当たりで衝突処理
	//int projectileCount = ProjectileManager::Instance().GetProjectileCount();
	//for (int i = 0; i < projectileCount; ++i)
	//{
	//	// 弾のタイプが違うと continue
	//	Projectile* projectile = ProjectileManager::Instance().GetProjectile(i);
	//	if (projectile->GetType() != Type::Dragon) continue;

	//	// 衝突処理
	//	if (Collision::IntersectSphereVsCapsule(
	//		projectile->GetPosition(),
	//		projectile->GetRadius(),
	//		startPos,
	//		goalPos,
	//		player.GetRadius()
	//	))
	//	{
	//		// ダメージを与える
	//		if (!player.ApplyDamage((int)attackPower, 3.0f)) return;

	//		// 吹っ飛ばす方向を計算
	//		DirectX::XMFLOAT3 vec;
	//		const float power = 50.0f;

	//		DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&projectile->GetDirection()));
	//		DirectX::XMStoreFloat3(&vec, DirectX::XMVectorScale(Dir, power));
	//		vec.y = 10.0f;

	//		// 吹き飛ばす方向を設定
	//		player.SetImpulseVec(vec);

	//		// 吹っ飛ばす
	//		player.movement.AddImpulse(vec);

	//		bossSEs[SE::Explosion]->Play(false);

	//		projectile->Destroy();
	//	}
	//}
}

#pragma region デバッグ
// デバッグ描画
void Dragon::OnDebugPrimitive()
{
	//DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

	//// 縄張り範囲をデバッグ円柱描画
	//debugRenderer->AddCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

	//// ターゲット位置をデバッグ球描画
	//debugRenderer->AddSphere(targetPosition, radius, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

	//// 索敵範囲をデバッグ円柱描画
	//debugRenderer->AddCylinder(transform.GetPosition(), searchRange, 1.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	//// 攻撃範囲をデバッグ円柱描画
	//debugRenderer->AddCylinder(transform.GetPosition(), attackRange, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	//
	//// ヒット判定のカプセル描画
	//if (debugFlg[Debug::HitCapsule]) {
	//	for (HitInformation::HitData& hit : hitinformation.hitDatas) {
	//		DrawDebugHitData(
	//			debugRenderer,
	//			hit
	//		);
	//	}
	//}
}

// デバッグエネミー情報表示
void Dragon::OnDebugGUI()
{
	//hitinformation.DrawDebugGUI();
	//hitinformation.EditHitData(model.get());

	//ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	//if (ImGui::Begin("Dragon", nullptr, ImGuiWindowFlags_None))
	//{
	//	// ビヘイビアツリー
	//	dragonTree->DebugBehaviorTree();

	//	// 状態
	//	DebugCondition();

	//	//ステータス
	//	if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		ImGui::SliderInt("HP", &health, 0, maxHealth);
	//	}

	//	if (ImGui::CollapsingHeader("Look", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		ImGui::DragFloat("UnderRotate", &underRotate, 0.1f, -5.0f, 5.0f, "%.2f");
	//		ImGui::Checkbox("LookAt", &lookFlg);
	//	}

	//	// ビヘイビア開始
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

	//	// パーティクル
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

// 岩
void Dragon::RockShot(
	std::shared_ptr<ModelResource> resource,
	const DirectX::XMFLOAT3& position,
	const int rockCount
)
{
	//if (stopRockShot) return;

	//bossSEs[SE::Explosion]->Play(false);

	//// プレイヤーのY座標 + 高さ
	//float pPosY = Player::Instance().transform.GetPosition().y;
	//pPosY += Player::Instance().GetHeight();

	//// 三方向に飛ばす
	//DirectX::XMVECTOR Forward, Right;
	//Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&transform.GetForward()));
	//Right   = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&transform.GetRight()));

	//// パラメータ設定
	//StraiteParam param{};
	//param.position = position;
	//param.speed    = 50.0f;
	//param.radius   = 1.0f;
	//param.type     = Type::Dragon;

	//// 扇状に飛ばす
	//const float shotRate = 0.1f;
	//for (int i = 1; i <= rockCount; ++i)
	//{
	//	int a = (i & 1);	   // 奇数 1 : 偶数 0
	//	int b = 2 * a - 1;	   // 奇数 1 : 偶数 -1
	//	int c = (i - a) >> 1;  // 0, 2, 2, 4... を 2 で割るのと同義

	//	// 飛ばす方向(まっすぐ)
	//	DirectX::XMVECTOR Dir = DirectX::XMVectorLerp(Forward, DirectX::XMVectorScale(Right, (float)b), c * shotRate);
	//	DirectX::XMFLOAT3 dir;
	//	DirectX::XMStoreFloat3(&dir, Dir);

	//	// プレイヤーの高さに飛んでいくようにする
	//	float distY = pPosY - transform.GetPosition().y;
	//	dir.y += DirectX::XMConvertToRadians(distY);
	//	param.direction = dir;

	//	// 発射
	//	ProjectileStraite* projectile_straite = new ProjectileStraite(resource);
	//	ProjectileManager::Instance().Register(projectile_straite);
	//	projectile_straite->Launch(param);
	//}

	//stopRockShot = true;
}

void Dragon::SetCondition()
{
	//// 発見
	//if (SearchTarget(searchRange, Player::Instance().transform.GetPosition()))
	//	condition |= Condition::C_Find;

	//// エリア移動
	//if(stayTime <= 0.0f)
	//	condition |= Condition::C_AreaMove;
	//
	//// 怒り
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