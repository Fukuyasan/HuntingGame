#pragma once
#include "Component/Character/Enemy/Enemy.h"
#include "AI/StateMachine/StateMachine.h"

#ifdef Stego
class Stego final : public Enemy
{
public:
    enum class State
    {
        S_Idle,
        S_Run,
        S_Death,
    };

    enum class Animation
    {
        A_Death,
        A_Idle,
        A_Run,
    };

    enum Condition
    {
        C_None     = 0,
        C_Idle     = (1 << 0),
        C_AreaMove = (1 << 1),
    };

public:
	Stego();
	~Stego()override = default;

	void Update(const float& elapsedTime) override;
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// AI登録
	void RegisterStateMachine();

    // ステート切り替え
    void ChangeState(State _state)
    {
        stateMacine->ChangeState(static_cast<int>(_state));
    }

    void OnDamaged() override;

    void CollisionNodeVSPlayer(const DirectX::XMFLOAT3& position, float radius);

private:
#pragma region ステートマシーン
	std::unique_ptr<StateMachine<Stego>> stateMacine = nullptr;
#pragma endregion
};

#endif