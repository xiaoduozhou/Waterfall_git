#include<math.h>
#include"perlin_noise.h"
float Noise(int x, int y)
{
	int n;
	n = x + y * 57;
	n = (n << 13) ^ n;
	return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float SmoothedNoise1(int x, int y)
{
	float corners, sides, center;
	corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16;
	sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8;
	center = Noise(x, y) / 4;
	return corners + sides + center;
}
float Interpolate(float a, float b, float x)
{
	return a*(1 - x) + b*x;
}
float  InterpolatedNoise_1(float x, float y)
{
	int integer_X = (int)x, integer_Y = (int)y;
	float fractional_X = x - integer_X, fractional_Y = y - integer_Y;
	float v1, v2, v3, v4, i1, i2;
	v1 = SmoothedNoise1(integer_X, integer_Y);
	v2 = SmoothedNoise1(integer_X + 1, integer_Y);
	v3 = SmoothedNoise1(integer_X, integer_Y + 1);
	v4 = SmoothedNoise1(integer_X + 1, integer_Y + 1);
	i1 = Interpolate(v1, v2, fractional_X);
	i2 = Interpolate(v3, v4, fractional_X);
	return Interpolate(i1, i2, fractional_Y);
}

float PerlinNoise_2D(float x, float y)
{
	float total = 0, p = 1, n = 3;  //p为持续度， n 为 倍频频率
	for (int i = 0; i<n; i++)
	{
		float frequency = _Pow_int(2, i);
		float amplitude = _Pow_int(p, i);
		total = total + InterpolatedNoise_1(x * frequency, y * frequency) * amplitude;
	}
	return total;
}