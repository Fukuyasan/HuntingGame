#pragma once

#include "System/Input/GamePad.h"
#include "System/Input/Mouse.h"

// �C���v�b�g
class Input final
{
public:
	Input(HWND hWnd);
	~Input() {}

public:
	// �C���X�^���X�擾
	static Input& Instance() { return *instance; }

	// �X�V����
	void Update();

	// �Q�[���p�b�h�擾
	GamePad& GetGamePad() { return gamePad; }

	// �}�E�X�擾
	Mouse& GetMouse() { return mouse; }

private:
	static Input*		instance;
	GamePad				gamePad;
	Mouse				mouse;
};
