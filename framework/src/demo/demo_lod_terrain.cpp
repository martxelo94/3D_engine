/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: demo_lod_terrain.cpp
Purpose: run assignment 4
Language: c++
Platform: windows 10
Project: cs562_markel.p_4
Author: Markel Pisano Berrojalbiz
Creation date: 11/14/2019
----------------------------------------------------------------------------------------------------------*/

#include <pch.h>

Demo_lod_terrain::Demo_lod_terrain() {
#pragma region BUFFERS
	// create G-buffer
	create_g_buffer(g_buffer, g_buffer_textures);
	// create framebuffer for postprocessing
	create_hdr_framebuffer(postpo_framebuffer, postpo_texture, &postpo_depth_buffer);
	for (u32 i = 0; i < AUX_BUFF_COUNT; i++)
		create_framebuffer(aux_framebuffer[i], aux_texture[i]);
	//create decals frambuffer
#pragma endregion

#pragma region NOISE
	//skybox params
	noise_colors.push_back(vec4{ 212, 212, 247, 0 } / 255.f);
	noise_colors.push_back(vec4{ 61, 127, 81, 14.f } / 255.f);
	noise_colors.push_back(vec4{ 234, 221, 127, 24.f } / 255.f);
	noise_colors.push_back(vec4{ 193, 136, 59, 39.f } / 255.f);
	noise_colors.push_back(vec4{ 160, 79, 14, 70 } / 255.f);
	noise_colors.push_back(vec4{ 57, 16, 0, 92 } / 255.f);
	noise_colors.push_back(vec4{ 40, 19, 59, 114 } / 255.f);
	noise_colors.push_back(vec4{ 161, 66, 255, 147 } / 255.f);
	noise_colors.push_back(vec4{ 255, 255, 255, 184 } / 255.f);

	freq = 100.f;
	lacunarity = 0.3f;
	persistance = 2.7f;
	oct = 4;
#pragma endregion
#pragma region SKYBOX
	const char * skybox_files[6] = {
	"../resources/textures/space_right_1.png",
	"../resources/textures/space_left_1.png",
	"../resources/textures/space_top_1.png",
	"../resources/textures/space_bot_1.png",
	"../resources/textures/space_front_1.png",
	"../resources/textures/space_back_1.png"
	};
	load_cubemap(&cubemap_texture, skybox_files);

#pragma endregion
#pragma region CAMERA
	// camera setting
	camera.pos = { 0.f, 1.f, 0.f };
	camera.target = vec3{ -1.f, 0.f, 0.f } +camera.pos;
	camera.far_plane = 5000.f;
	camera_axis_speed = 1;
#pragma endregion
	// scene hack

	// mesh loading
	create_cube_mesh(&mb_cube, nullptr);
	create_sphere_mesh(&mb_sphere, nullptr);
	create_cone_mesh(&mb_cone, nullptr);
	Frustum frustum{ camera.get_proj() * camera.get_view() };
	create_frustum_mesh(frustum, &mb_frustrum, nullptr);

	load_color_texture(&graphics.tex[Texture::t_white], Color{});
	load_color_texture(&graphics.tex[Texture::t_black], Color{ 0x000000ff });

#pragma region LIGHTS
	directional_light.direction = glm::normalize(vec3(-1, 0.1, 0));
	directional_light.diffuse = Color{};
	ambient_light = vec3{ 0.5f };

	// randomize lights
	light_init_height[0] = 2.f;
	point_light[0].attenuation = { 1, 1, 1 };
	for (u32 i = 1; i < MAX_LIGHT_COUNT; i++) {
		point_light[i].position = vec3{ glm::linearRand(-100, 100), glm::linearRand(2, 2), glm::linearRand(-100, 100) };
		point_light[i].diffuse = Color{ (u8)glm::linearRand(0, 255), (u8)glm::linearRand(0, 255) , (u8)glm::linearRand(0, 255) , (u8)255 };
		point_light[i].attenuation = { 1, 1, 1 };

		light_init_height[i] = point_light[i].position.y;
	}
	light_count = 1;
#pragma endregion

#pragma region TERRAIN
	terrain_camera_mode = false;
	terrain_camera = camera;
	terrain_scale = 0.01f;
	terrain_material.shininess = 100;
	load_mesh_obj("../resources/meshes/plane_grid_2.obj", &mb_lod_grid);

	terrain_grid_segments = 128;
	create_terrain_meshes(mb_terrain, terrain_grid_segments, terrain_grid_margin_proportion);

	// height maps
	load_noise_texture(&noise_height_texture, 1024, noise_offset, freq, lacunarity, persistance, (u8)oct);
	//load_texture(&height_texture, "../resources/textures/earth_height_map.jpg", false);
	//load_texture(&height_texture, "../resources/textures/australia_height_map.jpg", true, use_mipmaps);
	load_texture(&height_texture, "../resources/textures/earthbump10k.jpg", use_mipmaps);
	load_texture(&color_texture, "../resources/textures/earthmap10k.jpg", use_mipmaps);
	terrain_material.t_diffuse = &color_texture;

#pragma endregion
}
Demo_lod_terrain::~Demo_lod_terrain() {
	// delete G-buffer
	glDeleteTextures(sizeof(g_buffer_textures) / sizeof(GLuint), g_buffer_textures);
	glDeleteFramebuffers(1, &g_buffer);

	glDeleteTextures(1, &postpo_texture);
	glDeleteRenderbuffers(1, &postpo_depth_buffer);
	glDeleteFramebuffers(1, &postpo_framebuffer);

	glDeleteTextures(AUX_BUFF_COUNT, aux_texture);
	glDeleteFramebuffers(AUX_BUFF_COUNT, aux_framebuffer);

	glDeleteTextures(1, &noise_height_texture);
	glDeleteTextures(1, &height_texture);
	glDeleteTextures(1, &color_texture);

	glDeleteTextures(1, &cubemap_texture);
}

void Demo_lod_terrain::update() {
	// FIRST PERSON CAMERA

#pragma region Camera Controller
	// store camera's position for substracting new position
	camera_delta_pos = camera.pos;

	// rotate
	if (mouse.pressed(MOUSE_RIGHT)) {
		vec2 ndc_motion = vec2{ (float)mouse.getMove().x / graphics.window.size.x, (float)mouse.getMove().y / graphics.window.size.y };

		vec3 forward = camera.target - camera.pos;
		forward = glm::rotate(forward, ndc_motion.x * -mouse_velocity.x * dt, vec3{ 0, 1, 0 });
		vec3 axis = glm::cross(forward, { 0, 1, 0 });
		axis = glm::normalize(axis);
		forward = glm::rotate(forward, ndc_motion.y * -mouse_velocity.y * dt, axis);
		camera.target = camera.pos + forward;

	}
	// move
	vec3 forward = camera.target - camera.pos;
	vec3 right = glm::cross({ 0, 1, 0 }, forward);
	//vec3 up = glm::cross(forward, right);

	if (keyboard.pressed(GLFW_KEY_W)) {
		camera.pos += glm::normalize(glm::cross(right, vec3{ 0,1,0 })) *camera_axis_speed * dt;
	}
	else if (keyboard.pressed(GLFW_KEY_S)) {
		camera.pos -= glm::normalize(glm::cross(right, vec3{ 0,1,0 })) * camera_axis_speed * dt;
	}
	if (keyboard.pressed(GLFW_KEY_A)) {
		camera.pos += right * camera_axis_speed * dt;
	}
	if (keyboard.pressed(GLFW_KEY_D)) {
		camera.pos -= right * camera_axis_speed * dt;
	}
	if (keyboard.pressed(GLFW_KEY_Q)) {
		camera.pos -= vec3{ 0, 1, 0 } *camera_axis_speed * dt;
	}
	if (keyboard.pressed(GLFW_KEY_E)) {
		camera.pos += vec3{ 0, 1, 0 } *camera_axis_speed * dt;
	}
	camera.target = camera.pos + forward;

	// update velocity
	camera_delta_pos = camera.pos - camera_delta_pos;
	//if (glm::length2(vec2{ camera_delta_pos.x, camera_delta_pos.z }) > EPSILON) {
	//	//update light direction and skybox indirectly
	//	vec3 rot_vec = glm::normalize(glm::cross(vec3{ camera_delta_pos.x, 0.f, camera_delta_pos.z }, vec3{ 0, 1, 0 }));
	//	mat4 position_rot = glm::rotate(glm::length(vec2{ camera_delta_pos.x, camera_delta_pos.z }), rot_vec);
	//	directional_light.direction = vec3{ position_rot * vec4{ directional_light.direction, 1 } };
	//}
	mat4 position_rot = glm::rotate(camera_delta_pos.x * 0.05f, vec3{ 0, 0, 1 });
	directional_light.direction = vec3{ position_rot * vec4{directional_light.direction, 1} };

#pragma endregion

	if (terrain_camera_mode) {
		terrain_camera = camera;
	}

#pragma region Place light bulb
#if 0
	//intersect raycast with fixed plane
	if (mouse.triggered(LMB)) {
		vec3 rayDir = glm::normalize(forward); /* taken from Camera Controller above */
					  //camera.target - camera.pos;
		vec3 intersection;
		if (intersect_ray_to_plane(camera.pos, rayDir, plane_equation(vec3{ 0, 5, 0 }, vec3{ 0, 1, 0 }), &intersection)) {
			point_light[light_count++].position = intersection;
		}
	}
#endif
#pragma endregion

	if (animate_scene) {
		float h = sin(animation_time * animation_speed);
		for (u32 i = 0; i < light_count; i++) {
			point_light[i].position.y = light_init_height[i] + h; // sin(animation_speed * dt * (frame_count % 360) * DEG_TO_RAD);
		}
		//freq += h * 3 * dt;	// NO!
		light_intensity = 3 - 3 * h;

		animation_time += dt;
	}

#pragma region IMGUI
	//IMGUI STUFF
	int flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	bool gui_open = true;
	ImGui::Begin("Terrain", &gui_open, flags);
	ImGui::SetWindowPos({ 0, 50 });
	//Scene settings...
	if (ImGui::CollapsingHeader("CAMERA")) {
		ImGui::DragFloat("Near Plane", &camera.near_plane, 0.1f, 0.001f, 300);
		ImGui::DragFloat("Far Plane", &camera.far_plane, 0.1f, 0.001f, 30000);
		float fov_angle = glm::degrees(camera.fov);
		ImGui::DragFloat("FoV", &fov_angle, 0.1f, -180.f, 180.f);
		camera.fov = glm::radians(fov_angle);
#if CAMERA_ZOOM
		ImGui::DragFloat("Zoom Speed", &zoom_speed, 0.1f, 0.f, 10.f);
#endif
		ImGui::DragFloat("Speed", &camera_axis_speed, 0.1f, 0.1f, 1000.f);
		ImGui::DragFloat2("Rotation Speed", &mouse_velocity[0], 1.f, 1.f, 500.f);
		ImGui::Checkbox("Terrain Camera", &terrain_camera_mode);
		ImGui::SameLine();
		if (ImGui::Button("Update Frustrum")) {
			mb_frustrum.clear();
			Frustum frustum{ terrain_camera.get_proj() * terrain_camera.get_view() };
			create_frustum_mesh(frustum, &mb_frustrum);
		}
	}
	if (ImGui::CollapsingHeader("RENDER OPTIONS")) {
		std::string render_target_name;
		switch (current_showing_texture) {
		case -1:
			render_target_name = "Deferred Shading";
			break;
		case 0:
			render_target_name = "Positions";
			break;
		case 1:
			render_target_name = "Normals";
			break;
		case 2:
			render_target_name = "Albedo";
			break;
		case 3:
			render_target_name = "Specular";
			break;
		case 4:
			render_target_name = "Depth";
			break;
		case 5:
			render_target_name = "Sobel";
			break;
		case 6:
			render_target_name = "HDR";
			break;
		case 7:
			render_target_name = "HDR + Blur";
			break;
		case 8:
			render_target_name = "Ambient Occlusion";
			break;
		case 9:
			render_target_name = "Ambient Occlusion + Blur";
			break;
		}
		if (ImGui::Button("Recompile Shaders")) {
			graphics.delete_shaders();
			graphics.compile_shaders();
		}
		if (ImGui::BeginMenu(std::string("Render Target: " + render_target_name).c_str())) {
			if (ImGui::MenuItem("DEFERRED SHADING"))
				current_showing_texture = -1;
			else if (ImGui::MenuItem("Positions"))
				current_showing_texture = 0;
			else if (ImGui::MenuItem("Normals"))
				current_showing_texture = 1;
			else if (ImGui::MenuItem("Albedo"))
				current_showing_texture = 2;
			else if (ImGui::MenuItem("Specular"))
				current_showing_texture = 3;
			else if (ImGui::MenuItem("Depth"))
				current_showing_texture = 4;
			else if (ImGui::MenuItem("Sobel"))
				current_showing_texture = 5;
			else if (ImGui::MenuItem("HDR"))
				current_showing_texture = 6;
			else if (ImGui::MenuItem("HDR + Blur"))
				current_showing_texture = 7;

			ImGui::EndMenu();
		}
	}

	if (ImGui::CollapsingHeader("POST-PROCESSING")) {
		ImGui::Checkbox("Antialiasing", &show_antialiasing);
		ImGui::Checkbox("Bloom", &show_bloom);
		ImGui::DragInt("Blur Radius", reinterpret_cast<int*>(&blur_radius), 1, 0, 50);
	}

	//Lights
	if (ImGui::CollapsingHeader("LIGHTS")) {
		ImGui::Checkbox("Skybox", &show_skybox);
		directional_light.edit("Directional Light");
		ImGui::DragInt("Light count", reinterpret_cast<int*>(&light_count), 1.f, 0, MAX_LIGHT_COUNT);
		if (ImGui::TreeNode("Point Lights")) {
			for (u32 i = 0; i < light_count; i++)
				point_light[i].edit(std::to_string(i).c_str());
			ImGui::TreePop();
		}
		ImGui::ColorEdit3("Ambient", &ambient_light[0]);
		ImGui::DragFloat("Intensity", &light_intensity, 0.1f, 0.1f, 100.f);
		ImGui::Checkbox("Animate", &animate_scene);
		ImGui::DragFloat("Anim Speed", &animation_speed);
		ImGui::DragFloat("Min Att", &min_attenuation, 0.001f, 0.f, 10.f);
		ImGui::Checkbox("Scissor Opt", &do_scissor_test);
		ImGui::SameLine();
		ImGui::Checkbox("Show", &show_scissor_test);
	}
	// TERRAIN
	if (ImGui::CollapsingHeader("TERRAIN")) {
		terrain_material.edit("Material");

		ImGui::Checkbox("Wireframe", &show_wireframe);

		std::string render_target_name;
		switch (terrain_mesh) {
		case 0:
			render_target_name = "LOD_Mesh";
			break;
		case 1:
			render_target_name = "Uniform Grid";
			break;
		}
		if (ImGui::BeginMenu(std::string("Terrain Mesh: " + render_target_name).c_str())) {
			if (ImGui::MenuItem("LOD_Mesh"))
				terrain_mesh = 0;
			else if (ImGui::MenuItem("Uniform Grid"))
				terrain_mesh = 1;

			ImGui::EndMenu();
		}
		if (terrain_mesh == 1) {
			ImGui::SliderInt("LOD", &terrain_lod, 0, 20);
			ImGui::SliderInt("Grid Segments", &terrain_grid_segments, 1, 256);
			ImGui::DragFloat("Snap Precisioin", &snap_precision, 0.01f, 0.01f, 10.f);
			ImGui::DragFloat("Margin", &terrain_grid_margin_proportion, 0.01f, 0.f, 1.f);
			if (ImGui::Button("Contruct Grid")) {
				//mb_uniform_grid.clear();
				//create_grid_mesh(&mb_uniform_grid, nullptr, terrain_grid_segments);
				for (int i = 0; i < (int)LOD_terrain_meshes::mesh_count; ++i)
					mb_terrain[i].clear();
				create_terrain_meshes(mb_terrain, terrain_grid_segments, terrain_grid_margin_proportion);


			}
			ImGui::Checkbox("Frustrum Culling", &terrain_frustrum_culling);
		}

		ImGui::DragFloat("Scale", &terrain_scale, 0.01f, 0.01f, 100.f);
		ImGui::DragFloat("Height Scale", &terrain_height_scale, 0.01f, 0.01f, 100.f);
		ImGui::Checkbox("Noise", &use_noise_texture);
		if (use_noise_texture) {
			ImGui::DragFloat("Freq", &freq, 0.001f, 0.001f, 100.f);
			ImGui::DragFloat("Lacunarity", &lacunarity, 0.1f, 0.001f, 100.f);
			ImGui::DragFloat("Persistance", &persistance, 0.1f, 0.001f, 100.f);
			ImGui::DragInt("Octaves", &oct, 1, 0, 6);
			if (ImGui::Button("Reload Map"))
				load_noise_texture(&noise_height_texture, 1024, noise_offset, freq, lacunarity, persistance, (u8)oct);
		}

		if (ImGui::Checkbox("Mipmaps", &use_mipmaps)) {
			//load_texture(&height_texture, "../resources/textures/australia_height_map.jpg", true, use_mipmaps);
			load_texture(&height_texture, "../resources/textures/earthbump10k.jpg", use_mipmaps);
			load_texture(&color_texture, "../resources/textures/earthmap10k.jpg", use_mipmaps);

		}
		ImGui::SameLine();
		ImGui::Checkbox("MP colors", &use_color_mipmaps);


		ImGui::Text("Colors");
		if (ImGui::Button("Add")) {
			noise_colors.push_back(vec4{ 0, 0, 0, 1 });
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove")) {
			noise_colors.pop_back();
		}
		for (u32 i = 0; i < noise_colors.size(); i++) {
			vec4 &c = noise_colors[i];
			ImGui::ColorEdit4(std::to_string(i).c_str(), &c[0]);
		}

	}


	ImGui::End();
#pragma endregion

}

void Demo_lod_terrain::render() const {

	// camera matrix
	mat4 view_mtx = camera.get_view();
	mat4 proj_mtx = camera.get_proj();



	try {


		deferred_shading(proj_mtx, view_mtx);


		forward_rendering(proj_mtx, view_mtx);

#pragma region DEBUG TEXTURES
		if (current_showing_texture == 4) {
			// render buffers
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glUseProgram(graphics.shader_program[sh_depth_color]);
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_depth_color], "M"), 1, false, &(mat4()[0][0]));
			glUniform1f(glGetUniformLocation(graphics.shader_program[sh_depth_color], "cam_far"), camera.far_plane);
			glUniform1f(glGetUniformLocation(graphics.shader_program[sh_depth_color], "cam_near"), camera.near_plane);
			graphics.primitives[primitive_quad].bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, postpo_depth_buffer);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glEnable(GL_DEPTH_TEST);
			pop_gl_errors("G-Buffer texture rendering");
			throw current_showing_texture;
		}
#pragma endregion

		post_processing();

		// SKYBOX
		if (show_skybox) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, postpo_framebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, WINDOW_X, WINDOW_Y, 0, 0, WINDOW_X, WINDOW_Y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			pop_gl_errors("Depth Buffer Blit");

			render_skybox(proj_mtx, view_mtx);
		}

		if (!terrain_camera_mode)
			draw_camera_frustrum(proj_mtx, view_mtx, terrain_camera);

		// height minimap
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, GLsizei(WINDOW_X * 0.2f), GLsizei(WINDOW_Y * 0.2f));
		glClear(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_ONE, GL_ONE);
		glUseProgram(graphics.shader_program[sh_red_color]);
		graphics.primitives[primitive_quad].bind();
		mat4 mtx = glm::translate(vec3{ -0.8f, -0.8f, 0.f }) * glm::scale(vec3{ 0.2f });
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_red_color], "M"), 1, false, &(mtx[0][0]));
		glActiveTexture(GL_TEXTURE0);
		// height map
		glBindTexture(GL_TEXTURE_2D, height_texture);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// noise map
		glBindTexture(GL_TEXTURE_2D, noise_height_texture);
		glDrawArrays(GL_TRIANGLES, 0, 4);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);


		draw_world_axis(proj_mtx, view_mtx);
	}
	catch (...) {}

	pop_gl_errors(__FUNCTION__);
}


#pragma region TERRAIN RENDERING	// called inside deferred_shading

void Demo_lod_terrain::render_terrain(const mat4& proj_mtx, const mat4& view_mtx) const
{
	glUseProgram(graphics.shader_program[sh_terrain_map]);

	// common matrices
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "VIEW"), 1, false, &(view_mtx[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "PROJ"), 1, false, &(proj_mtx[0][0]));

	//pass uniforms
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "freq"), freq);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "lacunarity"), lacunarity);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "persistance"), persistance);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "oct"), oct);
	glUniform2fv(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "offset"), 1, &noise_offset[0]);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "scale"), terrain_scale);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "height_scale"), terrain_height_scale);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "snap_precision"), snap_precision);

	if (use_color_mipmaps) {
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "use_color_mipmaps"), 1);
	}
	else
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "use_color_mipmaps"), 0);

	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "LayerCount"), (int)noise_colors.size());
	for (u32 i = 0; i < noise_colors.size(); i++) {
		glUniform4fv(
			glGetUniformLocation(graphics.shader_program[sh_terrain_map], std::string("Layers[" + std::to_string(i) + "]").c_str())
			, 1, &(noise_colors[i][0]));
	}

	terrain_material.set_uniforms(sh_terrain_map);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, height_texture);
	glActiveTexture(GL_TEXTURE4);
	if (use_noise_texture)
		glBindTexture(GL_TEXTURE_2D, noise_height_texture);
	else
		glBindTexture(GL_TEXTURE_2D, graphics.tex[t_black]);

	if (show_wireframe) {
		float line_width = 10.f / (sqrtf((float)terrain_grid_segments) + 1);
		glLineWidth(line_width);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glFrontFace(GL_CW);
	if (terrain_mesh == 0) {
		mat4 world_mtx = glm::translate(vec3{ camera.pos.x, 0, camera.pos.z }) * glm::scale(vec3{ 10.f });
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_terrain_map], "WORLD"), 1, false, &(world_mtx[0][0]));
		glBindVertexArray(mb_lod_grid.vao);
		glDrawElements(GL_TRIANGLES, mb_lod_grid.index_count, GL_UNSIGNED_INT, 0);
	}
	else if (terrain_mesh == 1) {
		draw_terrain_LOD(sh_terrain_map, mb_terrain, terrain_camera, (u8)terrain_lod, snap_precision, terrain_frustrum_culling);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	pop_gl_errors(__FUNCTION__);
}

#pragma endregion
#pragma region RENDERING
void Demo_lod_terrain::deferred_shading(const mat4& proj_mtx, const mat4& view_mtx) const
{
#pragma region GEOMETRY PASS
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. geometry pass: render all geometric/color data to G-buffer
	glDisable(GL_BLEND);
	//glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);


#pragma region DRAW TERRAIN
	render_terrain(proj_mtx, view_mtx);

	pop_gl_errors("TERRAIN RENDERING");
#pragma endregion

	pop_gl_errors("G-Buffer rendering");
#pragma endregion
#pragma region DEBUG TEXTURES
	if (current_showing_texture >= 0 && current_showing_texture < MAX_G_BUFFER_CHANNELS - 1) {
		// render buffers
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(graphics.shader_program[sh_raw]);

		graphics.primitives[primitive_quad].bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_buffer_textures[current_showing_texture]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glEnable(GL_DEPTH_TEST);
		pop_gl_errors("G-Buffer texture rendering");
		throw current_showing_texture;
	}
	else if (current_showing_texture == 3) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(graphics.shader_program[sh_alpha_color]);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_alpha_color], "M"), 1, false, &(mat4()[0][0]));

		graphics.primitives[primitive_quad].bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_buffer_textures[2]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glEnable(GL_DEPTH_TEST);
		pop_gl_errors("G-Buffer texture rendering");
		throw current_showing_texture;
	}
#pragma endregion

#pragma region LIGHT PASS
	glBindFramebuffer(GL_FRAMEBUFFER, postpo_framebuffer);	// draw to texture for postprocessing
	glClear(GL_COLOR_BUFFER_BIT);

	// 2. lighting pass: use G-buffer to calculate the scene's lighting
	glEnable(GL_BLEND); // enable alpha blending
	glBlendFunc(GL_ONE, GL_ONE);	// additive blending
	glDisable(GL_DEPTH_TEST);
	// set lighting uniforms
	glUseProgram(graphics.shader_program[sh_lighting_pass]);

	// bind buffer textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[0]);	// position
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[1]);	// normals
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[2]);	// albedo + specular
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, graphics.tex[t_white]);

	// no ambient light yet...
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LDirectional_count"), 0);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LPoint_count"), glm::min(light_count, 1u));
	glUniform3fv(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LAmbient"), 1, &vec3()[0]);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "light_intensity"), light_intensity);

	// render quad
	graphics.primitives[primitive_quad].bind();
	// accumulate light on output texture!
	if (do_scissor_test) {
		glEnable(GL_SCISSOR_TEST);
#pragma omp parallel for
		for (u32 i = 0; i < light_count; i++) {
			point_light[i].set_uniforms(Shader::sh_lighting_pass, &view_mtx, 0);
			// light optimization (reduce drawing viewport)
			light_scissor_optimization(point_light[i], min_attenuation, camera, show_scissor_test);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		glDisable(GL_SCISSOR_TEST);

	}
	else {
#pragma omp parallel for
		for (u32 i = 0; i < light_count; i++) {
			point_light[i].set_uniforms(Shader::sh_lighting_pass, &view_mtx, 0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

	}

	// directional light
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LDirectional_count"), 1);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LPoint_count"), 0);
	directional_light.set_uniforms(sh_lighting_pass, &view_mtx, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// accumulate ambient light ONCE!!!
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LDirectional_count"), 0);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LPoint_count"), 0);
	glUniform3fv(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LAmbient"), 1, &ambient_light[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pop_gl_errors("Light Pass");
#pragma endregion
	// 2.5 Copy content of geometry depth buffer to default framebuffer's depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postpo_framebuffer);
	glBlitFramebuffer(0, 0, WINDOW_X, WINDOW_Y, 0, 0, WINDOW_X, WINDOW_Y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	pop_gl_errors(__FUNCTION__);
}
void Demo_lod_terrain::forward_rendering(const mat4& proj_mtx, const mat4& view_mtx) const
{
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, postpo_framebuffer);
	// render lights (FORWARD RENDERING PASS)
	glUseProgram(graphics.shader_program[sh_color]);
	glBindVertexArray(mb_sphere.vao);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_color], "diffuse_texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, graphics.tex[t_white]);
	Color{ 0xffffffff }.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	mat4 m = proj_mtx * view_mtx;
	for (u32 i = 0; i < light_count; i++) {
		mat4 world_mtx = glm::translate(point_light[i].position) * glm::scale(vec3{ 0.1f });
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "M"), 1, false, &(m * world_mtx)[0][0]);
		point_light[i].diffuse.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
		// draw
		glDrawElements(GL_TRIANGLES, mb_sphere.index_count, GL_UNSIGNED_INT, 0);
	}


	pop_gl_errors(__FUNCTION__);
}
void Demo_lod_terrain::post_processing() const
{
	glDisable(GL_DEPTH_TEST);
	// POST-PROCESSING
	graphics.primitives[primitive_quad].bind();

	if (show_antialiasing) {
		// sobel pass
		glBindFramebuffer(GL_FRAMEBUFFER, aux_framebuffer[1]);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(graphics.shader_program[sh_sobel]);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, postpo_depth_buffer);
		glBindTexture(GL_TEXTURE_2D, postpo_depth_buffer);
		glUniform1f(glGetUniformLocation(graphics.shader_program[sh_sobel], "cam_far"), camera.far_plane);
		glUniform1f(glGetUniformLocation(graphics.shader_program[sh_sobel], "cam_near"), camera.near_plane);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_sobel], "M"), 1, false, &(mat4()[0][0]));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		if (current_showing_texture == 5) {
			// draw sobel
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glUseProgram(graphics.shader_program[sh_raw]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, aux_texture[1]);
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_raw], "M"), 1, false, &(mat4()[0][0]));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			throw current_showing_texture;
		}

		// antialiasing pass
		glBindFramebuffer(GL_FRAMEBUFFER, aux_framebuffer[0]);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(graphics.shader_program[sh_antialiasing]);
		glActiveTexture(GL_TEXTURE0);	// color tex
		glBindTexture(GL_TEXTURE_2D, postpo_texture);
		glActiveTexture(GL_TEXTURE1);	// sobel tex
		glBindTexture(GL_TEXTURE_2D, aux_texture[1]);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_antialiasing], "M"), 1, false, &(mat4()[0][0]));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	}

	if (show_bloom) {
		// HDR
		glBindFramebuffer(GL_FRAMEBUFFER, aux_framebuffer[2]);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(graphics.shader_program[sh_hdr]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postpo_texture);
		//glUniform1i(glGetUniformLocation(graphics.shader_program[sh_hdr], "radius"), blur_radius);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_hdr], "M"), 1, false, &(mat4()[0][0]));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		if (current_showing_texture == 6) {
			// draw hdr
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(graphics.shader_program[sh_raw]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, aux_texture[2]);
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_raw], "M"), 1, false, &(mat4()[0][0]));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			throw current_showing_texture;
		}
		// Blur pass
		glBindFramebuffer(GL_FRAMEBUFFER, aux_framebuffer[1]);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(graphics.shader_program[sh_linear_blur]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, aux_texture[2]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_linear_blur], "radius"), blur_radius);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_linear_blur], "M"), 1, false, &(mat4()[0][0]));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		if (current_showing_texture == 7) {
			// draw HDR + Blur
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(graphics.shader_program[sh_raw]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, aux_texture[1]);
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_raw], "M"), 1, false, &(mat4()[0][0]));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			throw current_showing_texture;
		}

		// bloom (color + blur_hdr)
		glBindFramebuffer(GL_FRAMEBUFFER, aux_framebuffer[2]);
		glClear(GL_COLOR_BUFFER_BIT);

		glBlendFunc(GL_ONE, GL_ONE);

		glUseProgram(graphics.shader_program[sh_bloom]);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_bloom], "M"), 1, false, &(mat4()[0][0]));
		glActiveTexture(GL_TEXTURE0);
		if (show_antialiasing)
			glBindTexture(GL_TEXTURE_2D, aux_texture[0]);	// scene color
		else
			glBindTexture(GL_TEXTURE_2D, postpo_texture);	// scene color
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindTexture(GL_TEXTURE_2D, aux_texture[1]);	// bloom color
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	}
	{
		// final result
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(graphics.shader_program[sh_raw]);
		glActiveTexture(GL_TEXTURE0);
		const GLuint final_texture = show_bloom ? aux_texture[2] : show_antialiasing ? aux_texture[0] : postpo_texture;
		glBindTexture(GL_TEXTURE_2D, final_texture);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_raw], "M"), 1, false, &(mat4()[0][0]));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	}
	glEnable(GL_DEPTH_TEST);


}
void Demo_lod_terrain::render_skybox(const mat4& proj_mtx, const mat4& view_mtx) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);

	glUseProgram(graphics.shader_program[sh_skybox]);
	glBindVertexArray(mb_cube.vao);
	// bind texture cubemap
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
	//vec3 rot_vec = glm::normalize(glm::cross(vec3{ camera_delta_pos.x, 0.f, camera_delta_pos.z }, vec3{ 0, 1, 0 }));
	//mat4 position_rot = glm::rotate(glm::length(vec2{ camera_delta_pos.x, camera_delta_pos.z }), rot_vec);
	glm::quat rot = glm::rotation(vec3{ 0, 0, 0 }, -directional_light.direction);
	//mat4 skybox_mtx = proj_mtx * mat4(mat3(view_mtx)) * glm::toMat4(rot);
	mat4 skybox_mtx = proj_mtx * mat4(mat3(view_mtx)) * glm::rotate(float(-M_PI / 2), vec3{ 0, 1, 0 })*glm::rotate(float(M_PI / 2), vec3{ 0, 0, 1 })* mat4(mat3(glm::lookAt(vec3{}, -directional_light.direction, vec3{ 0,0,1 })));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_skybox], "M"), 1, GL_FALSE, &skybox_mtx[0][0]);
	mb_cube.draw(GL_TRIANGLES);

	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);

	pop_gl_errors(__FUNCTION__);
}
void Demo_lod_terrain::draw_world_axis(const mat4& proj_mtx, const mat4& view_mtx) const
{

	//glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	const float LONGITUD = 5;
	const float SCALE = 0.1f;
	mat4 m = camera.ortho_proj() * glm::translate(vec3{ 0.45, -0.45 , -1 - camera.near_plane }) * glm::scale(vec3{ 0.1f }) * mat4(mat3(view_mtx));
	glUseProgram(graphics.shader_program[sh_color]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, graphics.tex[t_white]);


	glBindVertexArray(mb_cube.vao);
	// X axis
	Color{ 0xff0000ff }.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "M"), 1, false, &(m * glm::scale(vec3{ LONGITUD, 1, 1 } *SCALE) * glm::translate(vec3{ 0.5, 0, 0 }))[0][0]);
	glDrawElements(GL_TRIANGLES, mb_cube.index_count, GL_UNSIGNED_INT, 0);
	// Y axis
	Color{ 0x00ff00ff }.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "M"), 1, false, &(m * glm::scale(vec3{ 1, LONGITUD, 1 } *SCALE) * glm::translate(vec3{ 0, 0.5, 0 }))[0][0]);
	glDrawElements(GL_TRIANGLES, mb_cube.index_count, GL_UNSIGNED_INT, 0);
	// Z axis
	Color{ 0x0000ffff }.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "M"), 1, false, &(m * glm::scale(vec3{ 1, 1, LONGITUD } *SCALE) * glm::translate(vec3{ 0, 0, 0.5 }))[0][0]);
	glDrawElements(GL_TRIANGLES, mb_cube.index_count, GL_UNSIGNED_INT, 0);

	//glEnable(GL_DEPTH_TEST);

	pop_gl_errors(__FUNCTION__);

}

void Demo_lod_terrain::draw_camera_frustrum(const mat4& proj_mtx, const mat4& view_mtx, const Camera& target) const {

	mat4 m = proj_mtx * view_mtx;
	//m *= target.get_proj();
	m *= glm::inverse(target.get_view());
	//m *= glm::translate(vec3{ 0, 0, -1 }) * glm::scale(vec3{ 1 });

	glUseProgram(graphics.shader_program[sh_color]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, graphics.tex[t_white]);


	glBindVertexArray(mb_frustrum.vao);
	// X axis
	int uniform_loc = glGetUniformLocation(graphics.shader_program[sh_color], "color");
	Color{ 0xff0000ff }.set_uniform_RGBA(uniform_loc);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "M"), 1, false, &m[0][0]);
	glDisable(GL_CULL_FACE);
	glLineWidth(10.f);
	mb_frustrum.draw(GL_LINES);
	glLineWidth(1.f);
	Color{ 0xffffff88 }.set_uniform_RGBA(uniform_loc);
	mb_frustrum.draw(GL_TRIANGLES);
	glEnable(GL_CULL_FACE);
}
#pragma endregion