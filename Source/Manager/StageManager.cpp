#include "StageManager.h"

#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <types/vector.hpp>
#include <types/string.hpp>
#include <archives/json.hpp>

#include "Cereal/Cereal.h"

#include "System/String/StringFunction.h"

#include "GameObject/GameObjectManager.h"

CEREAL_CLASS_VERSION(StageManager::StageParam, 1)
CEREAL_CLASS_VERSION(StageManager::StageData, 1)

#define STAGEDEBUG 0

template<class Archive>
void serialize(Archive& archive, Vec3& vector3)
{
	archive(
		cereal::make_nvp("x", vector3.x),
		cereal::make_nvp("y", vector3.y),
		cereal::make_nvp("z", vector3.z)
	);
}

template<class Archive>
inline void StageManager::StageParam::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(position),
		CEREAL_NVP(rotation),
		CEREAL_NVP(scale)
	);
}

template<class Archive>
inline void StageManager::StageData::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(stageParam)
	);
}

//template<class Archive>
//inline void StageMain::AreaData::serialize(Archive& archive)
//{
//	archive(
//		CEREAL_NVP(areaNum),
//		CEREAL_NVP(areaPos),
//		CEREAL_NVP(areaRange)
//	);
//}
//
//template<class Archive>
//inline void StageMain::CollisionData::serialize(Archive& archive)
//{
//	archive(
//		CEREAL_NVP(position),
//		CEREAL_NVP(size)
//	);
//}

void StageManager::LoadStageObjects(const char* filename)
{
	// Jsonファイルを読み込み
	std::ifstream ifs(filename, std::ios::in);
	cereal::JSONInputArchive archive(ifs);

	// 読み込んだJsonファイルを
	// ステージ情報の構造体にいれる
	StageData stageData;
	stageData.serialize(archive);

	// 名前を取得
	std::string path = "Data/Model/Stage/model/";

	// Jsonのステージ情報のサイズループ
	int stageSize = static_cast<int>(stageData.stageParam.size());
	//namePool.resize(stageSize);

	constexpr float scaleRatio = 2.0f;

	for (int i = 0; i < stageSize; ++i)
	{
		// " "から前を区切る
		std::string stageName = stageData.stageParam.at(i).name;
		//namePool[i] = stageData.stageParam.at(i).name;
		//std::string modelName = sprit(namePool[i], " ");
		std::string modelName = sprit(stageName, " ");

		// 文字を合体
		std::string finalName = path + modelName + ".mdl";
		const char* fileName  = finalName.c_str();

		// ステージのトランスフォームを取得
		Vec3 stagePos   = stageData.stageParam.at(i).position;
		Vec3 stageRot   = stageData.stageParam.at(i).rotation;
		Vec3 stageScale = stageData.stageParam.at(i).scale;

		// 位置を設定
		DirectX::XMFLOAT3 position, rotation, scale;
		position.x = stagePos.x * scaleRatio;
		position.y = stagePos.y * scaleRatio;
		position.z = stagePos.z * scaleRatio;

		// 回転値を設定
		rotation.x = DirectX::XMConvertToRadians(stageRot.x);
		rotation.y = DirectX::XMConvertToRadians(stageRot.y);
		rotation.z = DirectX::XMConvertToRadians(stageRot.z);

		// スケール値を設定
		scale.x = stageScale.x * (scaleRatio * 0.01f);
		scale.y = stageScale.y * (scaleRatio * 0.01f);
		scale.z = stageScale.z * (scaleRatio * 0.01f);

		// ステージのオブジェクトを作成
		std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
		gameObject->LoadModel(fileName);
		gameObject->SetName(stageName);
		gameObject->SetLayer(Layer::Stage);
					
		gameObject->transform.SetPosition(position);
		gameObject->transform.SetAngle(rotation);
		gameObject->transform.SetScale(scale);

		gameObject->transform.UpdateTransform();
		gameObject->GetModel()->UpdateTransform(gameObject->transform.GetTransform());
		
	}
}
