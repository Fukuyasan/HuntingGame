#pragma once
#include "UI/Button/UIButton.h"
#include <memory>

class Fade
{
private:
	Fade() {}
	~Fade() {}

public:
	Fade(const Fade&) = delete;
	Fade(Fade&&)	  = delete;
	Fade operator=(const Fade&) = delete;
	Fade operator=(Fade&&)		= delete;

public:
	static Fade& Instance()
	{
		static Fade instance;
		return instance;
	}

	void Initialize(const float time);
	bool PlayFade(const float elapsedTime);

	void SetFade() { isFade = true; }

private:
	std::unique_ptr<UIButton> fade = nullptr;

	float totalTime = 0.0f;
	float fadeTime  = 0.0f;

	bool isFade = false;
};
