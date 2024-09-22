#pragma once
#include "UI\UIBase.h"

class UIGauge : public UIBase
{
public:
    UIGauge(const char* filename) : UIBase(filename) {}
    ~UIGauge()override {}

    void Update(const float& elapsedTime) override;

    void SetIndex(float index) { this->index = index; }
    void SetMax(float max)     { this->max   = max; }

    void DrawDebugGUI() override;

private:
    float index = 0.0f;
    float max   = 0.0f;
};