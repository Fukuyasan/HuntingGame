#pragma once

#include <DirectXMath.h>
#include "Graphics/Model/Model.h"

// ヒット結果
struct HitResult
{
    DirectX::XMFLOAT3 position      = { 0,0,0 };    // レイとポリゴンの交点
    DirectX::XMFLOAT3 normal        = { 0,0,0 };    // 衝突したポリゴンの法線ベクトル
    float             distance      = 0.0f;         // レイの始点から交点までの距離
    int               materialIndex = -1;           // 衝突したポリゴンのマテリアル番号
    DirectX::XMFLOAT3 rotation      = { 0,0,0 };    // 回転量
};

struct Sphere
{
    DirectX::XMFLOAT3 position;
    float             radius;
};

struct Cylinder
{
    DirectX::XMFLOAT3 position;
    float             radius;
    float             height;
};

struct Capsule
{
    DirectX::XMFLOAT3 startPosition;
    DirectX::XMFLOAT3 goalPosition;
    float             radius;
};

struct Square
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 size;
};

// コリジョン
class Collision
{
public:
    // 球と球の交差判定 (当たり判定のみ) 
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB
    );

    static bool IntersectSphereVsSphere(
        const Sphere& sphereA,
        const Sphere& sphereB,
        DirectX::XMFLOAT3& outPositionB
    );


    // 円柱と円柱の交差判定
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    static bool IntersectCylinderVsCylinder(
       const Cylinder& cylinderA,
       const Cylinder& cylinderB,
        DirectX::XMFLOAT3& outPositionB
    );

    // 円と円柱の交差判定
    static bool IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& SpherePosition,
        float SphereRadius,
        const DirectX::XMFLOAT3& CylinderPosition,
        float CylinderRadius,
        float CylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    static bool IntersectSphereVsCylinder(
        const Sphere& sphereA,
        const Cylinder& cylinderB,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    // レイとモデルの交差判定
    static bool IntersectRayVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result
    );

    // 円とカプセルの当たり判定 (押し出しなし)
    static bool IntersectSphereVsCapsule(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& capsuleStartPos,
        const DirectX::XMFLOAT3& capsuleGoalPos,
        float capsuleRadius      
    );

    static bool IntersectSphereVsCapsule(
        const Sphere&  sphereA,
        const Capsule& capsuleA
    );

    // 円とカプセルの当たり判定 (押し出しあり)
    static bool IntersectSphereVsCapsule(
        DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        float sphereMass,
        DirectX::XMFLOAT3& capsuleStartPos,
        DirectX::XMFLOAT3& capsuleGoalPos,
        float capsuleRadius,
        float capsuleMass
    );

    static bool IntersectSphereVsCapsule(
        const Sphere& sphereA,
        float sphereMass,
        const Capsule& capsuleA,
        float capsuleMass
    );

    // カプセルとカプセルの当たり判定
    static bool IntersectCapsuleVSCapsule(
        const DirectX::XMFLOAT3& capsuleStartPosA,
        const DirectX::XMFLOAT3& capsuleGoalPosA,
        float capsuleRadiusA,
        const DirectX::XMFLOAT3& capsuleStartPosB,
        const DirectX::XMFLOAT3& capsuleGoalPosB,
        float capsuleRadiusB
    );

    static bool IntersectCapsuleVSCapsule(
        const Capsule& capsuleA,
        const Capsule& capsuleB
    );


    // 球と矩形の当たり判定
    static bool IntersectSphereVSSquare(
        DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        DirectX::XMFLOAT3& velocitySp,
        float sphereMass,
        const DirectX::XMFLOAT3& SquarePosition,
        const DirectX::XMFLOAT3& SquareSize,
        DirectX::XMFLOAT3& squareVelocity,
        float SquareMass
    );

    static bool IntersectSphereVSSquare(
        const Sphere& sphere,
        const DirectX::XMFLOAT3& velocitySp,
        float sphereMass,
        const Square& Square,
        const DirectX::XMFLOAT3& velocitySq,
        float SquareMass
        );

};