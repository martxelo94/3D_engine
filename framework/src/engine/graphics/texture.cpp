/**
* @file texture.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/30	(eus)
* @brief Implement texture
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>
// stb
//#define STB_IMAGE_STATIC
//#define STB_IMAGE_IMPLEMENTATION // compile stb implementation, not just headers
//#include <stb/stb_image.h>

/**
*
* @param texture_id
* @param pixel_size
*/
void load_pattern_texture(GLuint * texture_id, const u32 pixel_size)
{
	assert(texture_id);
	assert(*texture_id == 0); // texture is free already

	const Color colors[] = {
		0x0000ffff /*blue*/, 0x00ffffff /*cyan*/, 0x00ff00ff /*green*/,
		0xffff00ff/*yellow*/, 0xff0000ff/*red*/, 0xff69b4ff /*pink*/
	};
	const u32 colorCount = sizeof(colors) / sizeof(Color);
	assert(pixel_size >= colorCount);
	const u32 colorByteSize = 4;	/* RGBA -> MUST BE 4-byte aligned */
	const u32 widthBytes = pixel_size * colorByteSize;
	const u32 colorQuadByteWidth = widthBytes / colorCount;
	const u32 colorQuadByteHeight = pixel_size / colorCount;
	//allocate mem
	std::vector<u8> data(pixel_size * widthBytes, 0);
	for (u32 y = 0; y < pixel_size; ++y)
	{
		for (u32 colorChunk = 0; colorChunk < colorCount; ++colorChunk)
		{
			const u32 idx = (pixel_size - y - 1) * widthBytes + colorChunk * colorQuadByteWidth;
			const u32 color_idx = (colorChunk + (y / colorQuadByteHeight)) % colorCount;
			const Color& c = colors[color_idx];
			for (u32 currPixIdx = 0; currPixIdx < colorQuadByteWidth; currPixIdx += colorByteSize)
			{
				const u32 & color_int = (const u32&)c;
				std::memcpy(data.data() + idx + currPixIdx, &color_int, colorByteSize * sizeof(u8));
			}
		}
	}
	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_size, pixel_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
	pop_gl_errors(__FUNCTION__);
}
/**
*
* @param texture_id
* @param color
*/
void load_color_texture(GLuint * texture_id, Color color) {
	// Single pixel white texture
	assert(texture_id);
	assert(*texture_id == 0); // texture is free already

	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &u32(color));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
	pop_gl_errors(__FUNCTION__);
}
/**
*
* @param texture_id
* @param filepath
* @param invert_y
* @param generate_mipmaps
*/
void load_texture(GLuint *texture_id, const char* filepath, bool generate_mipmaps)
{
	/*
	int x, y, n;
	u8* data = stbi_load(filepath, &x, &y, &n, STBI_default);
	if (data == nullptr) {
		std::cerr << "Failed loading \"" << filepath << "\"" << std::endl;
		std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
		return;
	}
	assert(texture_id);
	assert(*texture_id == 0); // texture is free already

	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (generate_mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	*/
}

void load_depth_texture(GLuint *texture_id, const char* filepath, bool generate_mipmaps)
{
	/*
	int x, y, n;
	u8* data = stbi_load(filepath, &x, &y, &n, STBI_grey);
	if (data == nullptr) {
		std::cerr << "Failed loading \"" << filepath << "\"" << std::endl;
		std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
		return;
	}
	assert(texture_id);
	assert(*texture_id == 0); // texture is free already


	std::vector<float> depths(x * y, 0.f);
	for (int i = 0; i < depths.size(); i++)
		depths[i] = (float)reinterpret_cast<u8*>(data)[i * n] / 255;

	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depths.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (generate_mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	pop_gl_errors(__FUNCTION__);
	*/
}
void load_cubemap(GLuint *texture_id, const char* filepaths[6], bool generate_mipmaps)
{
	/*
	assert(texture_id);
	assert(*texture_id == 0); // texture is free already

	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *texture_id);

	for (int i = 0; i < 6; i++) {
		int x, y, n;
		u8* data = stbi_load(filepaths[i], &x, &y, &n, STBI_default);
		if (data) {
			assert(n == 4 || n == 3);
			const unsigned format_offset = n == 4 ? 1 : 0;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB + format_offset, x, y, 0, GL_RGB + format_offset, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else {
			// failed loading map, erase texture and return
			glDeleteTextures(1, texture_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			std::cerr << "Failed loading " << filepaths[i] << " as cubemap." << std::endl;
			std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
			return;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (generate_mipmaps)	// DOES IT WORK? DUNNO!
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	pop_gl_errors(__FUNCTION__);
	*/
}
void load_noise_texture(GLuint *texture_id, size_t size, bool generate_mipmaps)
{
	assert(texture_id);
	assert(*texture_id == 0); // texture is free already

	std::vector<float> pixels(size * size * 4, 0);
	for (int y = 0; y < size; ++y) {
		for (int x = 0; x < size; ++x) {
			vec2 xy = glm::circularRand(1.0f);
			float z = glm::linearRand(0.f, 1.f);
			float w = glm::linearRand(0.f, 1.f);

			int idx = 4 * (x + y * size);
			pixels[idx] = xy.x;
			pixels[idx + 1] = xy.y;
			pixels[idx + 2] = z;
			pixels[idx + 3] = w;
		}
	}

	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size, size, 0, GL_RGBA, GL_FLOAT, pixels.data());



	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	if (generate_mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	pop_gl_errors(__FUNCTION__);
}
void load_noise_texture(GLuint *texture_id, size_t size, vec2 offset, float frequency, float lacunarity, float persistence, u8 oct, bool generate_mipmaps)
{
	assert(texture_id);
	assert(*texture_id == 0); // texture is free already

	std::vector<float> pixels = Noise::create_depth_texture(Noise::perlin2D, size, offset, frequency, lacunarity, persistence, oct);

	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixels.data());


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	if (generate_mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	pop_gl_errors(__FUNCTION__);
}
