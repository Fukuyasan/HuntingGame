#include "System/Misc.h"
#include "Audio/Audio.h"

#include <fstream>
#include <iostream>
#include <string>

void Audio::Initialize()
{
	HRESULT hr;

	// COM�̏�����
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio������
	hr = XAudio2Create(&xaudio, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// �}�X�^�����O�{�C�X����
	hr = xaudio->CreateMasteringVoice(&masteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// ���y���C���|�[�g
	ImportAudio();
}

void Audio::Finalize()
{
	for (auto& audio : audioPool)
	{
		audio.second->Stop();
	}
	audioPool.clear();

	// �}�X�^�����O�{�C�X�j��
	if (masteringVoice != nullptr)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	// XAudio�I����
	if (xaudio != nullptr)
	{
		xaudio->Release();
		xaudio = nullptr;
	}

	// COM�I����
	CoUninitialize();
}

void Audio::ImportAudio()
{
	std::ifstream fp("Data/Audio/AudioList.txt");
	std::string line;

	int i = 0;
	while (std::getline(fp, line))
	{
		audioPool[i] = LoadAudioSource(line.c_str());
		++i;
	}
}

// �I�[�f�B�I�\�[�X�ǂݍ���
std::unique_ptr<AudioSource> Audio::LoadAudioSource(const char* filename)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return std::make_unique<AudioSource>(xaudio, resource);
}

void Audio::PlayAudio(AudioList list, bool loop, float volum)
{
	audioPool[static_cast<int>(list)]->SetVolume(volum);
	audioPool[static_cast<int>(list)]->Play(loop);
}
