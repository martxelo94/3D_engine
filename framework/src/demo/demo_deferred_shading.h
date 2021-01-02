/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: Demo_deferred_shading.h
Purpose: run assignment 1
Language: c++
Platform: windows 10
Project: cs562_markel.p_1
Author: Markel Pisano Berrojalbiz
Creation date: 9/18/2019
----------------------------------------------------------------------------------------------------------*/

#ifndef DEMO_DEFERRED_SHADING_H
#define DEMO_DEFERRED_SHADING_H
struct Demo_deferred_shading : public Demo
{
	void update() override;
	void render() const override;
	Demo_deferred_shading();
	~Demo_deferred_shading();

	// render passes
	void render_skybox(const mat4& proj_mtx, const mat4& view_mtx) const;
	void forward_shading(const mat4& proj_mtx, const mat4& view_mtx) const;
	void deferred_shading(const mat4& proj_mtx, const mat4& view_mtx) const;
	void forward_rendering(const mat4& proj_mtx, const mat4& view_mtx) const;
	void post_processing() const;


	Camera camera;
	vec2 mouse_velocity = { 100.f, 100.f };
	float camera_axis_speed = 20.f;
	float zoom_speed = 4.f;

	const static u32 MAX_LIGHT_COUNT = 1000;
	float light_init_height[MAX_LIGHT_COUNT] = { 0.f };
	PointLight point_light[MAX_LIGHT_COUNT];
	u32 light_count = 0;
	float animation_speed = 10.f;
	vec3 ambient_light = vec3{ 0.2f };
	float min_attenuation = 0.001f;
	float light_intensity = 1.f;
	bool do_scissor_test = true;
	bool show_scissor_test = false;

	bool animate_scene = true;
	float animation_time = 0.f;

	// models
	//MeshBuffers mb_sphere, mb_quad, mb_cube;	// already in graphics
	MeshBuffers model_sponza;

	// noise skybox
	std::vector<vec4> skybox_colors;
	float freq = 1.f, lacunarity = 1.f, persistance = 1.f;
	int oct = 0;
	vec3 noise_offset;


	// deferred shading buffers
	GLuint g_buffer = 0;
	GLuint g_buffer_textures[MAX_G_BUFFER_CHANNELS] = { 0 };
	int current_showing_texture = -1;

	// postprocessing stuff
	GLuint postpo_framebuffer = 0;
	GLuint postpo_texture = 0;	// main scene rendered here
	GLuint postpo_depth_buffer = 0;

	// antialiasing
	u32 blur_radius = 1;
	bool show_bloom = true;
	bool show_antialiasing = true;

	// auxiliar framebuffer
#define AUX_BUFF_COUNT 3
	GLuint aux_framebuffer[AUX_BUFF_COUNT] = { 0 };
	GLuint aux_texture[AUX_BUFF_COUNT] = { 0 };

};


#endif