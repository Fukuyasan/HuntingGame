#include "Movement.h"
#include "System/Time/TimeManager.h"
#include "System/Collision/Collision.h"

#include "GameObject/GameObject.h"
#include "GameObject/GameObjectManager.h"

void Movement::Move(float vx, float vz, float speed)
{
    // �ړ��x�N�g����ݒ�
    m_moveVecX = vx;
    m_moveVecZ = vz;

    // �ő呬�x�ݒ�
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

    // �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    float length = vx * vx + vz * vz;
    if (length == 0) return;

    length = sqrtf(length);

    // �i�s�x�N�g����P�ʃx�N�g����
    vx /= length;
    vz /= length;

    // ���g�̉�]�l����O���������߂�
    float frontX = sinf(object->transform.GetAngle().y);
    float frontZ = cosf(object->transform.GetAngle().y);
    // �� angle����������킩��

    // ��]�p�����߂邽�߁A2�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (frontX * vx) + (frontZ * vz);

    // ���ϒl��-1.0�`1.0�ŕ\������Ă���A2�̒P�ʃx�N�g���̊p�x��
    // �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    float rot = (std::min)(1.0f - dot, speed);  // �␳�l

    // ���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (frontZ * vx) - (frontX * vz);

    // 2d�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
    // ���E������s�����Ƃɂ���č��E��]��I������
    if (cross < 0.0f) {
        object->transform.m_angle.y -= rot;
    }
    else {
        object->transform.m_angle.y += rot;
    }
}

void Movement::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // ���͂ɗ͂�������
    m_velocity.x += impulse.x;
    m_velocity.y += impulse.y;
    m_velocity.z += impulse.z;
}

void Movement::UpdateVelocity(const float& elapsedTime)
{
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    // �������͍X�V����
    UpdateVerticalVelocity(elapsedFrame);

    // �������͍X�V����
    UpdateHorizontalVelocity(elapsedFrame);

    // �����ړ��X�V����
    UpdateVerticalMove(elapsedTime);

    // �����ړ��X�V����
    UpdateHorizontalMove(elapsedTime);
}

void Movement::UpdateVerticalVelocity(float elapsedFrame)
{
    // �d�͏���
    m_velocity.y += m_gravity * elapsedFrame;
}

void Movement::UpdateVerticalMove(float elapsedTime)
{
    auto object = gameObject.lock();

    // ���������̈ړ���
    float my = m_velocity.y * elapsedTime;

    // �L�����N�^�[��Y�����ƂȂ�@���x�N�g��
    DirectX::XMFLOAT3 normal = { 0,1,0 };

    // �X�Η���������
    m_slopeRate = 0.0f;
    
    // �㏸��
    if (my > 0.0f)
    {
        object->transform.m_position.y += my;
        //isGround = false;
    }
    // ������
    else
    {
        // ���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start =
        {
            object->transform.m_position.x,
            object->transform.m_position.y + m_stepOffset,
            object->transform.m_position.z
        };

        // ���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end =
        {
            object->transform.m_position.x,
            object->transform.m_position.y + my,
            object->transform.m_position.z };

        // ���C�L���X�g�ɂ��n�ʔ���
        auto stage = GameObjectManager::Instance().Find("Stage");
        HitResult hit;
        if (Collision::IntersectRayVsModel(start, end, stage->GetModel(), hit))
        {
            // �@���x�N�g���̎擾
            normal = hit.normal;

            // �X�Η��̌v�Z�@�X�Η� = ���� / ��Ӂ{�����j
            DirectX::XMFLOAT2 normal2 = { normal.x, normal.z };
            float length = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMLoadFloat2(&normal2)));

            // �Օ��������߂�
            m_slopeRate = 1.0f - normal.y / (length + normal.y);

            // �n�ʂɐڒn���Ă���
            object->transform.m_position.y = hit.position.y;

            // ��]
            object->transform.m_angle.y += hit.rotation.y;

            // ���n����
            if (!m_isGround)
            {
                //m_isLanding = true;
            }
            m_isGround = true;
            m_velocity.y = 0.0f;
        }
        else
        {
            // �󒆂ɕ����Ă���
            object->transform.m_position.y += my;
            //m_isGround  = false;
            //m_isLanding = false;
        }
    }

#if 0
    // �n�ʂ̌����ɉ����悤��XZ����]
    {
        // Y�����@���x�N�g�������Ɍ����I�C���[�p���Z�o����
        float ax = atan2f(normal.z, normal.y);
        float az = -atan2f(normal.x, normal.y);

        // ���`��ԂŊ��炩�ɉ�]����dd
        angle.x = Mathf::lerp(angle.x, ax, 0.2f);
        angle.z = Mathf::lerp(angle.z, az, 0.2f);
    }
#endif
}

void Movement::UpdateHorizontalVelocity(float elapsedFrame)
{
    // XZ���ʂ̑��͂���������
    float length = m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z;
    if (length < 0.0f) return;

    // ���C��
    float friction = this->m_friction * elapsedFrame;

    //// �󒆂ɂ���Ƃ��͖��C�͂����炷
    //if (!isGround) {
    //    m_friction *= m_airControl;
    //}

    // ���C�ɂ�鉡���ʂ̌�������
    if (length > friction * friction)
    {
        length = sqrtf(length);

        // �P�ʃx�N�g����
        float vx = m_velocity.x / length;
        float vz = m_velocity.z / length;

        m_velocity.x -= vx * friction;
        m_velocity.z -= vz * friction;
    }
    // �������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
    else
    {
        m_velocity.x = 0.0f;
        m_velocity.z = 0.0f;
    }

    // XZ���ʂ̑��͂���������
    if (length <= m_maxMoveSpeed * m_maxMoveSpeed)
    {
        // �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
        float moveVecLength = m_moveVecX * m_moveVecX + m_moveVecZ * m_moveVecZ;
        if (moveVecLength < 0.0f) return;

        // sqrtf�͏������d���̂ŁAif�̌�Ɏg��
        moveVecLength = sqrtf(moveVecLength);

        // ������
        float acceleration = this->m_acceleration * elapsedFrame;

        //// �󒆂ɂ���Ƃ��͉����͂����炷
        //if (!isGround) {
        //    m_acceleration *= m_airControl;
        //}

        // �ړ��x�N�g���ɂ���������
        m_velocity.x += m_moveVecX * acceleration;
        m_velocity.z += m_moveVecZ * acceleration;

        // �ő呬�x����
        float length = m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z;
        if (length > m_maxMoveSpeed * m_maxMoveSpeed)
        {
            length = sqrtf(length);

            float vx = m_velocity.x / length;
            float vz = m_velocity.z / length;

            m_velocity.x = vx * m_maxMoveSpeed;
            m_velocity.z = vz * m_maxMoveSpeed;
        }

        // �����ŃK�^�K�^���Ȃ��悤�ɂ���
        if (m_slopeRate > 0.1f)
        {
            m_velocity.y = -m_maxMoveSpeed * (1.0f + m_slopeRate);
        }
        
    }
    // �ړ��x�N�g�������Z�b�g
    m_moveVecX = 0.0f;
    m_moveVecZ = 0.0f;
}

void Movement::UpdateHorizontalMove(float elapsedTime)
{
    auto object = gameObject.lock();

    // �������͗ʌv�Z
    float velocityLengthXZ = m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z;

    if (velocityLengthXZ < 0.0f) return;

    // �����ړ��l
    float mx = m_velocity.x * elapsedTime;
    float mz = m_velocity.z * elapsedTime;

    // ���C�̊J�n�ʒu�ƏI�_�ʒu
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

    // ���C�L���X�g�ɂ��ǔ���
    std::shared_ptr<GameObject> stage = GameObjectManager::Instance().Find("Stage");
    HitResult hit;
    if (Collision::IntersectRayVsModel(start, end, stage->GetModel(), hit))
    {
        // �ǂ܂ł̃x�N�g��
        DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
        DirectX::XMVECTOR End   = DirectX::XMLoadFloat3(&hit.position);

        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
        // �␳�ʒu�̌v�Z
        DirectX::XMVECTOR B  = DirectX::XMLoadFloat3(&end);
        DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(End, B);

        // �ǂ̖@��
        DirectX::XMVECTOR Normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hit.normal));

        // ���˃x�N�g����@���Ɏˉe
        DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(PB, Normal);
        float dot = DirectX::XMVectorGetX(Dot);
        dot += 0.0000000001f;  // �W���o�����b�N���

        // 
        DirectX::XMVECTOR N = DirectX::XMVectorAdd(B, DirectX::XMVectorScale(Normal, dot));

        // �x�N�g���̍���
        DirectX::XMVECTOR R = DirectX::XMVectorSubtract(N, End);

        DirectX::XMFLOAT3 r;
        DirectX::XMStoreFloat3(&r, R);

        object->transform.m_position.x += r.x;
        object->transform.m_position.z += r.z;
    }
    else
    {
        // �ړ�
        object->transform.m_position.x += mx;
        object->transform.m_position.z += mz;
    }    

    //// �X�e�[�W�Ƃ̓����蔻��
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
