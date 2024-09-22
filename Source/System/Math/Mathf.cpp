#include "Mathf.h"
#include <stdlib.h>
#include <string>

// 何度も生成するとランダムの偏りの原因となるので
// 一回だけ生成する
std::random_device Mathf::device;
std::mt19937       Mathf::mt(Mathf::device());

bool Mathf::RandomProbability(const float _probability)
{
    // 確率分布を指定
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    // 0.0から1.0までの範囲で乱数を生成し、確率と比較
    return distribution(mt) < _probability;
}

// 吹き飛ばす
void GameMath::Impulse(float power, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& goal, DirectX::XMFLOAT3& impulse)
{
    const DirectX::XMVECTOR P1 = DirectX::XMLoadFloat3(&goal);
    const DirectX::XMVECTOR P2 = DirectX::XMLoadFloat3(&start);

    // 吹き飛ばす方向を求める ※正規化
    const DirectX::XMVECTOR P1_P2 = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(P1, P2));

    // 吹き飛ばす方向に力を加える
    DirectX::XMStoreFloat3(&impulse, DirectX::XMVectorScale(P1_P2, power));
}

// 平面での長さを求める
float GameMath::LengthFromXZ(const DirectX::XMFLOAT3& _start, const DirectX::XMFLOAT3& _goal)
{
    return sqrtf(LengthSqFromXZ(_start, _goal));
}

// 平面での長さの二乗を求める
float GameMath::LengthSqFromXZ(const DirectX::XMFLOAT3& _start, const DirectX::XMFLOAT3& _goal)
{
    float vx = (_goal.x - _start.x) * (_goal.x - _start.x);
    float vz = (_goal.z - _start.z) * (_goal.z - _start.z);

    return vx + vz;
}

// 長さを求める
float GameMath::Length(const DirectX::XMFLOAT3& _start, const DirectX::XMFLOAT3& _goal)
{
    return sqrtf(LengthSq(_start, _goal));
}

// 長さの二乗を求める
float GameMath::LengthSq(const DirectX::XMFLOAT3& _start, const DirectX::XMFLOAT3& _goal)
{
    DirectX::XMVECTOR SP = DirectX::XMLoadFloat3(&_start);
    DirectX::XMVECTOR GP = DirectX::XMLoadFloat3(&_goal);

    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(GP, SP)));
}

// 行列から位置情報取得
DirectX::XMFLOAT3 GameMath::GetPositionFrom4x4(const DirectX::XMFLOAT4X4& _transform)
{   
    DirectX::XMFLOAT3 position{};
    DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&_transform);
    DirectX::XMStoreFloat3(&position, Transform.r[3]);
    return position;
}

bool GameMath::PermissionInAnimationSeconds(float _seconds, float _start, float _goal)
{
    return (_seconds >= _start) && (_seconds <= _goal);
}

void GameMath::ConvertWorldCoordinates(const DirectX::XMFLOAT3& _localPosition, const DirectX::XMFLOAT4X4& _worldTransform, DirectX::XMFLOAT3& _worldPosition)
{
    DirectX::XMVECTOR LocalPosition  = DirectX::XMLoadFloat3(&_localPosition);
    DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&_worldTransform);

    DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Transform(LocalPosition, WorldTransform);
    DirectX::XMStoreFloat3(&_worldPosition, WorldPosition);
}