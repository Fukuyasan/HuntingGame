#pragma once
#include <memory>
#include <vector>
#include <map>

#include "System/Math/Mathf.h"

// �U���̃f�[�^
struct AttackData
{
    float attackPower    = 0;     // �U����
    int   hitStopFrame   = 0;     // �q�b�g�X�g�b�v
    float invincibleTime = 0.5f;  // ���G����

    template<class Archive>
    void serialize(Archive& archive);
};

// �U���̈ړ��l
struct MoveData
{
    // �ړ��l
    float lerpRate = 0.0f;

    template<class Archive>
    void serialize(Archive& archive);
};

// ���̋�Ԃōs���𔭐�������
struct EventData
{
    int beginFrame;
    int endFrame;
    std::string eventType;

    AttackData attackData;
    MoveData   moveData;

    template<class Archive>
    void serialize(Archive& archive);
};

class EventInfo
{
public:
    // ����̃A�j���[�V�����Đ����ԓ��Ő؂�ւ���
    bool possibleEvent(int animNo, const char* eventType, float second);

    void Deserialize(const char* filename);

    const AttackData& GetAttackData() { return saveAttackData; }
    const MoveData& GetMoveData() { return saveMoveData; }

public:
    // �A�j���[�V�����̔ԍ��A�A�j���[�V�����̃C�x���g
    std::map<int, std::vector<EventData>> eventDatas = {};

    // ���̎擾�p�ɕۑ����Ă������߂̍\����
private:
    AttackData saveAttackData;
    MoveData saveMoveData;
};