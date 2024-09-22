#pragma once
#include "Graphics/Graphics.h"
#include <DirectXMath.h>

class Particle
{
public:
	Particle() = default;
	virtual ~Particle() = default;

protected:
	// ƒXƒŒƒbƒh‚Ì®—ñ
	UINT align(UINT num, UINT alignment)
	{
		return (num + (alignment - 1)) & ~(alignment - 1);
	}
};
