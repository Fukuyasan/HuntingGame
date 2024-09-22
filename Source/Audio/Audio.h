#pragma once

#include <map>
#include <xaudio2.h>
#include "Audio/AudioSource.h"

enum class AudioList
{
	BattleBGM,

	PlayerWalk,
	PlayerInVail,
	PlayerSlash,
	PlayerGetHit,
	PlayerDie,
	PlayerAttack1,
	PlayerAttack2,
	PlayerAttack3,
	PlayerAttack4,

	DragonVoice,
	DragonRoar,
	DragonFootStep,
	DragonExplosion,
	DragonLand,
	DragonClack
};

// �I�[�f�B�I
class Audio
{
private:
	Audio()  {}
	~Audio() {}

public:
	// �C���X�^���X�擾
	static Audio& Instance() 
	{
		static Audio instance;
		return instance; 
	}

	void Initialize();
	void Finalize();

	void ImportAudio();

	// �I�[�f�B�I�\�[�X�ǂݍ���
	std::unique_ptr<AudioSource> LoadAudioSource(const char* filename);

	void PlayAudio(AudioList list, bool loop = false, float volum = 1.0f);

private:
	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;

	std::map<int, std::unique_ptr<AudioSource>> audioPool;
};
