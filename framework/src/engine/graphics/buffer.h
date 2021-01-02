/**
* @file buffer.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/30	(eus)
* @brief Declare buffer
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef BUFFER_H
#define BUFFER_H

#define SHADOW_MAP_W 1024
#define SHADOW_MAP_H 1024
#define MAX_G_BUFFER_CHANNELS 4	// position, normals, albedo, depth


// create buffers for shadow shading
void create_depth_buffers(GLuint &depth_fbo, GLuint &depth_texture, u32 texture_width = SHADOW_MAP_W, u32 texture_height = SHADOW_MAP_H);
// create framebuffers for environment mapping
void create_cubemap_framebuffers(GLuint(&framebuffers)[6], GLuint &cubemap, u32 resolution = 1024);
// G-Buffer (geometry pass) for deferred shading
void create_g_buffer(GLuint &g_buffer, GLuint(&g_buffer_textures)[MAX_G_BUFFER_CHANNELS], u32 tex_width = WINDOW_X, u32 tex_height = WINDOW_Y);
// buffer for light accumulation
void create_framebuffer(GLuint &framebuffer, GLuint &out_texture, GLuint *depth_buffer = nullptr, u32 texture_width = WINDOW_X, u32 texture_height = WINDOW_Y);
// HDR: Hight Dynamic Range (texture with HALF_FLOAT channels)
void create_hdr_framebuffer(GLuint &framebuffer, GLuint &out_texture, GLuint *depth_buffer = nullptr, u32 texture_width = WINDOW_X, u32 texture_height = WINDOW_Y);
// decals frambuffer
void create_decal_framebuffer(GLuint &framebuffer, GLuint &out_diffuse_tex, GLuint &out_normal_tex, GLuint* depth_buffer = nullptr, u32 texture_width = WINDOW_X, u32 texture_height = WINDOW_Y);

#endif