#include "MetaAI.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <types/vector.hpp>
#include <types/string.hpp>
#include <archives/json.hpp>

#include "Component/Stage/StageMain.h"

#include "Cereal/Cereal.h"

template<class Archive>
inline void MetaAI::AreaData::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(areaNum),
		CEREAL_NVP(areaPos),
		CEREAL_NVP(areaRange)
	);
}

void MetaAI::Initialize()
{
	ImportStageData();

	for (AreaData& data : areaDatas)
	{
		areaDataPool[data.areaNum].areaPos	 = data.areaPos;
		areaDataPool[data.areaNum].areaRange = data.areaRange;
	}
}

// ���M���ꂽ���b�Z�[�W�ɂ��������
bool MetaAI::OnMessage(const Telegram& telegram)
{
	switch (telegram.type)
	{
	case MessageType::M_ApplicationAreaMove:  // �G���A�ړ��̐\������


		return true;
		break;
	case MessageType::M_PermitAreaMove:		  // �G���A�ړ��̋���

		return true;
		break;
	}

	return false;
}

void MetaAI::SendMessaging(const Telegram& telegram)
{
	
}

void MetaAI::ImportStageData()
{
	// �G���A�t�@�C���ǂݍ���
	{
		std::ifstream ifs("Data/Json/AreaData.json", std::ios::in);

		if (ifs.is_open())
		{
			cereal::JSONInputArchive archive(ifs);
			archive(
				CEREAL_NVP(areaDatas)
			);
		}
	}
}
