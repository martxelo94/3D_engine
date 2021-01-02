/**
* @file buffer.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/30	(eus)
* @brief Implement buffer
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>

// create buffers for shadow shading
void create_depth_buffers(GLuint &depth_fbo, GLuint &depth_texture, u32 texture_width, u32 texture_height)
{
	// using clean buffers
	assert(depth_fbo == 0);
	assert(depth_texture == 0);

	// SHADOW MAP INITIALIZATION
	glGenFramebuffers(1, &depth_fbo);	// init frame buffer to store depth map
										// generate texture
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// tell this buffer we are not using colors
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// unbind depth buffer

}
// create framebuffers for environment mapping
void create_cubemap_framebuffers(GLuint(&framebuffers)[6], GLuint &cubemap, u32 resolution)
{
	// using clean buffers
	for(int i = 0; i < 6; ++i)
		assert(framebuffers[i] == 0);
	assert(cubemap == 0);

	// delete posible buffers first
	glDeleteFramebuffers(6, framebuffers);
	glDeleteTextures(1, &cubemap);

	// create 6 frame buffers
	glGenFramebuffers(6, framebuffers);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &cubemap);	// use last cubemap to render on it
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	//create 2D images
	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach 6 buffers

	for (int i = 0; i < 6; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);
		GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Cubemap's framebuffer " << i << " failed to attach!" << std::endl;
			glDeleteFramebuffers(6, framebuffers);
			glDeleteTextures(1, &cubemap);
			break;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	pop_gl_errors(__FUNCTION__);

}
// create a geometry buffer for deferred shading technique
void create_g_buffer(GLuint &g_buffer, GLuint(&g_buffer_textures)[MAX_G_BUFFER_CHANNELS], u32 tex_width, u32 tex_height)
{
	// using clean buffers
	for (int i = 0; i < MAX_G_BUFFER_CHANNELS; ++i)
		assert(g_buffer_textures[i] == 0);
	assert(g_buffer == 0);

	glGenFramebuffers(1, &g_buffer);
	glGenTextures(MAX_G_BUFFER_CHANNELS, g_buffer_textures);
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);

	// position color
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tex_width, tex_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_buffer_textures[0], 0);
	pop_gl_errors("position buffer");

	// normal + shininess color
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, tex_width, tex_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_buffer_textures[1], 0);
	pop_gl_errors("normal buffer");

	// albedo (diffuse) + specular (shininess)
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_buffer_textures[2], 0);
	pop_gl_errors("albedo buffer");

	// depth
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, tex_width, tex_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// tell this buffer we are not using colors
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_buffer_textures[3], 0);
	pop_gl_errors("depth buffer");

	// tell OpenGL which color attachments we'll use for rendering
	GLuint attachments[MAX_G_BUFFER_CHANNELS - 1] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(sizeof(attachments) / sizeof(GLuint), attachments);


	//finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Geometry Buffer not complete!" << std::endl;

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pop_gl_errors(__FUNCTION__);
}
void create_framebuffer(GLuint &framebuffer, GLuint &out_texture, GLuint *depth_buffer, u32 texture_width, u32 texture_height)
{
	// using clean buffers
	assert(framebuffer == 0);
	assert(out_texture == 0);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenTextures(1, &out_texture);
	glBindTexture(GL_TEXTURE_2D, out_texture);

	// light color channels (RGB)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_texture, 0);

	// tell OpenGL which color attachments we'll use for rendering
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// depth buffer
	if (depth_buffer) {
		assert(*depth_buffer == 0);
		// with texture
		glGenTextures(1, depth_buffer);
		glBindTexture(GL_TEXTURE_2D, *depth_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// tell this buffer we are not using colors
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depth_buffer, 0);
	}

	//finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pop_gl_errors(__FUNCTION__);

}
// buffer for light accumulation
void create_hdr_framebuffer(GLuint &framebuffer, GLuint &out_texture, GLuint *depth_buffer, u32 texture_width, u32 texture_height)
{
	// using clean buffers
	assert(framebuffer == 0);
	assert(out_texture == 0);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenTextures(1, &out_texture);
	glBindTexture(GL_TEXTURE_2D, out_texture);

	// color channels (RGB)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, texture_width, texture_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_texture, 0);

	// tell OpenGL which color attachments we'll use for rendering
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// depth buffer
	if (depth_buffer) {
		if (*depth_buffer == 0) {
// with texture
			glGenTextures(1, depth_buffer);
			glBindTexture(GL_TEXTURE_2D, *depth_buffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width, tex_height, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
		// tell this buffer we are not using colors
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depth_buffer, 0);
	}

	//finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pop_gl_errors(__FUNCTION__);
}
// decals frambuffer
void create_decal_framebuffer(GLuint &framebuffer, GLuint &out_diffuse_tex, GLuint &out_normal_tex, GLuint* depth_buffer, u32 texture_width, u32 texture_height)
{
	// delete posible buffers first
	glDeleteFramebuffers(1, &framebuffer);
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// diffuse texture
	if (out_diffuse_tex == 0) {
		glGenTextures(1, &out_diffuse_tex);
		glBindTexture(GL_TEXTURE_2D, out_diffuse_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_diffuse_tex, 0);
	pop_gl_errors("diffuse buffer");

	// normal texture
	if (out_normal_tex == 0) {
		glGenTextures(1, &out_normal_tex);
		glBindTexture(GL_TEXTURE_2D, out_normal_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texture_width, texture_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, out_normal_tex, 0);
	pop_gl_errors("normal buffer");
	// depth buffer
	if (depth_buffer) {
		if (*depth_buffer == 0) {
#if 0
			glDeleteRenderbuffers(1, depth_buffer);
			glGenRenderbuffers(1, depth_buffer);
			glBindRenderbuffer(GL_RENDERBUFFER, *depth_buffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture_width, texture_height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *depth_buffer);
#else	// with texture
			glGenTextures(1, depth_buffer);
			glBindTexture(GL_TEXTURE_2D, *depth_buffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width, tex_height, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
#endif
		// tell this buffer we are not using colors
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depth_buffer, 0);
	}

	// tell OpenGL which color attachments we'll use for rendering
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(sizeof(attachments) / sizeof(GLuint), attachments);


	//finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Decals Buffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pop_gl_errors(__FUNCTION__);

}
