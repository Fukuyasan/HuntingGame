#include "Stego.h"
#include "StegoState.h"

#include "AI/MetaAI/MetaAI.h"

#include "Graphics/ShaderState.h"

#include "Component/Character/Player/player.h"

#ifdef Stego

Stego::Stego()
{
	model       = std::make_unique<Model>("Data/Model/Dragon/Stegasaurus/Stegasaurus.mdl");
	model->LoadAnimation("Data/Model/Dragon/Stegasaurus/Stegasaurus.anim");

	animator    = std::make_unique<Animator>(*model.get());
	smokeEffect = std::make_unique<Effect>("Data/Effect/smokeWalk.efkefc");

	eventInfo.Deserialize("Data/Model/Dragon/Stegasaurus/Stegasaurus.event");

	stayTime = 0.0f;

	areaNum = 4;

	transform.SetPosition(MetaAI::Instance().GetAreaData(areaNum).areaPos);

	const float minS = 10;
	const float maxS = 20;

	float scale = Mathf::RandomRange(minS, maxS);

	transform.SetScale(0.001f * scale);

	RegisterStateMachine();

	SetCoolTimer(0.2f);
}

void Stego::Update(const float& elapsedTime)
{
	// 時間経過で移動
	stayTime -= (std::max)(elapsedTime, 0.0f);
	if (stayTime <= 0.0f)
		condition |= Condition::C_AreaMove;

	stateMacine->Update(elapsedTime);

	// オブジェクト行列を更新
	transform.UpdateTransform();

	// モデルアニメーション更新
	animator->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform.GetTransform());
}

void Stego::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

void Stego::RegisterStateMachine()
{
	stateMacine = std::make_unique<StateMachine<Stego>>();

	stateMacine->RegisterState(std::make_shared<StegoIdleState>(this));
	stateMacine->RegisterState(std::make_shared<StegoRunState>(this));
	stateMacine->RegisterState(std::make_shared<StegoDeathState>(this));

	stateMacine->SetState(static_cast<int>(State::S_Idle));
}

void Stego::OnDamaged()
{
	ChangeState(State::S_Death);
}

void Stego::CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position, float radius)
{
	// 当たり判定表示
	ShaderState::Instance().GetDebugRenderer()->AddSphere(
		position, radius, DirectX::XMFLOAT4(1, 0, 0, 1)
	);

	// プレイヤーと当たり判定
	Player& player    = Player::Instance();
	Transform playerT = player.transform;

	DirectX::XMFLOAT3 goalPos =
	{
		playerT.GetPosition().x,
		playerT.GetPosition().y + player.GetHeight(),
		playerT.GetPosition().z
	};

	// カプセルと円の当たり判定
	if (Collision::IntersectSphereVsCapsule(
		position,
		radius,
		playerT.GetPosition(),
		goalPos,
		player.GetRadius()
	))
	{
		// ダメージを与える
		if (player.ApplyDamage((int)attackPower, 20.0f))
		{
			// 吹っ飛ばす方向を計算
			DirectX::XMFLOAT3 vec;
			const float power = Mathf::RandomRange(10.0f, 50.0f);

			GameMath::Impulse(power, position, goalPos, vec);

			vec.y = Mathf::RandomRange(10.0f, 50.0f);

			// 吹き飛ばす方向を設定
			player.SetImpulseVec(vec);

			// 吹っ飛ばす
			player.movement.AddImpulse(vec);
		}
	}
}

#endif
