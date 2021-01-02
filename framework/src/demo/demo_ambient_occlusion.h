#ifndef DEMO_AMBIENT_OCCLUSION_H
#define DEMO_AMBIENT_OCCLUSION_H

struct Demo_ambient_occlusion : public Demo
{
	void update() override;
	void render() const override;
	Demo_ambient_occlusion();
	~Demo_ambient_occlusion();

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
	MeshBuffers mb_sphere, mb_quad, mb_cube;
	MeshBuffers model_sponza;
	vec3 sponza_pos, sponza_scale;

	// noise skybox
	bool show_skybox = false;
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

	// decals
	GLuint decal_framebuffer = 0;
	GLuint decal_textures[2] = { 0 };	// normals + diffuse
	std::vector<GLuint> decals_diffuse, decals_normal;
	std::vector<mat4> decals_transform;
	float angle_limit = 89.f;	// degrees
	bool show_decal_boxes = false;
	bool draw_decals = true;
	int decal_render_mode = 0;	// all, plane, box

	// Ambient Occlusion	(ao)
	bool show_ambient_occlusion = true;
	bool use_ao_blur = true;
	int ao_blur_type = 0;	// Gaussian, Bilateral
	int ao_type = 0;		// HBAO, SSAO
	float ao_radius = 0.3f;
	float ao_angle_bias = 0.f;
	u32 ao_direction_count = 6;
	u32 ao_step_count = 6;
	float ao_attenuation = 1.f;
	float ao_scale = 1.f;

	GLuint noise_texture = 0;

	// blur
	int ao_blur_radius = 1;
	float blur_deviation = 1.f;
	float blur_range_deviation = 1.f;
	float blur_spatial_deviation = 1.f;
};


#endif