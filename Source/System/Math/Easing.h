#pragma once

class Easing
{
public:
	static inline float OutQuart(float time, float totalTime, float max = 1, float min = 0)
	{
		max -= min;
		time = time / totalTime - 1;

		return -max * (time * time * time * time - 1) + min;
	}
};
