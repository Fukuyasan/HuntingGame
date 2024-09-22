#pragma once

// ダメージ処理を行うためのクラス
class DamageObject
{
protected:
	// ダメージを受けた際の処理
	virtual void OnDamaged() {}

	// 死亡した際の処理
	virtual void OnDead() {}

protected:
	// 無敵時間
	float invincibleTimer = 1.0f;
};