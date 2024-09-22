#include "System/Misc.h"
#include "Audio/Audio.h"

#include <fstream>
#include <iostream>
#include <string>

void Audio::Initialize()
{
	HRESULT hr;

	// COMの初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio初期化
	hr = XAudio2Create(&xaudio, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// マスタリングボイス生成
	hr = xaudio->CreateMasteringVoice(&masteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 音楽をインポート
	ImportAudio();
}

void Audio::Finalize()
{
	for (auto& audio : audioPool)
	{
		audio.second->Stop();
	}
	audioPool.clear();

	// マスタリングボイス破棄
	if (masteringVoice != nullptr)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	// XAudio終了化
	if (xaudio != nullptr)
	{
		xaudio->Release();
		xaudio = nullptr;
	}

	// COM終了化
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

// オーディオソース読み込み
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
