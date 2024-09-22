#pragma once

#include <DirectXMath.h>
#include "Graphics/Model/Model.h"

// �q�b�g����
struct HitResult
{
    DirectX::XMFLOAT3 position      = { 0,0,0 };    // ���C�ƃ|���S���̌�_
    DirectX::XMFLOAT3 normal        = { 0,0,0 };    // �Փ˂����|���S���̖@���x�N�g��
    float             distance      = 0.0f;         // ���C�̎n�_�����_�܂ł̋���
    int               materialIndex = -1;           // �Փ˂����|���S���̃}�e���A���ԍ�
    DirectX::XMFLOAT3 rotation      = { 0,0,0 };    // ��]��
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

// �R���W����
class Collision
{
public:
    // ���Ƌ��̌������� (�����蔻��̂�) 
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


    // �~���Ɖ~���̌�������
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

    // �~�Ɖ~���̌�������
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

    // ���C�ƃ��f���̌�������
    static bool IntersectRayVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result
    );

    // �~�ƃJ�v�Z���̓����蔻�� (�����o���Ȃ�)
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

    // �~�ƃJ�v�Z���̓����蔻�� (�����o������)
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

    // �J�v�Z���ƃJ�v�Z���̓����蔻��
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


    // ���Ƌ�`�̓����蔻��
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