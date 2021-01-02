/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: Demo_tesselation.h
Purpose: run assignment 2
Language: c++
Platform: windows 10
Project: cs562_markel.p_2
Author: Markel Pisano Berrojalbiz
Creation date: 10/24/2019
----------------------------------------------------------------------------------------------------------*/

#ifndef DEMO_TESSELATION_H
#define DEMO_TESSELATION_H

struct Demo_tesselation : public Demo
{
	void update() override;
	void render() const override;
	Demo_tesselation();
	~Demo_tesselation();

	Camera camera;
	vec2 mouse_velocity = { 100.f, 100.f };
	float camera_axis_speed = 20.f;
	float zoom_speed = 4.f;

	const static u32 MAX_LIGHT_COUNT = 1000;
	PointLight point_light[MAX_LIGHT_COUNT];
	u32 light_count = 1;
	vec3 ambient_light = vec3{ 0.2f };
	float min_attenuation = 0.001f;
	float light_intensity = 20.f;
	bool do_scissor_test = true;
	bool show_scissor_test = false;

	// models
	MeshBuffers mb_sphere, mb_suzanne;
	Material material;

	// tessellation
	bool adaptive_tessellation = false;
	bool adaptive_color = false;
	bool show_wireframe = false;
	bool show_normals = false;
	float tess_levels = 1.f;
	float tess_alpha = 0.5f;


	// deferred shading buffers
	GLuint g_buffer = 0;
	GLuint g_buffer_textures[MAX_G_BUFFER_CHANNELS] = { 0 };
	int current_showing_texture = -1;

	// postprocessing stuff
	GLuint postpo_framebuffer = 0;
	GLuint postpo_texture = 0;	// main scene rendered here
	GLuint postpo_depth_buffer = 0;
};


#endif