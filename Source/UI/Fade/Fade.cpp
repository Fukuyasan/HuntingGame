#include "Fade.h"
#include "UI/UIManager.h"

void Fade::Initialize(const float time)
{
    fade = std::make_unique<UIButton>(nullptr);
    fade->SetTexSize({ 1920, 1080 });  // ‰æ–ÊÅ‘å
    fade->SetColor({ 0, 0, 0, 0 });
    fade->SetAlpha(0.0f);

    UIManager::Instance().RegisterUI(fade.get());

    fadeTime  = 0.0f;
    totalTime = time;
    isFade = false;
}

bool Fade::PlayFade(const float elapsedTime)
{
    if (!isFade) return false;

    // ˆÃ“]
    fadeTime += elapsedTime;

    const float fadeAlpha = fadeTime / totalTime;
    fade->SetAlpha(fadeAlpha);

    return fadeTime > totalTime;
}
