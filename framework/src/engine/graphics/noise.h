
#ifndef NOISE_H
#define NOISE_H

namespace Noise
{
	using Method = float(*)(vec2, float);

	std::vector<float> create_depth_texture(Method noise_method, u32 size, vec2 offset, float freq, float lacunarity, float persistence, u8 oct);
	std::vector<Color> create_color_texture(Method noise_method, u32 size, vec2 offset, float freq, float lacunarity, float persistence, u8 oct);
	std::vector<Color> depth_to_gradient_color(const std::vector<float> &depth);

	// noise methods
	float perlin2D(vec2 point, float freq);
	float perlin3D(vec3 point, float freq);
	float value2D(vec2 point, float freq);

	float perlin2D(vec2 point, float freq, float lacunarity, float persistence, u8 oct);
	float perlin3D(vec3 point, float freq, float lacunarity, float persistence, u8 oct);

};

#endif