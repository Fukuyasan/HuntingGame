#pragma once

#include "System/Input/GamePad.h"
#include "System/Input/Mouse.h"

// インプット
class Input final
{
public:
	Input(HWND hWnd);
	~Input() {}

public:
	// インスタンス取得
	static Input& Instance() { return *instance; }

	// 更新処理
	void Update();

	// ゲームパッド取得
	GamePad& GetGamePad() { return gamePad; }

	// マウス取得
	Mouse& GetMouse() { return mouse; }

private:
	static Input*		instance;
	GamePad				gamePad;
	Mouse				mouse;
};
