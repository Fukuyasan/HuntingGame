#include "ResourceManager.h"
#include "System/Logger.h"
#include "Graphics/Graphics.h"

// ������
void ResourceManager::Initialize()
{		
	// �C�x���g�쐬
	event = CreateEvent(NULL, false, false, NULL);
	
	// �X���b�h�J�n
	thread = std::make_unique<std::thread>([this] { LoadingThread(); });
}

// �I����
void ResourceManager::Finalize()
{
	// �X���b�h�I���v��
	exitRequested = true;

	// �X���b�h���I������܂ő҂�
	if (thread != nullptr)
	{
		WakeUpLoadingThread();
		thread->join();
	}

	// �C�x���g�I��
	if(event != NULL)
	{
		CloseHandle(event);
		event = NULL;
	}

	// �Ǘ����Ă��郊�\�[�X�N���A
	penddingModels.clear();
	loadedModels.clear();
}

// �ǂݍ��ݒ��̃��\�[�X���ǂݍ��݊�������܂ő҂�
void ResourceManager::WaitOnPending()
{
	size_t numPending = 0;
	do
	{
		std::lock_guard<std::mutex> lock(mutex);

		numPending = penddingModels.size();

		if (numPending)
		{
			Sleep(16);
		}
	} while (numPending);
}

// ���f�����\�[�X�ǂݍ���
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	// ���f��������
	ModelMap::iterator it = loadedModels.find(filename);
	if (it != loadedModels.end())
	{
		// �����N�i�����j���؂�Ă��Ȃ����m�F
		if (!it->second.expired())
		{
			// �ǂݍ��ݍς݂̃��f�����\�[�X��Ԃ�
			return it->second.lock();
		}
	}

	// �V�K���f�����\�[�X�쐬���ǂݍ���
	ID3D11Device* device = Graphics::Instance().GetDevice();
	auto model = std::make_shared<ModelResource>();
	model->Load(device, filename);

	// �}�b�v�ɓo�^
	loadedModels[filename] = model;

	return model;
}

// ���f�����\�[�X�ǂݍ���(�񓯊�)
std::shared_ptr<ModelResource> ResourceManager::LoadModelResourceAsync(const char* filename)
{
	std::lock_guard<std::mutex> lock(mutex);

	std::shared_ptr<ModelResource> resource;

	// �ǂݍ��ݍς݃��\�[�X�����邩�`�F�b�N
	{
		ModelMap::iterator it = loadedModels.find(filename);
		if (it != loadedModels.end())
		{
			// �����N���؂�Ă��Ȃ����`�F�b�N
			if (!it->second.expired())
			{
				resource = it->second.lock();
			}
		}
	}

	// �ǂݍ��݃��\�[�X�����邩�`�F�b�N
	if (resource == nullptr)
	{
		ModelMap::iterator it = penddingModels.find(filename);
		if (it != penddingModels.end())
		{
			resource = it->second.lock();
		}
	}

	// �V�K���\�[�X���쐬���A�ǂݍ��ݒ����X�g�ɒǉ�
	if (resource == nullptr)
	{
		resource = std::make_shared<ModelResource>();
		penddingModels[filename] = resource;
		// �X���b�h���N����
		WakeUpLoadingThread();
	}

	return resource;
}

// ���[�f�B���O�X���b�h
void ResourceManager::LoadingThread()
{
	while (!exitRequested)
	{
		// �X���b�h���N����܂ő҂�
		{
			WaitForSingleObject(event, INFINITE);
		}
		if (exitRequested) break;

		// ���f�����\�[�X
		LoadModel();
	}
}

// ���f����ǂݍ���
void ResourceManager::LoadModel()
{
	// �ǂݍ��ݏ���
	ID3D11Device* device = Graphics::Instance().GetDevice();

	std::string filename;
	bool pending = true;
	while (pending)
	{
		// ���f�����\�[�X
		{
			std::shared_ptr<ModelResource> model;
			// �}�b�v����ǂݍ��ݏ��擾
			{
				std::lock_guard<std::mutex> lock(mutex);

				// �擪�̓ǂݍ��ݏ����擾
				ModelMap::iterator it = penddingModels.begin();

				// �ǂݍ��ނ��̂��Ȃ���ΏI��
				if (it == penddingModels.end())
				{
					pending = false;
					continue;
				}

				// ���ꂩ��ǂݍ��ރt�@�C�������擾
				filename = it->first;

				// ���ꂩ��ǂݍ��ރ��\�[�X���擾
				model = it->second.lock();
			}
			// �ǂݍ���
			model->Load(device, filename.c_str());
			LOG("Loaded %s\n",  filename.c_str());

			// �ǂݍ��ݍς݃��\�[�X�̓o�^�Ɠǂݍ��ݒ����\�[�X�̉���
			{
				std::lock_guard<std::mutex> lock(mutex);

				loadedModels[filename] = model;
				penddingModels.erase(filename);
			}
		}
	}
}

// �X���b�h���N����
void ResourceManager::WakeUpLoadingThread()
{
	if (event != NULL)
	{
		SetEvent(event);
	}
}
