// https://github.com/ashima/webgl-noise
//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
// 

// HEAT_HAZE
float hash1(float2 p)
{
    //Generate a pseudo random number from 'p'.
	return frac(sin(p.x * 0.129898 + p.y * 0.78233) * 43758.5453);
}

float value_noise(float2 p)
{
    //Cell (whole number) coordinates
	float2 cell = floor(p);
    //Sub-cell (fractional) coordinates
	float2 sub = p - cell;
    //Cubic interpolation (use sub for linear interpolation)
	float2 cube = sub * sub * (3. - 2. * sub);
    //Offset vector
	const float2 off = float2(0, 1);

    //Sample cell corners and interpolate between them.
	return lerp(lerp(hash1(cell + off.xx), hash1(cell + off.yx), cube.x),
        lerp(hash1(cell + off.xy), hash1(cell + off.yy), cube.x), cube.y);
}
