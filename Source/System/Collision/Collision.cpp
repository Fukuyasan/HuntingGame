#include "Collision.h"
#include "System/Math/Mathf.h"

#include <deque>

// 球と球の交差判定
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB
)
{
    // B->Aの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec       = DirectX::XMVectorSubtract(PositionA, PositionB);
    float             lengthSq  = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

    // 距離判定
    float range = radiusA + radiusB;
    if (lengthSq > range * range) return false;

    return true;
}

// 円柱と円柱の交差判定
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
    // Aの足元がBの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }

    // Aの頭がBの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

# if 0
    // XZ平面での範囲チェック
    // B->Aの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);

    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // 距離判定
    // range は Aの円の半径 ＋ Bの円の半径
    float range = radiusA + radiusB;
    // 距離の方が長かったらfalseを返す
    if (range * range < lengthSq) { return false; }

    // AがBを押し出す
    // ベクトルを正規化(単位ベクトル化)
    DirectX::XMVECTOR NorVec = DirectX::XMVector3Normalize(Vec);

    // 単位ベクトルと円の半径を足したものをかける
    NorVec = DirectX::XMVectorScale(NorVec, range);

    DirectX::XMStoreFloat3(&outPositionB, DirectX::XMVectorAdd(NorVec, PositionB));
#endif
    // XZ平面での範囲チェック
    float dx = positionA.x - positionB.x;
    float dz = positionA.z - positionB.z;

    float range = radiusA + radiusB;

    if (range * range < dx * dx + dz * dz)
    {
        return false;
    }

    // AがBを押し出す
    float length = sqrtf(dx * dx + dz * dz);
    // 正規化
    dx /= length;
    dz /= length;
    outPositionB.x = dx * range + positionB.x;
    outPositionB.y = positionA.y - positionB.y;
    outPositionB.z = dz * range + positionB.z;

    return true;
}

// 円と円柱の交差判定
bool Collision::IntersectSphereVsCylinder(
    const DirectX::XMFLOAT3& SpherePosition,
    float SphereRadius, 
    const DirectX::XMFLOAT3& CylinderPosition, 
    float CylinderRadius, 
    float CylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition)
{
    // 円の足元が円柱の頭より上なら当たっていない    
    if (SpherePosition.y - SphereRadius > CylinderPosition.y + CylinderHeight)
    {
        return false;
    }

    // 円の頭が円柱の足元より下なら当たっていない
    if (SpherePosition.y + SphereRadius < CylinderPosition.y)
    {
        return false;
    }

    // XZ平面での範囲チェック
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

// レイとモデルの交差判定
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

    // ワールド空間のレイの長さ
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        // メッシュノード取得
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        // レイをワールド空間からローカル空間へ変換
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        // DirectX::XMMatrixInverse( *pDeterminant, M ) : Mの行列の逆数を返す
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
        // DirectX::XMVector3TransformCoord( FXMVECTOR V,FXMMATRIX M ) : トランスフォームに変換したVectorを返す
        DirectX::XMVECTOR S      = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E      = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE     = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V      = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        // レイの長さ
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        // 三角形（面）との交差判定
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

                // 三角形の頂点を抽出
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                // 三角形の三辺ベクトルを算出
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                // 三角形の法線ベクトルを算出
                DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

                // 内積の結果がプラスなら裏向き
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);
                if (dot >= 0)continue;

                // レイと平面の交点を算出
                DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
                // DirectX::XMVectorDivide() :Vector型の割り算
                DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(SA, N), Dot);
                float x = DirectX::XMVectorGetX(X);
                DirectX::XMStoreFloat(&x, X);
                if (x < 0.0f || x > neart) continue;  // 交点までの距離が今までに計算した距離より
                                                     // 大きいときはスキップ
                DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

                // 交点が三角形の内側にあるか判定
                // 一つ目
                DirectX::XMVECTOR PA     = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
                DirectX::XMVECTOR Dot1   = DirectX::XMVector3Dot(Cross1, N);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);
                if (dot1 < 0)continue;
                // 二つ目
                DirectX::XMVECTOR PB     = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
                DirectX::XMVECTOR Dot2   = DirectX::XMVector3Dot(Cross2, N);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);
                if (dot2 < 0)continue;
                // 三つ目
                DirectX::XMVECTOR PC     = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
                DirectX::XMVECTOR Dot3   = DirectX::XMVector3Dot(Cross3, N);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);
                if (dot3 < 0)continue;

                // 最近距離を更新
                neart         = x;
                // 交点と法線を更新
                HitPosition   = P;
                HitNormal     = N;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            // ローカル空間からワールド空間へ変換
            DirectX::XMVECTOR WorldPosition    = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec    = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            // ヒット情報保存
            if (result.distance > distance)
            {
                // DirectxX::XMVector3TransformNormal(V,M) : Mの行列で変換されたベクトルをかえす
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

// 球とカプセルの当たり判定 (押し出しなし)
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

    // カプセルの始点から球の中心に向かうベクトルを求める
    DirectX::XMVECTOR SP_CSP = DirectX::XMVectorSubtract(SP, CSP);

    // カプセルの始点から終点に向かうベクトルを求める
    DirectX::XMVECTOR CGP_CSP = DirectX::XMVectorSubtract(CGP, CSP);

    // カプセルの長さ
    float capsuleLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(CGP_CSP));

    // ※射影ベクトルのため正規化
    CGP_CSP = DirectX::XMVector3Normalize(CGP_CSP);

    // 射影ベクトルを求める
    FLOAT Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(SP_CSP, CGP_CSP));

    // 最短の点を求める
    DirectX::XMVECTOR q = {};

    // 判定が正常にできない場合、補正をかける
    // 射影長がマイナスの場合
    if (Dot < 0)
        q = CSP;
    // 射影長がカプセルの長さよりも長い場合
    else if (Dot > capsuleLength)
        q = CGP;
    else
        q = DirectX::XMVectorAdd(CSP, DirectX::XMVectorScale(CGP_CSP, Dot));

    DirectX::XMVECTOR q_SP = DirectX::XMVectorSubtract(q, SP);
    // 交差判定
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

    // カプセルの始点から球の中心に向かうベクトルを求める
    DirectX::XMVECTOR SP_CSP = DirectX::XMVectorSubtract(SP, CSP);

    // カプセルの始点から終点に向かうベクトルを求める
    DirectX::XMVECTOR CGP_CSP = DirectX::XMVectorSubtract(CGP, CSP);

    // カプセルの長さ
    float capsuleLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(CGP_CSP));

    // ※射影ベクトルのため正規化
    CGP_CSP = DirectX::XMVector3Normalize(CGP_CSP);

    // 射影ベクトルを求める
    FLOAT Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(SP_CSP, CGP_CSP));

    // 最短の点を求める
    DirectX::XMVECTOR q = {};

    // 判定が正常にできない場合、補正をかける
    // 射影長がマイナスの場合
    if (Dot < 0)
        q = CSP;
    // 射影長がカプセルの長さよりも長い場合
    else if (Dot > capsuleLength)
        q = CGP;
    else
        q = DirectX::XMVectorAdd(CSP, DirectX::XMVectorScale(CGP_CSP, Dot));

    DirectX::XMVECTOR q_SP = DirectX::XMVectorSubtract(q, SP);
    // 交差判定
    float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(q_SP));
    float radiusLength = sphereRadius + capsuleRadius;

    // 当たってなければ false
    if (lengthSq > radiusLength * radiusLength) return false;

    // -----------------------------
    // 
    //    押し出し
    // 
    // -----------------------------

    float length = sqrtf(lengthSq);

    // めりーさん(めり込み)をもとめる
    float penetration = radiusLength - length;

    // Y軸は考えない
    float x = DirectX::XMVectorGetX(q_SP);
    float z = DirectX::XMVectorGetZ(q_SP);

    q_SP = DirectX::XMVectorSet(x, 0, z, 0);
 
    // ベクトルを正規化
    q_SP = DirectX::XMVector3Normalize(q_SP);

    // 押し出し比率を求める
    float mass = sphereMass + capsuleMass;

    float sMass = (sphereMass  / mass) * penetration;
    float cMass = (capsuleMass / mass) * penetration;

    // 押し出し
    DirectX::XMStoreFloat3(&spherePosition,  DirectX::XMVectorAdd(SP,  DirectX::XMVectorScale(q_SP, -cMass)));
    DirectX::XMStoreFloat3(&capsuleStartPos, DirectX::XMVectorAdd(CSP, DirectX::XMVectorScale(q_SP,  sMass)));

    return true;
}

// カプセルとカプセルの当たり判定
bool Collision::IntersectCapsuleVSCapsule(const DirectX::XMFLOAT3& capsuleStartPosA, const DirectX::XMFLOAT3& capsuleGoalPosA, float capsuleRadiusA, const DirectX::XMFLOAT3& capsuleStartPosB, const DirectX::XMFLOAT3& capsuleGoalPosB, float capsuleRadiusB)
{
    // それぞれのカプセルの中心に向かうベクトルを求める
    DirectX::XMVECTOR CGA_CSA = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&capsuleGoalPosA), DirectX::XMLoadFloat3(&capsuleStartPosA));
    DirectX::XMVECTOR CGB_CSB = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&capsuleGoalPosB), DirectX::XMLoadFloat3(&capsuleStartPosB));
    // カプセルの始点に向かうベクトルを求める
    DirectX::XMVECTOR CSA_CSB = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&capsuleStartPosA), DirectX::XMLoadFloat3(&capsuleStartPosB));

    // それぞれの長さ(二乗)を求める
    float CA_Len      = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGA_CSA, CGA_CSA));
    float CB_Len      = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGB_CSB, CGB_CSB));
    
    // カプセル同士の始点から内積を求める
    float CB_CSACSB_Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGB_CSB, CSA_CSB));

    float t0 = 0.0f, t1 = 0.0f;

    // 両線分が点に縮退している場合
    if (CA_Len <= FLT_EPSILON && CB_Len <= FLT_EPSILON)
    {
        t0 = t1 = 0.0f;
    }
    // CGA_CSAが点に縮退している場合
    else if (CA_Len <= FLT_EPSILON)
    {
        t0 = 0.0f;
        t1 = Mathf::Clamp(CB_CSACSB_Dot / CB_Len, 0.0f, 1.0f);
    }
    else
    {
        float CA_CSACSB_Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGA_CSA, CSA_CSB));

        // CGB_CSBが点に縮退している場合
        if (CB_Len <= FLT_EPSILON)
        {
            t1 = 0.0f;
            t0 = Mathf::Clamp(-CA_CSACSB_Dot / CA_Len, 0.0f, 1.0f);
        }
        else
        {
            float CA_CB_Dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CGA_CSA, CGB_CSB));
            float denom = CA_Len * CB_Len - CA_CB_Dot * CA_CB_Dot;

            // カプセル同士が平行かどうか
            if (denom != 0.0f)
                // 平行時はt0 = 0.0f（線分の始端）を仮の初期値として計算をすすめる
                t0 = Mathf::Clamp((CA_CB_Dot * CB_CSACSB_Dot - CA_CSACSB_Dot * CB_Len) / denom, 0.0f, 1.0f);

            t1 = CA_CB_Dot * t0 + CB_CSACSB_Dot;

            // t1が始端より外側にある場合
            if (t1 < 0.0f)
            {
                t1 = 0.0f;
                t0 = Mathf::Clamp(-CA_CSACSB_Dot / CA_Len, 0.0f, 1.0f);
            }
            // t1が終端より外側にある場合
            else if (t1 > CB_Len)
            {
                t1 = 1.0f;
                t0 = Mathf::Clamp((CA_CB_Dot - CA_CSACSB_Dot) / CA_Len, 0.0f, 1.0f);
            }
            // t1が線分上にある場合
            else
            {
                t1 /= CB_Len;
            }
        }
    }

    DirectX::XMFLOAT3 p0 = {}, p1 = {};

    // 最近点算出
    DirectX::XMStoreFloat3(&p0,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&capsuleStartPosA),DirectX::XMVectorScale(CGA_CSA,t0)));
    DirectX::XMStoreFloat3(&p1,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&capsuleStartPosB),DirectX::XMVectorScale(CGB_CSB,t1)));
    
    // 交差判定
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
    // 矩形の最短点を計算
    DirectX::XMFLOAT3 nearP = spherePosition;
    nearP.x = Mathf::Clamp(nearP.x, squarePosition.x - squareSize.x * 0.5f, squarePosition.x + squareSize.x * 0.5f);
    nearP.y = Mathf::Clamp(nearP.y, squarePosition.y - squareSize.y * 0.5f, squarePosition.y + squareSize.y * 0.5f);
    nearP.z = Mathf::Clamp(nearP.z, squarePosition.z - squareSize.z * 0.5f, squarePosition.z + squareSize.z * 0.5f);

    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&spherePosition), DirectX::XMLoadFloat3(&nearP));
    float lengthSq        = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

    if (lengthSq >= sphereRadius * sphereRadius) return false;
    
    // めり込みを求める
    float penetration = sphereRadius * sphereRadius - lengthSq;

    // ベクトルを正規化
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

    // 押し出し比率を求める
    float mass = sphereMass + squareMass;

    float spMass = (sphereMass / mass) * penetration;
    float sqMass = (squareMass / mass) * penetration;

    // 押し出し
    DirectX::XMStoreFloat3(&spherePosition, DirectX::XMVectorAdd(spPos, DirectX::XMVectorScale(Vec,  sqMass)));
    //DirectX::XMStoreFloat3(&squarePosition, DirectX::XMVectorAdd(sqPos, DirectX::XMVectorScale(Vec, -spMass)));

    return true;
}
