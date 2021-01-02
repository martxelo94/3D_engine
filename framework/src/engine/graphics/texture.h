/**
* @file texture.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/30	(eus)
* @brief Define texture
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef TEXTURE_H
#define TEXTURE_H

// used in Graphics texture array as indices
enum Texture : int { no_texture = -1, t_depth_map = 0, t_light_acc, t_skybox, t_white, t_black, t_red, t_green, t_blue, t_pattern, texture_count };

// LOAD TEXTURES
void load_texture(GLuint *texture_id, const char* filepath, bool generate_mipmaps);
// color checker pattern texture
void load_pattern_texture(GLuint * texture_id, const u32 pixel_size = 6);
// Single pixel white texture
void load_color_texture(GLuint * texture_id, Color color);
// compute random noise texture
void load_noise_texture(GLuint *texture_id, size_t size, bool generate_mipmaps = false);
// compute perlin noise texture
void load_noise_texture(GLuint *texture_id, size_t size, vec2 offset, float frequency, float lacunarity, float persistence, u8 oct, bool generate_mipmaps = false);
// depth texture for shadowing
void load_depth_texture(GLuint *texture_id, const char* filepath, bool generate_mipmaps = false);
// cubemap texture from 6 sources
void load_cubemap(GLuint *texture_id, const char* filepaths[6], bool generate_mipmaps = false);

#endif