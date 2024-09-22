#include "TimeManager.h"
#include "System/Framework.h"

void TimeManager::Update()
{
	// タイマーを更新する関数
	m_timer.Tick();

	// true : FPS固定していない、false : FPS固定
	float elapsedTime = (Framework::syncInterval == 0)
		? m_timer.TimeInterval()
		: Framework::syncInterval / 60.0f;

	// m_deltaTime の設定
	m_deltaTime = elapsedTime;
}
