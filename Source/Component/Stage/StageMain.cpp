//#include "StageMain.h"
//
//#include <iostream>
//#include <fstream>
//#include <sstream>
//
//#include <types/vector.hpp>
//#include <types/string.hpp>
//#include <archives/json.hpp>
//
//#include <imgui.h>
//#include <d3d11_1.h>
//
//#include "Cereal/Cereal.h"
//
//#include "Graphics/Graphics.h"
//#include "Graphics/ShaderState.h"
//#include "Graphics/RenderState.h"
//
//#include "System/String/StringFunction.h"
//#include "Graphics/Renderer/DebugRenderer.h"
//
//#include "GameObject/GameObjectManager.h"
//
//CEREAL_CLASS_VERSION(StageMain::StageParam, 1)
//CEREAL_CLASS_VERSION(StageMain::StageData,  1)
//CEREAL_CLASS_VERSION(Stage::AreaData,	    1)
//CEREAL_CLASS_VERSION(Stage::CollisionData,  1)
//
//#define STAGEDEBUG 0
//
//template<class Archive>
//void serialize(Archive& archive, Vec3& vector3)
//{
//	archive(
//		cereal::make_nvp("x", vector3.x),
//		cereal::make_nvp("y", vector3.y),
//		cereal::make_nvp("z", vector3.z)
//	);
//}
//
//template<class Archive>
//inline void StageMain::StageParam::serialize(Archive& archive)
//{
//	archive(
//		CEREAL_NVP(name),
//		CEREAL_NVP(position),
//		CEREAL_NVP(rotation),
//		CEREAL_NVP(scale)
//	);
//}
//
//template<class Archive>
//inline void StageMain::StageData::serialize(Archive& archive)
//{
//	archive(
//		CEREAL_NVP(stageParam)
//	);
//}
//
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
//
//StageMain::~StageMain()
//{
//	models.clear();
//}
//
//void StageMain::Awake()
//{	
//	// Json����X�e�[�W�ǂݍ���
//	//LoadStageObjects("Data/DataFile/StageData.json");
//}
//
//void StageMain::Start()
//{
//	std::shared_ptr<GameObject> object = gameObject.lock();
//
//	// �I�u�W�F�N�g�s����X�V
//	//object->transform.UpdateTransform();
//
//	// �G���A�f�[�^�ǂݍ���
//	//LoadAreaData();
//}
//
//void StageMain::Update()
//{
//}
//
//void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
//{
//	RenderState& rs = RenderState::Instance();
//
//	// RenderDoc�Ŋm�F���邽��
//	ID3DUserDefinedAnnotation* annot;
//	dc->QueryInterface<ID3DUserDefinedAnnotation>(&annot);
//	annot->BeginEvent(L"DrawStageModels");
//
//	rs.SetDepth(dc,DEPTH_MODE::ZT_ON_ZW_ON);
//	// �`�揈��
//	for (auto& model : models)
//	{
//		shader->Draw(dc, model.get());
//	}
//
//	annot->EndEvent();
//	annot->Release();
//}
//
////void StageMain::InputStageObjects()
////{
////
////	StageParam param;
////	param.name = "Terrain";
////	param.position = { 0,0,0 };
////	param.rotation = { 0,0,0 };
////	param.scale = {1, 1, 1 };
////
////	// �t�@�C���o��
////	std::ofstream ofs("StageTest.json", std::ios::out);
////	cereal::JSONOutputArchive archiveFile(ofs);
////	param.serialize(archiveFile);
////}
////void StageMain::LoadStageObjects()
////{
////	StageParam param;
////
////	// Json�t�@�C����ǂݍ���
////	std::ifstream ifs("StageTest.json", std::ios::in);
////
////	cereal::JSONInputArchive archive(ifs);
////
////	// �ǂݍ���Json�t�@�C����
////	// �X�e�[�W���̍\���̂ɂ����
////	param.serialize(archive);
////
////	// ���O���擾
////	std::string path = "Data/Model/Stage/";
////	// " "����O����؂�
////	std::string stage = sprit(param.name, " ");
////
////	// ����������
////	std::string finalName = path + stage + ".mdl";
////
////	// 
////	const char* fileName = finalName.c_str();
////
////	model = new Model(fileName);
////
////	position.x = param.position.x;
////	position.y = param.position.y;
////	position.z = param.position.z;
////
////	rotation.z = param.rotation.x;
////	rotation.z = param.rotation.y;
////	rotation.z = param.rotation.z;
////
////	scale.z = param.scale.x;
////	scale.z = param.scale.y;
////	scale.z = param.scale.z;	
////}
//
//// Json�ɂ���X�e�[�W�������[�h
//void StageMain::LoadStageObjects(const char* filename)
//{
//	// Json�t�@�C����ǂݍ���
//	std::ifstream ifs(filename, std::ios::in);
//	cereal::JSONInputArchive archive(ifs);
//
//	// �ǂݍ���Json�t�@�C����
//	// �X�e�[�W���̍\���̂ɂ����
//	StageData stageData;
//	stageData.serialize(archive);
//
//	// ���O���擾
//	std::string path = "Data/Model/Stage/model/";
//
//	// Json�̃X�e�[�W���̃T�C�Y���[�v
//	int stageSize = static_cast<int>(stageData.stageParam.size());
//	models.resize(stageSize);
//
//	constexpr float scaleRatio = 2.0f;
//
//	for (int i = 0; i < stageSize; ++i)
//	{
//		// " "����O����؂�
//		std::string stageName = stageData.stageParam.at(i).name;
//		std::string modelName = sprit(stageName, " ");
//
//		// ����������
//		std::string finalName = path + modelName + ".mdl";
//		const char* fileName  = finalName.c_str();
//
//		// �X�e�[�W�̃g�����X�t�H�[�����擾
//		Vec3 stagePos	= stageData.stageParam.at(i).position;
//		Vec3 stageRot	= stageData.stageParam.at(i).rotation;
//		Vec3 stageScale = stageData.stageParam.at(i).scale;
//
//		// �ʒu���擾
//		DirectX::XMFLOAT3 position, rotation, scale;
//		position.x = stagePos.x * scaleRatio;
//		position.y = stagePos.y * scaleRatio;
//		position.z = stagePos.z * scaleRatio;
//
//		// ��]�l��ݒ�
//		rotation.x = DirectX::XMConvertToRadians(stageRot.x);
//		rotation.y = DirectX::XMConvertToRadians(stageRot.y);
//		rotation.z = DirectX::XMConvertToRadians(stageRot.z);
//
//		// �X�P�[���l��ݒ�
//		scale.x = stageScale.x * (scaleRatio * 0.01f);
//		scale.y = stageScale.y * (scaleRatio * 0.01f);
//		scale.z = stageScale.z * (scaleRatio * 0.01f);
//
//		// �X�e�[�W�̃I�u�W�F�N�g���쐬
//		{
//			std::shared_ptr<GameObject> gameObject = GameObjectManager::Instance().Create();
//			gameObject->LoadModel(fileName);
//			gameObject->SetName(stageName.c_str());
//			gameObject->transform.SetPosition(position);
//			gameObject->transform.SetAngle(rotation);
//			gameObject->transform.SetScale(scale);
//			gameObject->transform.UpdateTransform();
//			gameObject->GetModel()->UpdateTransform(gameObject->transform.GetTransform());
//		}
//
//		//m_modelPositions.emplace_back(position);
//	}
//}
//
//void StageMain::LoadAreaData()
//{
//	// �G���A�t�@�C���ǂݍ���
//	{
//		std::ifstream ifs("Data/Json/AreaData.json", std::ios::in);
//
//		if (ifs.is_open())
//		{
//			cereal::JSONInputArchive archive(ifs);
//			archive(
//				CEREAL_NVP(areaDatas)
//			);
//		}
//	}
//
//	// �����蔻��t�@�C���ǂݍ���
//	{		
//		std::ifstream ifs("Data/Json/CollisionData.json", std::ios::in);
//		cereal::JSONInputArchive archive(ifs);
//
//		if (ifs.is_open())
//		{
//			archive(
//				CEREAL_NVP(collisionDatas)
//			);
//		}
//	}
//}
//
//bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
//{
//	return Collision::IntersectRayVsModel(start, end, models.at(0).get(), hit);
//}
//
//void StageMain::OnDebugGUI()
//{
//	std::shared_ptr<GameObject> gameObject = GetGameObject();
//
//	if (ImGui::CollapsingHeader("StageMain"))
//	{
//		// �g�����X�t�H�[��
//		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
//		{
//			// �ʒu
//			DirectX::XMFLOAT3 position = gameObject->transform.GetPosition();
//			ImGui::InputFloat3("Position", &position.x);
//			gameObject->transform.SetPosition(position);
//
//			//// ��]
//			//DirectX::XMFLOAT3 angle = gameObject->transform.GetAngle();
//			//DirectX::XMFLOAT3 a{};
//			//a.x = DirectX::XMConvertToDegrees(angle.x);
//			//a.y = DirectX::XMConvertToDegrees(angle.y);
//			//a.z = DirectX::XMConvertToDegrees(angle.z);
//			//ImGui::InputFloat3("Angle", &a.x);
//			//angle.x = DirectX::XMConvertToRadians(a.x);
//			//angle.y = DirectX::XMConvertToRadians(a.y);
//			//angle.z = DirectX::XMConvertToRadians(a.z);
//
//			//gameObject->transform.SetAngle(angle);
//
//			// �X�P�[��
//			float s = gameObject->transform.GetScale().x;
//			ImGui::InputFloat("Scale", &s);
//
//			gameObject->transform.SetScale(s);
//		}
//
//		///*for (auto& model : models)
//		//{
//		//	auto& rootNode = model->GetNodes()[0];
//
//		//	if (ImGui::TreeNodeEx(rootNode.name, ImGuiTreeNodeFlags_DefaultOpen))
//		//	{
//		//		ImGui::DragFloat4("[0]", &rootNode.worldTransform._11);
//		//		ImGui::DragFloat4("[1]", &rootNode.worldTransform._21);
//		//		ImGui::DragFloat4("[2]", &rootNode.worldTransform._31);
//		//		ImGui::DragFloat4("[3]", &rootNode.worldTransform._41);
//
//		//		ImGui::TreePop();
//		//	}
//		//}*/
//
//		//EditAreaData();
//		//EditCollisionData();
//	}
//}
//
//void StageMain::OnDebugPrimitive()
//{
//	DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();
//	
//	// �G���A
//	for (auto& data : areaDatas)
//	{
//		debugRenderer->AddSphere(data.areaPos, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
//		debugRenderer->AddCylinder(data.areaPos, data.areaRange, 2.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
//	}
//
//	// �R���W����
//	for (auto& data : collisionDatas)
//	{
//		debugRenderer->AddSquare(data.position, data.size, DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));
//	}
//}
//
//void StageMain::EditAreaData()
//{
//	if (ImGui::CollapsingHeader("AreaData"))
//	{
//		for (auto& data : areaDatas)
//		{
//			std::string num = std::to_string(data.areaNum);
//
//			std::string Num		 = "Num"	  + num;
//			std::string Position = "Position" + num;
//			std::string Range	 = "Range"	  + num;
//
//			ImGui::InputInt(Num.c_str(),		&data.areaNum);
//			ImGui::DragFloat3(Position.c_str(), &data.areaPos.x, 1.0f, 0.0f, 300.0f, "%.1f");
//			ImGui::InputFloat(Range.c_str(),    &data.areaRange);
//		}
//
//		if (ImGui::Button("Add"))
//		{
//			AreaData& data = areaDatas.emplace_back();
//			data.areaNum   = static_cast<int>(areaDatas.size()) - 1;
//			data.areaPos   = { 0,0,0 };
//			data.areaRange = 50.0f;
//		}
//
//		if (ImGui::Button("Save"))
//		{
//			// �t�@�C���o��
//			std::ofstream ofs("Data/Json/AreaData.json", std::ios::out);
//			cereal::JSONOutputArchive archive(ofs);
//
//			if (ofs.is_open())
//			{
//				archive(
//					CEREAL_NVP(areaDatas)
//				);
//			}
//		}
//
//		ImGui::InputInt("DeleteArea", &deleteArea);
//		if (ImGui::Button("Delete"))
//		{
//			auto a = std::find_if(areaDatas.begin(), areaDatas.end(), [this](AreaData& data) {return data.areaNum == deleteArea; });
//			if (a != areaDatas.end())
//			{
//				areaDatas.erase(a);
//			}
//		}
//	}
//}
//
//void StageMain::EditCollisionData()
//{
//	if (ImGui::CollapsingHeader("CollisionData"))
//	{
//		int num = -1;
//		for (auto& data : collisionDatas)
//		{
//			std::string Num = std::to_string(++num);
//
//			std::string Position = "Position:" + Num;
//			std::string Size     = "Size:"     + Num;
//
//			ImGui::DragFloat3(Position.c_str(), &data.position.x, 1.0f, 0.0f, 300.0f, "%.1f");
//			ImGui::DragFloat3(Size.c_str(),		&data.size.x,     0.1f, 0.0f, 500.0f, "%.1f");
//		}
//
//		if (ImGui::Button("Add"))
//		{
//			CollisionData& data = collisionDatas.emplace_back();
//			data.position       = { 0,0,0 };
//			data.size           = { 1,1,1 };
//		}
//
//		if (ImGui::Button("Save"))
//		{
//			// �t�@�C���o��
//			std::ofstream ofs("Data/Json/CollisionData.json", std::ios::out);
//			cereal::JSONOutputArchive archive(ofs);
//
//			if (ofs.is_open())
//			{
//				archive(
//					CEREAL_NVP(collisionDatas)
//				);
//			}
//		}
//	}
//}
