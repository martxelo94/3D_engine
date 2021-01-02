
#ifndef DEMO_LOD_TERRAIN_H
#define DEMO_LOD_TERRAIN_H

struct Demo_lod_terrain : public Demo
{
	void update() override;
	void render() const override;
	Demo_lod_terrain();
	~Demo_lod_terrain();

	// render passes
	void render_terrain(const mat4& proj_mtx, const mat4& view_mtx) const;
	void render_skybox(const mat4& proj_mtx, const mat4& view_mtx) const;
	void deferred_shading(const mat4& proj_mtx, const mat4& view_mtx) const;
	void forward_rendering(const mat4& proj_mtx, const mat4& view_mtx) const;
	void post_processing() const;
	void draw_world_axis(const mat4& proj_mtx, const mat4& view_mtx) const;
	void draw_camera_frustrum(const mat4& proj_mtx, const mat4& view_mtx, const Camera& target) const;

	Camera camera;
	Camera terrain_camera;
	bool terrain_camera_mode = true;
	vec3 camera_delta_pos;
	vec2 mouse_velocity = { 100.f, 100.f };
	float camera_axis_speed = 20.f;
#if CAMERA_ZOOM
	float zoom_speed = 4.f;
#endif
	DirectionalLight directional_light;	// sun light
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

	bool animate_scene = false;
	float animation_time = 0.f;

	// models
	MeshBuffers mb_sphere, mb_cone, mb_frustrum, mb_cube;
	// grid meshes
	MeshBuffers mb_lod_grid, mb_terrain[(int)LOD_terrain_meshes::mesh_count];


	// antialiasing
	bool show_bloom = true;
	bool show_antialiasing = true;
	u32 blur_radius = 5;

	// deferred shading buffers
	GLuint g_buffer = 0;
	GLuint g_buffer_textures[MAX_G_BUFFER_CHANNELS] = { 0 };
	int current_showing_texture = -1;

	// postprocessing stuff
	GLuint postpo_framebuffer = 0;
	GLuint postpo_texture = 0;	// main scene rendered here
	GLuint postpo_depth_buffer = 0;


	// auxiliar framebuffer
#define AUX_BUFF_COUNT 3
	GLuint aux_framebuffer[AUX_BUFF_COUNT] = { 0 };
	GLuint aux_texture[AUX_BUFF_COUNT] = { 0 };

	// noise
	bool show_skybox = true;
	std::vector<vec4> noise_colors;
	float freq = 1.f, lacunarity = 1.f, persistance = 1.f;
	int oct = 0;
	vec3 noise_offset;

	// skybox
	GLuint cubemap_texture = 0;

	// TERRAIN
	float terrain_height_scale = 1.f;
	float terrain_scale = 1.f;
	bool show_wireframe = false;
	int terrain_mesh = 1;	// static LOD, grid LOD
	int terrain_lod = 6;
	int terrain_grid_segments = 1;
	float terrain_grid_margin_proportion = 0.3f;	// how much of the segment size is offseted as a margin
	bool use_mipmaps = true;
	bool use_color_mipmaps = false;
	bool use_noise_texture = true;
	bool terrain_frustrum_culling = true;
	float snap_precision = 1.f;
	GLuint noise_height_texture = 0;
	GLuint height_texture = 0;
	GLuint color_texture = 0;
	Material terrain_material;

};


#endif