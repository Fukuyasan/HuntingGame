#include "EventData.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <types/vector.hpp>
#include <types/map.hpp>
#include <types/string.hpp>
#include <archives/binary.hpp>
#include <archives/json.hpp>

#include <imgui.h>

#include "System/Time/TimeManager.h"

CEREAL_CLASS_VERSION(EventData,  1)
CEREAL_CLASS_VERSION(AttackData, 1)
CEREAL_CLASS_VERSION(MoveData,   1)

template<class Archive>
inline void AttackData::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(attackPower),
        CEREAL_NVP(hitStopFrame),
        CEREAL_NVP(invincibleTime)
    );
}

template<class Archive>
inline void MoveData::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(lerpRate)
    );
}

template<class Archive>
inline void EventData::serialize(Archive& archive)
{ 
    archive(
        CEREAL_NVP(beginFrame),
        CEREAL_NVP(endFrame),
        CEREAL_NVP(eventType),
        CEREAL_NVP(attackData),
        CEREAL_NVP(moveData)
    );
}

void EventInfo::Deserialize(const char* filename)
{
    // ÉtÉ@ÉCÉãì«Ç›çûÇ›
    std::ifstream ifs(filename, std::ios::binary);

    if (ifs.is_open())
    {
        cereal::BinaryInputArchive archive(ifs);

        archive(
            CEREAL_NVP(eventDatas)
        );
    }
}

bool EventInfo::possibleEvent(int animNo, const char* eventType, float second)
{
    for (const auto& eventData : eventDatas[animNo])
    {
        if (eventData.eventType != eventType) continue;

        const float beginSeconds = TimeManager::Instance().ConvertFrameToSeconds(eventData.beginFrame);
        const float endSeconds = TimeManager::Instance().ConvertFrameToSeconds(eventData.endFrame);

        if (!GameMath::PermissionInAnimationSeconds(second, beginSeconds, endSeconds)) continue;

        // èÓïÒÇï€ë∂
        if (eventData.eventType == "Collision")
        {
            saveAttackData = eventData.attackData;
        }
        if (eventData.eventType == "Move")
        {
            saveMoveData = eventData.moveData;
        }

        return true;
    }

    return false;
}