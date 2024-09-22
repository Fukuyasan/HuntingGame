#include "Movement.h"
#include "System/Time/TimeManager.h"
#include "System/Collision/Collision.h"

#include "GameObject/GameObject.h"
#include "GameObject/GameObjectManager.h"

void Movement::Move(float vx, float vz, float speed)
{
    // 移動ベクトルを設定
    m_moveVecX = vx;
    m_moveVecZ = vz;

    // 最大速度設定
    m_maxMoveSpeed = speed;
}

void Movement::MoveDirection(const DirectX::XMFLOAT3& direction, float speed)
{
    Move(direction.x, direction.z, speed);
}

void Movement::Turn(const float& elapsedTime, float vx, float vz, float speed)
{
    std::shared_ptr<GameObject> object = gameObject.lock();

    speed *= elapsedTime;

    // 進行ベクトルがゼロベクトルの場合は処理する必要なし
    float length = vx * vx + vz * vz;
    if (length == 0) return;

    length = sqrtf(length);

    // 進行ベクトルを単位ベクトル化
    vx /= length;
    vz /= length;

    // 自身の回転値から前方向を求める
    float frontX = sinf(object->transform.GetAngle().y);
    float frontZ = cosf(object->transform.GetAngle().y);
    // ※ angleから方向がわかる

    // 回転角を求めるため、2つの単位ベクトルの内積を計算する
    float dot = (frontX * vx) + (frontZ * vz);

    // 内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度が
    // 小さいほど1.0に近づくという性質を利用して回転速度を調整する
    float rot = (std::min)(1.0f - dot, speed);  // 補正値

    // 左右判定を行うために2つの単位ベクトルの外積を計算する
    float cross = (frontZ * vx) - (frontX * vz);

    // 2dの外積値が正の場合か負の場合によって左右判定が行える
    // 左右判定を行うことによって左右回転を選択する
    if (cross < 0.0f) {
        object->transform.m_angle.y -= rot;
    }
    else {
        object->transform.m_angle.y += rot;
    }
}

void Movement::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // 速力に力を加える
    m_velocity.x += impulse.x;
    m_velocity.y += impulse.y;
    m_velocity.z += impulse.z;
}

void Movement::UpdateVelocity(const float& elapsedTime)
{
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;

    // 垂直速力更新処理
    UpdateVerticalVelocity(elapsedFrame);

    // 水平速力更新処理
    UpdateHorizontalVelocity(elapsedFrame);

    // 垂直移動更新処理
    UpdateVerticalMove(elapsedTime);

    // 水平移動更新処理
    UpdateHorizontalMove(elapsedTime);
}

void Movement::UpdateVerticalVelocity(float elapsedFrame)
{
    // 重力処理
    m_velocity.y += m_gravity * elapsedFrame;
}

void Movement::UpdateVerticalMove(float elapsedTime)
{
    auto object = gameObject.lock();

    // 垂直方向の移動量
    float my = m_velocity.y * elapsedTime;

    // キャラクターのY方向となる法線ベクトル
    DirectX::XMFLOAT3 normal = { 0,1,0 };

    // 傾斜率を初期化
    m_slopeRate = 0.0f;
    
    // 上昇中
    if (my > 0.0f)
    {
        object->transform.m_position.y += my;
        //isGround = false;
    }
    // 落下中
    else
    {
        // レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start =
        {
            object->transform.m_position.x,
            object->transform.m_position.y + m_stepOffset,
            object->transform.m_position.z
        };

        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end =
        {
            object->transform.m_position.x,
            object->transform.m_position.y + my,
            object->transform.m_position.z };

        // レイキャストによる地面判定
        auto stage = GameObjectManager::Instance().Find("Stage");
        HitResult hit;
        if (Collision::IntersectRayVsModel(start, end, stage->GetModel(), hit))
        {
            // 法線ベクトルの取得
            normal = hit.normal;

            // 傾斜率の計算　傾斜率 = 高さ / 底辺＋高さ）
            DirectX::XMFLOAT2 normal2 = { normal.x, normal.z };
            float length = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMLoadFloat2(&normal2)));

            // 遮蔽率を求める
            m_slopeRate = 1.0f - normal.y / (length + normal.y);

            // 地面に接地している
            object->transform.m_position.y = hit.position.y;

            // 回転
            object->transform.m_angle.y += hit.rotation.y;

            // 着地した
            if (!m_isGround)
            {
                //m_isLanding = true;
            }
            m_isGround = true;
            m_velocity.y = 0.0f;
        }
        else
        {
            // 空中に浮いている
            object->transform.m_position.y += my;
            //m_isGround  = false;
            //m_isLanding = false;
        }
    }

#if 0
    // 地面の向きに沿うようにXZ軸回転
    {
        // Y軸が法線ベクトル方向に向くオイラー角を算出する
        float ax = atan2f(normal.z, normal.y);
        float az = -atan2f(normal.x, normal.y);

        // 線形補間で滑らかに回転するdd
        angle.x = Mathf::lerp(angle.x, ax, 0.2f);
        angle.z = Mathf::lerp(angle.z, az, 0.2f);
    }
#endif
}

void Movement::UpdateHorizontalVelocity(float elapsedFrame)
{
    // XZ平面の速力を減速する
    float length = m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z;
    if (length < 0.0f) return;

    // 摩擦力
    float friction = this->m_friction * elapsedFrame;

    //// 空中にいるときは摩擦力を減らす
    //if (!isGround) {
    //    m_friction *= m_airControl;
    //}

    // 摩擦による横方面の減速処理
    if (length > friction * friction)
    {
        length = sqrtf(length);

        // 単位ベクトル化
        float vx = m_velocity.x / length;
        float vz = m_velocity.z / length;

        m_velocity.x -= vx * friction;
        m_velocity.z -= vz * friction;
    }
    // 横方向の速力が摩擦力以下になったので速力を無効化
    else
    {
        m_velocity.x = 0.0f;
        m_velocity.z = 0.0f;
    }

    // XZ平面の速力を加速する
    if (length <= m_maxMoveSpeed * m_maxMoveSpeed)
    {
        // 移動ベクトルがゼロベクトルでないなら加速する
        float moveVecLength = m_moveVecX * m_moveVecX + m_moveVecZ * m_moveVecZ;
        if (moveVecLength < 0.0f) return;

        // sqrtfは処理が重いので、ifの後に使う
        moveVecLength = sqrtf(moveVecLength);

        // 加速力
        float acceleration = this->m_acceleration * elapsedFrame;

        //// 空中にいるときは加速力を減らす
        //if (!isGround) {
        //    m_acceleration *= m_airControl;
        //}

        // 移動ベクトルによる加速処理
        m_velocity.x += m_moveVecX * acceleration;
        m_velocity.z += m_moveVecZ * acceleration;

        // 最大速度制限
        float length = m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z;
        if (length > m_maxMoveSpeed * m_maxMoveSpeed)
        {
            length = sqrtf(length);

            float vx = m_velocity.x / length;
            float vz = m_velocity.z / length;

            m_velocity.x = vx * m_maxMoveSpeed;
            m_velocity.z = vz * m_maxMoveSpeed;
        }

        // 下り坂でガタガタしないようにする
        if (m_slopeRate > 0.1f)
        {
            m_velocity.y = -m_maxMoveSpeed * (1.0f + m_slopeRate);
        }
        
    }
    // 移動ベクトルをリセット
    m_moveVecX = 0.0f;
    m_moveVecZ = 0.0f;
}

void Movement::UpdateHorizontalMove(float elapsedTime)
{
    auto object = gameObject.lock();

    // 水平速力量計算
    float velocityLengthXZ = m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z;

    if (velocityLengthXZ < 0.0f) return;

    // 水平移動値
    float mx = m_velocity.x * elapsedTime;
    float mz = m_velocity.z * elapsedTime;

    // レイの開始位置と終点位置
    DirectX::XMFLOAT3 start =
    {
        object->transform.m_position.x,
        object->transform.m_position.y + m_stepOffset,
        object->transform.m_position.z
    };

    DirectX::XMFLOAT3 end =
    {
        object->transform.m_position.x + mx,
        object->transform.m_position.y + m_stepOffset,
        object->transform.m_position.z + mz
    };

    // レイキャストによる壁判定
    std::shared_ptr<GameObject> stage = GameObjectManager::Instance().Find("Stage");
    HitResult hit;
    if (Collision::IntersectRayVsModel(start, end, stage->GetModel(), hit))
    {
        // 壁までのベクトル
        DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
        DirectX::XMVECTOR End   = DirectX::XMLoadFloat3(&hit.position);

        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
        // 補正位置の計算
        DirectX::XMVECTOR B  = DirectX::XMLoadFloat3(&end);
        DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(End, B);

        // 壁の法線
        DirectX::XMVECTOR Normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hit.normal));

        // 入射ベクトルを法線に射影
        DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(PB, Normal);
        float dot = DirectX::XMVectorGetX(Dot);
        dot += 0.0000000001f;  // ジンバルロック回避

        // 
        DirectX::XMVECTOR N = DirectX::XMVectorAdd(B, DirectX::XMVectorScale(Normal, dot));

        // ベクトルの合成
        DirectX::XMVECTOR R = DirectX::XMVectorSubtract(N, End);

        DirectX::XMFLOAT3 r;
        DirectX::XMStoreFloat3(&r, R);

        object->transform.m_position.x += r.x;
        object->transform.m_position.z += r.z;
    }
    else
    {
        // 移動
        object->transform.m_position.x += mx;
        object->transform.m_position.z += mz;
    }    

    //// ステージとの当たり判定
    //StageManager& manager = StageManager::Instance();
    //Stage* stage = manager.GetStage(0);

    //for (auto& collision : stage->GetCollisionData())
    //{
    //    if (Collision::IntersectSphereVSSquare(
    //        m_parent->position,
    //        penentrationRadius,
    //        m_velocity,
    //        0.01f,
    //        collision.position,
    //        collision.size,
    //        m_velocity,
    //        1000.0f)) {
    //        OnWallClash();
    //    }
    //}
}
