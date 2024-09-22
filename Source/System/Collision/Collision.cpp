#include "Collision.h"
#include "System/Math/Mathf.h"

#include <deque>

// ���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB
)
{
    // B->A�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec       = DirectX::XMVectorSubtract(PositionA, PositionB);
    float             lengthSq  = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

    // ��������
    float range = radiusA + radiusB;
    if (lengthSq > range * range) return false;

    return true;
}

// �~���Ɖ~���̌�������
bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB, 
    float radiusB, 
    float heightB,
    DirectX::XMFLOAT3& outPositionB
)
{
    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }

    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

# if 0
    // XZ���ʂł͈̔̓`�F�b�N
    // B->A�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);

    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // ��������
    // range �� A�̉~�̔��a �{ B�̉~�̔��a
    float range = radiusA + radiusB;
    // �����̕�������������false��Ԃ�
    if (range * range < lengthSq) { return false; }

    // A��B�������o��
    // �x�N�g���𐳋K��(�P�ʃx�N�g����)
    DirectX::XMVECTOR NorVec = DirectX::XMVector3Normalize(Vec);

    // �P�ʃx�N�g���Ɖ~�̔��a�𑫂������̂�������
    NorVec = DirectX::XMVectorScale(NorVec, range);

    DirectX::XMStoreFloat3(&outPositionB, DirectX::XMVectorAdd(NorVec, PositionB));
#endif
    // XZ���ʂł͈̔̓`�F�b�N
    float dx = positionA.x - positionB.x;
    float dz = positionA.z - positionB.z;

    float range = radiusA + radiusB;

    if (range * range < dx * dx + dz * dz)
    {
        return false;
    }

    // A��B�������o��
    float length = sqrtf(dx * dx + dz * dz);
    // ���K��
    dx /= length;
    dz /= length;
    outPositionB.x = dx * range + positionB.x;
    outPositionB.y = positionA.y - positionB.y;
    outPositionB.z = dz * range + positionB.z;

    return true;
}

// �~�Ɖ~���̌�������
bool Collision::IntersectSphereVsCylinder(
    const DirectX::XMFLOAT3& SpherePosition,
    float SphereRadius, 
    const DirectX::XMFLOAT3& CylinderPosition, 
    float CylinderRadius, 
    float CylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition)
{
    // �~�̑������~���̓�����Ȃ瓖�����Ă��Ȃ�    
    if (SpherePosition.y - SphereRadius > CylinderPosition.y + CylinderHeight)
    {
        return false;
    }

    // �~�̓����~���̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (SpherePosition.y + SphereRadius < CylinderPosition.y)
    {
        return false;
    }

    // XZ���ʂł͈̔̓`�F�b�N
    float dx = SpherePosition.x - CylinderPosition.x;
    float dz = SpherePosition.z - CylinderPosition.z;

    float range = SphereRadius + CylinderRadius;

    if (range * range < dx * dx + dz * dz)
    {
        return false;
    }

    outCylinderPosition.x = dx * range + CylinderPosition.x;
    outCylinderPosition.y = SpherePosition.y - CylinderPosition.y;
    outCylinderPosition.z = dz * range + CylinderPosition.z;

    return true;
}

// ���C�ƃ��f���̌�������
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
    DirectX::XMVECTOR WorldStart     = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd       = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec    = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    // ���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        // ���b�V���m�[�h�擾
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        // ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        // DirectX::XMMatrixInverse( *pDeterminant, M ) : M�̍s��̋t����Ԃ�
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
        // DirectX::XMVector3TransformCoord( FXMVECTOR V,FXMMATRIX M ) : �g�����X�t�H�[���ɕϊ�����Vector��Ԃ�
        DirectX::XMVECTOR S      = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E      = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE     = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V      = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        // ���C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        // �O�p�`�i�ʁj�Ƃ̌�������
        const auto& vertices = mesh.vertices;
        const auto& indices  = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;

        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                // �O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                // �O�p�`�̎O�Ӄx�N�g�����Z�o
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                // �O�p�`�̖@���x�N�g�����Z�o
                DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

                // ���ς̌��ʂ��v���X�Ȃ痠����
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);
                if (dot >= 0)continue;

                // ���C�ƕ��ʂ̌�_���Z�o
                DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
                // DirectX::XMVectorDivide() :Vector�^�̊���Z
                DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(SA, N), Dot);
                float x = DirectX::XMVectorGetX(X);
                DirectX::XMStoreFloat(&x, X);
                if (x < 0.0f || x > neart) continue;  // ��_�܂ł̋��������܂łɌv�Z�����������
                                                     // �傫���Ƃ��̓X�L�b�v
                DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

                // ��_���O�p�`�̓����ɂ��邩����
                // ���
                DirectX::XMVECTOR PA     = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
                DirectX::XMVECTOR Dot1   = DirectX::XMVector3Dot(Cross1, N);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);
                if (dot1 < 0)continue;
                // ���
                DirectX::XMVECTOR PB     = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
                DirectX::XMVECTOR Dot2   = DirectX::XMVector3Dot(Cross2, N);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);
                if (dot2 < 0)continue;
                // �O��
                DirectX::XMVECTOR PC     = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
                DirectX::XMVECTOR Dot3   = DirectX::XMVector3Dot(Cross3, N);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);
                if (dot3 < 0)continue;

                // �ŋߋ������X�V
                neart         = x;
                // ��_�Ɩ@�����X�V
                HitPosition   = P;
                HitNormal     = N;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            // ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR WorldPosition    = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec    = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            // �q�b�g���ۑ�
            if (result.distance > distance)
            {
                // DirectxX::XMVector3TransformNormal(V,M) : M�̍s��ŕϊ����ꂽ�x�N�g����������
                DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal,WorldTransform);

                result.distance      = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                hit = true;
            }
        }
    }

    return hit;
}

// ���ƃJ�v�Z���̓����蔻�� (�����o���Ȃ�)
bool Collision::IntersectSphereVsCapsule(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius, 
    const DirectX::XMFLOAT3& capsuleStartPos,
    const DirectX::XMFLOAT3& capsuleGoalPos, 
    float capsuleRadius
)
{
    DirectX::XMVECTOR SP  = DirectX::XMLoadFloat3(&spherePosition);
    DirectX::XMVECTOR CSP = DirectX::XMLoadFloat3(&capsuleStartPos);
    DirectX::XMVECTOR CGP = DirectX::XMLoadFloat3(&capsuleGoalPos);

    // �J�v�Z���̎n�_���狅�̒��S�Ɍ������x�N�g�������߂�
    DirectX::XMVECTOR SP_CSP = DirectX::XMVectorSubtract(SP, CSP);

    // �J�v�Z���̎n�_����I�_�Ɍ������x�N�g�������߂�
    DirectX::XMVECTOR CGP_CSP = DirectX::XMVectorSubtract(CGP, CSP);

    // �J�v�Z���̒���
    float capsuleLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(CGP_CSP));

    // ���ˉe�x�N�g���̂��ߐ��K��
    CGP_CSP = DirectX::XMVector3Normalize(CGP_CSP);

    // �ˉe�x�N�g�������߂�
    FLOAT Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(SP_CSP, CGP_CSP));

    // �ŒZ�̓_�����߂�
    DirectX::XMVECTOR q = {};

    // ���肪����ɂł��Ȃ��ꍇ�A�␳��������
    // �ˉe�����}�C�i�X�̏ꍇ
    if (Dot < 0)
        q = CSP;
    // �ˉe�����J�v�Z���̒������������ꍇ
    else if (Dot > capsuleLength)
        q = CGP;
    else
        q = DirectX::XMVectorAdd(CSP, DirectX::XMVectorScale(CGP_CSP, Dot));

    DirectX::XMVECTOR q_SP = DirectX::XMVectorSubtract(q, SP);
    // ��������
    float lengthSq     = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(q_SP));
    float radiusLength = sphereRadius + capsuleRadius;


    return lengthSq < radiusLength * radiusLength;
}


bool Collision::IntersectSphereVsCapsule(
    DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    float sphereMass,
    DirectX::XMFLOAT3& capsuleStartPos,
    DirectX::XMFLOAT3& capsuleGoalPos,
    float capsuleRadius,
    float capsuleMass
)
{
    DirectX::XMVECTOR SP  = DirectX::XMLoadFloat3(&spherePosition);
    DirectX::XMVECTOR CSP = DirectX::XMLoadFloat3(&capsuleStartPos);
    DirectX::XMVECTOR CGP = DirectX::XMLoadFloat3(&capsuleGoalPos);

    // �J�v�Z���̎n�_���狅�̒��S�Ɍ������x�N�g�������߂�
    DirectX::XMVECTOR SP_CSP = DirectX::XMVectorSubtract(SP, CSP);

    // �J�v�Z���̎n�_����I�_�Ɍ������x�N�g�������߂�
    DirectX::XMVECTOR CGP_CSP = DirectX::XMVectorSubtract(CGP, CSP);

    // �J�v�Z���̒���
    float capsuleLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(CGP_CSP));

    // ���ˉe�x�N�g���̂��ߐ��K��
    CGP_CSP = DirectX::XMVector3Normalize(CGP_CSP);

    // �ˉe�x�N�g�������߂�
    FLOAT Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(SP_CSP, CGP_CSP));

    // �ŒZ�̓_�����߂�
    DirectX::XMVECTOR q = {};

    // ���肪����ɂł��Ȃ��ꍇ�A�␳��������
    // �ˉe�����}�C�i�X�̏ꍇ
    if (Dot < 0)
        q = CSP;
    // �ˉe�����J�v�Z���̒������������ꍇ
    else if (Dot > capsuleLength)
        q = CGP;
    else
        q = DirectX::XMVectorAdd(CSP, DirectX::XMVectorScale(CGP_CSP, Dot));

    DirectX::XMVECTOR q_SP = DirectX::XMVectorSubtract(q, SP);
    // ��������
    float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(q_SP));
    float radiusLength = sphereRadius + capsuleRadius;

    // �������ĂȂ���� false
    if (lengthSq > radiusLength * radiusLength) return false;

    // -----------------------------
    // 
    //    �����o��
    // 
    // -----------------------------

    float length = sqrtf(lengthSq);

    // �߂�[����(�߂荞��)�����Ƃ߂�
    float penetration = radiusLength - length;

    // Y���͍l���Ȃ�
    float x = DirectX::XMVectorGetX(q_SP);
    float z = DirectX::XMVectorGetZ(q_SP);

    q_SP = DirectX::XMVectorSet(x, 0, z, 0);
 
    // �x�N�g���𐳋K��
    q_SP = DirectX::XMVector3Normalize(q_SP);

    // �����o���䗦�����߂�
    float mass = sphereMass + capsuleMass;

    float sMass = (sphereMass  / mass) * penetration;
    float cMass = (capsuleMass / mass) * penetration;

    // �����o��
    DirectX::XMStoreFloat3(&spherePosition,  DirectX::XMVectorAdd(SP,  DirectX::XMVectorScale(q_SP, -cMass)));
    DirectX::XMStoreFloat3(&capsuleStartPos, DirectX::XMVectorAdd(CSP, DirectX::XMVectorScale(q_SP,  sMass)));

    return true;
}

// �J�v�Z���ƃJ�v�Z���̓����蔻��
bool Collision::IntersectCapsuleVSCapsule(const DirectX::XMFLOAT3& capsuleStartPosA, const DirectX::XMFLOAT3& capsuleGoalPosA, float capsuleRadiusA, const DirectX::XMFLOAT3& capsuleStartPosB, const DirectX::XMFLOAT3& capsuleGoalPosB, float capsuleRadiusB)
{
    // ���ꂼ��̃J�v�Z���̒��S�Ɍ������x�N�g�������߂�
    DirectX::XMVECTOR CGA_CSA = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&capsuleGoalPosA), DirectX::XMLoadFloat3(&capsuleStartPosA));
    DirectX::XMVECTOR CGB_CSB = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&capsuleGoalPosB), DirectX::XMLoadFloat3(&capsuleStartPosB));
    // �J�v�Z���̎n�_�Ɍ������x�N�g�������߂�
    DirectX::XMVECTOR CSA_CSB = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&capsuleStartPosA), DirectX::XMLoadFloat3(&capsuleStartPosB));

    // ���ꂼ��̒���(���)�����߂�
    float CA_Len      = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGA_CSA, CGA_CSA));
    float CB_Len      = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGB_CSB, CGB_CSB));
    
    // �J�v�Z�����m�̎n�_������ς����߂�
    float CB_CSACSB_Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGB_CSB, CSA_CSB));

    float t0 = 0.0f, t1 = 0.0f;

    // ���������_�ɏk�ނ��Ă���ꍇ
    if (CA_Len <= FLT_EPSILON && CB_Len <= FLT_EPSILON)
    {
        t0 = t1 = 0.0f;
    }
    // CGA_CSA���_�ɏk�ނ��Ă���ꍇ
    else if (CA_Len <= FLT_EPSILON)
    {
        t0 = 0.0f;
        t1 = Mathf::Clamp(CB_CSACSB_Dot / CB_Len, 0.0f, 1.0f);
    }
    else
    {
        float CA_CSACSB_Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGA_CSA, CSA_CSB));

        // CGB_CSB���_�ɏk�ނ��Ă���ꍇ
        if (CB_Len <= FLT_EPSILON)
        {
            t1 = 0.0f;
            t0 = Mathf::Clamp(-CA_CSACSB_Dot / CA_Len, 0.0f, 1.0f);
        }
        else
        {
            float CA_CB_Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGA_CSA, CGB_CSB));
            float denom = CA_Len * CB_Len - CA_CB_Dot * CA_CB_Dot;

            // �J�v�Z�����m�����s���ǂ���
            if (denom != 0.0f)
                // ���s����t0 = 0.0f�i�����̎n�[�j�����̏����l�Ƃ��Čv�Z�������߂�
                t0 = Mathf::Clamp((CA_CB_Dot * CB_CSACSB_Dot - CA_CSACSB_Dot * CB_Len) / denom, 0.0f, 1.0f);

            t1 = CA_CB_Dot * t0 + CB_CSACSB_Dot;

            // t1���n�[���O���ɂ���ꍇ
            if (t1 < 0.0f)
            {
                t1 = 0.0f;
                t0 = Mathf::Clamp(-CA_CSACSB_Dot / CA_Len, 0.0f, 1.0f);
            }
            // t1���I�[���O���ɂ���ꍇ
            else if (t1 > CB_Len)
            {
                t1 = 1.0f;
                t0 = Mathf::Clamp((CA_CB_Dot - CA_CSACSB_Dot) / CA_Len, 0.0f, 1.0f);
            }
            // t1��������ɂ���ꍇ
            else
            {
                t1 /= CB_Len;
            }
        }
    }

    DirectX::XMFLOAT3 p0 = {}, p1 = {};

    // �ŋߓ_�Z�o
    DirectX::XMStoreFloat3(&p0,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&capsuleStartPosA),DirectX::XMVectorScale(CGA_CSA,t0)));
    DirectX::XMStoreFloat3(&p1,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&capsuleStartPosB),DirectX::XMVectorScale(CGB_CSB,t1)));
    
    // ��������
    DirectX::XMVECTOR P1_P0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p0), DirectX::XMLoadFloat3(&p1));

    float range = capsuleRadiusA + capsuleRadiusB;

    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(P1_P0, P1_P0)) < range * range;
}

bool Collision::IntersectSphereVSSquare(
    DirectX::XMFLOAT3& spherePosition, 
    float sphereRadius,
    DirectX::XMFLOAT3& sphereVelocity,
    float sphereMass,
    const DirectX::XMFLOAT3& squarePosition,
    const DirectX::XMFLOAT3& squareSize,
    DirectX::XMFLOAT3& squareVelocity,
    float squareMass)
{
    // ��`�̍ŒZ�_���v�Z
    DirectX::XMFLOAT3 nearP = spherePosition;
    nearP.x = Mathf::Clamp(nearP.x, squarePosition.x - squareSize.x * 0.5f, squarePosition.x + squareSize.x * 0.5f);
    nearP.y = Mathf::Clamp(nearP.y, squarePosition.y - squareSize.y * 0.5f, squarePosition.y + squareSize.y * 0.5f);
    nearP.z = Mathf::Clamp(nearP.z, squarePosition.z - squareSize.z * 0.5f, squarePosition.z + squareSize.z * 0.5f);

    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&spherePosition), DirectX::XMLoadFloat3(&nearP));
    float lengthSq        = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

    if (lengthSq >= sphereRadius * sphereRadius) return false;
    
    // �߂荞�݂����߂�
    float penetration = sphereRadius * sphereRadius - lengthSq;

    // �x�N�g���𐳋K��
    Vec = DirectX::XMVector3Normalize(Vec);

    DirectX::XMVECTOR spPos = DirectX::XMLoadFloat3(&spherePosition);
    DirectX::XMVECTOR sqPos = DirectX::XMLoadFloat3(&squarePosition);

    if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec)) < FLT_EPSILON)
    {
        Vec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(spPos, sqPos));
    }

    DirectX::XMVECTOR spVelocity = DirectX::XMLoadFloat3(&sphereVelocity);
    DirectX::XMVECTOR sqVelocity = DirectX::XMLoadFloat3(&squareVelocity);
    float v1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(spVelocity, Vec));
    float v2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sqVelocity, Vec));
    if (v1 - v2 > 0)
    {
        const float restitution = 0.95f;
        float coefficient_1 = (1 + restitution) / (sphereMass / squareMass + 1);
        float coefficient_2 = (1 + restitution) / (squareMass / sphereMass + 1);
        DirectX::XMStoreFloat3(&sphereVelocity, DirectX::XMVectorAdd(DirectX::XMVectorScale(Vec, (-v1 + v2) * coefficient_1), spVelocity));
        DirectX::XMStoreFloat3(&squareVelocity, DirectX::XMVectorAdd(DirectX::XMVectorScale(Vec, (-v2 + v1) * coefficient_2), sqVelocity));
    }

    // �����o���䗦�����߂�
    float mass = sphereMass + squareMass;

    float spMass = (sphereMass / mass) * penetration;
    float sqMass = (squareMass / mass) * penetration;

    // �����o��
    DirectX::XMStoreFloat3(&spherePosition, DirectX::XMVectorAdd(spPos, DirectX::XMVectorScale(Vec,  sqMass)));
    //DirectX::XMStoreFloat3(&squarePosition, DirectX::XMVectorAdd(sqPos, DirectX::XMVectorScale(Vec, -spMass)));

    return true;
}
