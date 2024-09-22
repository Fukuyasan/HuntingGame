#pragma once
#include "HighResolutionTimer.h"

class TimeManager final
{
private:
	TimeManager() {}
	~TimeManager() {}

public:
	TimeManager(const TimeManager&) = delete;
	TimeManager(TimeManager&&) = delete;
	TimeManager operator=(const TimeManager&) = delete;
	TimeManager operator=(TimeManager&&) = delete;

	static TimeManager& Instance()
	{
		static TimeManager instance;
		return instance;
	}

public:
	// �X�V����
	void Update();

	void SetDeltaTime(const float time) { m_deltaTime = time; }
	const float GetDeltaTime() { return m_deltaTime; }

	//�t���[����b�ɕϊ�
	float ConvertFrameToSeconds(int frame) { return  static_cast<float>(frame) / 60.0f; }

	// �b���t���[���ɕϊ�
	int ConvertSecondsToFrame(float seconds) { return static_cast<int>(seconds * 60.0f); }

public:
	// �^�C�}�[�𐶐�����N���X
	HighResolutionTimer	m_timer;

private:
	float m_deltaTime = 0.0f;
};
