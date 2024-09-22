#include "TimeManager.h"
#include "System/Framework.h"

void TimeManager::Update()
{
	// �^�C�}�[���X�V����֐�
	m_timer.Tick();

	// true : FPS�Œ肵�Ă��Ȃ��Afalse : FPS�Œ�
	float elapsedTime = (Framework::syncInterval == 0)
		? m_timer.TimeInterval()
		: Framework::syncInterval / 60.0f;

	// m_deltaTime �̐ݒ�
	m_deltaTime = elapsedTime;
}
