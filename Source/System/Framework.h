#pragma once

#include <windows.h>
#include "Input/input.h"

class Framework
{
public:
	Framework(HWND hWnd);
	~Framework();

private:
	void Update();
	void Render();

	void CalculateFrameStats();

public:
	int Run();
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 垂直同期間隔設定
	static inline const int syncInterval = 1;

private:
	const HWND				hWnd;
	Input					input;
};

