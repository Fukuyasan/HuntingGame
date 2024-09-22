#pragma once

#include <memory>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ModelResource.h"

class ResourceManager
{
private:
	ResourceManager()  = default;
	~ResourceManager() = default;
public:
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&)		= delete;
	ResourceManager operator= (const ResourceManager&) = delete;
	ResourceManager operator= (ResourceManager&&)	   = delete;

	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	// ������
	void Initialize();

	// �I����
	void Finalize();

	// �ǂݍ��ݒ��̃��\�[�X���ǂݍ��݊�������܂ő҂�
	void WaitOnPending();

	// ���f�����\�[�X�ǂݍ���
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
	std::shared_ptr<ModelResource> LoadModelResourceAsync(const char* filename);
private:
	// ���[�f�B���O�X���b�h
	void LoadingThread();

	// ���f��
	void LoadModel();

	// �X���b�h���N����
	void WakeUpLoadingThread();
private:
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;

	// ���f����ǂݍ��ޗp�̃}�b�v
	ModelMap penddingModels;
	// ���f�������[�h����p�̃}�b�v
	ModelMap loadedModels;

	std::unique_ptr<std::thread> thread;
	std::mutex					 mutex;
	HANDLE						 event = NULL;

	// volatile : Release���ƍœK���̂��߁Abool�������Ă��܂��炵���̂ł����h��
	volatile bool exitRequested = false;
};
