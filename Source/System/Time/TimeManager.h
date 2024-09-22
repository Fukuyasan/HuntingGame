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
	// 更新処理
	void Update();

	void SetDeltaTime(const float time) { m_deltaTime = time; }
	const float GetDeltaTime() { return m_deltaTime; }

	//フレームを秒に変換
	float ConvertFrameToSeconds(int frame) { return  static_cast<float>(frame) / 60.0f; }

	// 秒をフレームに変換
	int ConvertSecondsToFrame(float seconds) { return static_cast<int>(seconds * 60.0f); }

public:
	// タイマーを生成するクラス
	HighResolutionTimer	m_timer;

private:
	float m_deltaTime = 0.0f;
};
