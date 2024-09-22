#pragma once
#include "AI/BehaviorTree/ActionBase.h"
#include "Component/Character/Enemy/Wyvern/Wyvern.h"

#ifdef Wyvern

#pragma region UŒ‚
// ’Ü‚ÅUŒ‚
class A_WyvernClaw : public ActionBase<Wyvern>
{
public:
	A_WyvernClaw(Wyvern* wyvern) :ActionBase<Wyvern>(wyvern) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
private:
	std::shared_ptr<ModelResource> resource;

	const char* filenames[2];
	int random = -1;
};

// ŠpUŒ‚
class A_WyvernHorn : public ActionBase<Wyvern>
{
public:
	A_WyvernHorn(Wyvern* wyvern) : ActionBase<Wyvern>(wyvern) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

};

// Šš‚İ‚Â‚«
class A_WyvernBite : public ActionBase<Wyvern>
{
public:
	A_WyvernBite(Wyvern* wyvern) : ActionBase<Wyvern>(wyvern) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

};
#pragma endregion

// ™ôšK
class A_WyvernRoar : public ActionBase<Wyvern>
{
public:
	A_WyvernRoar(Wyvern* wyvern, const float power = 0.0f) : ActionBase<Wyvern>(wyvern), roarPower(power) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;

private:
	DirectX::XMFLOAT4 normalColor = { 1.0f,   1.0f,   1.0f, 1.0f };
	DirectX::XMFLOAT4 angryColor = { 0.735f, 0.287f, 0.137f, 3.0f };

	float roarPower = 0.0f;
	float lerpTime = 0.0f;
};

#pragma region œpœjs“®
// œpœjs“®
class A_WyvernWander : public ActionBase<Wyvern>
{
public:
	A_WyvernWander(Wyvern* wyvern) :ActionBase<Wyvern>(wyvern) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

// ‘Ò‹@s“®
class A_WyvernIdle : public ActionBase<Wyvern>
{
public:
	A_WyvernIdle(Wyvern* wyvern) :ActionBase<Wyvern>(wyvern) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};
#pragma endregion

#pragma region ƒ_ƒ[ƒW‚ğó‚¯‚½‚Æ‚«‚Ìs“®
// ‹¯‚İs“®
class A_WyvernFlinch : public ActionBase<Wyvern>
{
public:
	A_WyvernFlinch(Wyvern* wyvern) : ActionBase<Wyvern>(wyvern) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
};

// €–Ss“®
class A_WyvernDie : public ActionBase<Wyvern>
{
public:
	A_WyvernDie(Wyvern* wyvern) : ActionBase<Wyvern>(wyvern) {}

	bool Start() override;
	ActionState Run(const float& elapsedTime)override;
private:
	DirectX::XMFLOAT4 normalColor = { 1.0f,   1.0f,   1.0f, 1.0f };
	DirectX::XMFLOAT4 angryColor = { 0.735f, 0.287f, 0.137f, 3.0f };

	float lerpRate = 0.0f;
};
#pragma endregion

#endif