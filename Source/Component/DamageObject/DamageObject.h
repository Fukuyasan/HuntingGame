#pragma once

// �_���[�W�������s�����߂̃N���X
class DamageObject
{
protected:
	// �_���[�W���󂯂��ۂ̏���
	virtual void OnDamaged() {}

	// ���S�����ۂ̏���
	virtual void OnDead() {}

protected:
	// ���G����
	float invincibleTimer = 1.0f;
};