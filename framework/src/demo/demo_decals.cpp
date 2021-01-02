
/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: Demo_decals.cpp
Purpose: run assignment 3
Language: c++
Platform: windows 10
Project: cs562_markel.p_3
Author: Markel Pisano Berrojalbiz
Creation date: 9/18/2019
----------------------------------------------------------------------------------------------------------*/

#include <pch.h>

namespace {
	bool json_hack(const char *json_path, std::string &mesh_name, vec3 &pos, vec3& scale)
	{
		std::ifstream file(json_path);
		if (!file.is_open())
			return false;
		char buffer[256] = { 0 };
		while (!file.eof()) {
			file.getline(buffer, 256);
			std::stringstream line;
			line << buffer;
			//get first word
			std::string word;
			line >> word;

			if (word == "\"mesh\":") {
				line >> word;
				mesh_name = word.substr(1, word.size() - 3);
				mesh_name = "." + mesh_name;
			}
			else if (word == "\"translation\":") {
				file.getline(buffer, 256);	// {
				file.getline(buffer, 256);	// "x": 0.0,
				line.clear();
				line.str(buffer);
				line >> word;	// "x":
				line >> word;	// 0.0,
				pos.x = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "y": 0.0,
				line.clear();
				line.str(buffer);
				line >> word;	// "y":
				line >> word;	// 0.0,
				pos.y = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "z": 0.0,
				line.clear();
				line.str(buffer);
				line >> word;	// "z":
				line >> word;	// 0.0,
				pos.z = std::stof(word.substr(0, word.size() - 1));

			}
			else if (word == "\"scale\":") {
				file.getline(buffer, 256);	// {
				file.getline(buffer, 256);	// "x": 0.0,
				line.clear();
				line.str(buffer);
				line >> word;	// "x":
				line >> word;	// 0.0,
				scale.x = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "y": 0.0,
				line.clear();
				line.str(buffer);
				line >> word;	// "y":
				line >> word;	// 0.0,
				scale.y = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "z": 0.0,
				line.clear();
				line.str(buffer);
				line >> word;	// "z":
				line >> word;	// 0.0,
				scale.z = std::stof(word.substr(0, word.size()));

			}
		}
		return true;
	}
	bool json_hack_decals(const char *json_path, std::vector<std::string> &diffuse_textures, std::vector<std::string> &normal_textures,
		std::vector<vec3> &positions, std::vector<vec3>& rotations, std::vector<vec3> &scales)
	{
		std::ifstream file(json_path);
		if (!file.is_open())
			return false;
		char buffer[256] = { 0 };
		while (!file.eof()) {
			file.getline(buffer, 256);
			std::stringstream line;
			line << buffer;
			//get first word
			std::string word;
			line >> word;

			if (word == "\"diffuse\":") {
				line >> word;
				std::string tex_name = word.substr(1, word.size() - 3);
				tex_name = "." + tex_name;
				diffuse_textures.push_back(tex_name);
			}
			else if (word == "\"normal\":") {
				line >> word;
				std::string tex_name = word.substr(1, word.size() - 3);
				tex_name = "." + tex_name;
				normal_textures.push_back(tex_name);
			}
			else if (word == "\"translation\":") {
				file.getline(buffer, 256);	// {
				file.getline(buffer, 256);	// "x": 0.0,

				line.clear();
				line.str(buffer);
				line >> word;	// "x":
				line >> word;	// 0.0,
				vec3 pos;
				pos.x = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "y": 0.0,
				line.clear();
				line.str(buffer);
				line << buffer;
				line >> word;	// "y":
				line >> word;	// 0.0,
				pos.y = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "z": 0.0,

				line.clear();
				line.str(buffer);
				line >> word;	// "z":
				line >> word;	// 0.0,
				pos.z = std::stof(word.substr(0, word.size()));
				positions.push_back(pos);
			}
			else if (word == "\"rotate\":") {
				file.getline(buffer, 256);	// {
				file.getline(buffer, 256);	// "x": 0.0,

				line.clear();
				line.str(buffer);
				line >> word;	// "x":
				line >> word;	// 0.0,
				vec3 rot;
				rot.x = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "y": 0.0,

				// HACK: YZ swaped!!!

				line.clear();
				line.str(buffer);
				line >> word;	// "y":
				line >> word;	// 0.0,
				rot.z = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "z": 0.0,

				line.clear();
				line.str(buffer);
				line >> word;	// "z":
				line >> word;	// 0.0,
				rot.y = std::stof(word.substr(0, word.size()));
				rotations.push_back(rot);
			}
			else if (word == "\"scale\":") {
				file.getline(buffer, 256);	// {
				file.getline(buffer, 256);	// "x": 0.0,

				line.clear();
				line.str(buffer);
				line >> word;	// "x":
				line >> word;	// 0.0,
				vec3 scale;
				scale.x = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "y": 0.0,

				line.clear();
				line.str(buffer);
				line >> word;	// "y":
				line >> word;	// 0.0,
				scale.y = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "z": 0.0,

				line.clear();
				line.str(buffer);
				line >> word;	// "z":
				line >> word;	// 0.0,
				scale.z = std::stof(word.substr(0, word.size()));
				scales.push_back(scale);
			}
		}
		return true;
	}

}

void Demo_decals::update() {
	// FIRST PERSON CAMERA

#pragma region Camera Controller
	// rotate
	if (mouse.pressed(MOUSE_RIGHT)) {
		vec2 ndc_motion = vec2{ (float)mouse.getMove().x / graphics.window.size.x, (float)mouse.getMove().y / graphics.window.size.y };
		//float dotprod = glm::dot(ndc_motion, mouse_velocity);
		//if (dotprod < 0 || dotprod == 0.0f)
		//	mouse_velocity = ndc_motion;
		//else //if (glm::length2(ndc_motion) > glm::length2(orbit_velocity))
		//	mouse_velocity += ndc_motion;

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

#pragma endregion

#pragma region Place light bulb

	//intersect raycast with fixed plane
	if (mouse.triggered(MOUSE_LEFT)) {
		vec3 rayDir = glm::normalize(forward); /* taken from Camera Controller above */
					  //camera.target - camera.pos;
		float t = intersection_ray_plane(Ray{ camera.pos, rayDir }, Plane{ vec3{ 0, 1, 0 }, vec3{ 0, 5, 0 } });
		if (t > cEpsilon) {
			point_light[light_count++].position = camera.pos + rayDir * t;
		}
	}

#pragma endregion

	if (animate_scene) {
		float h = sin(animation_time * animation_speed);
		for (u32 i = 0; i < light_count; i++) {
			point_light[i].position.y = light_init_height[i] + h; // sin(animation_speed * dt * (frame_count % 360) * DEG_TO_RAD);
		}
		freq += h * 3 * dt;
		light_intensity = 3 - 3 * h;

		animation_time += dt;
	}

#pragma region IMGUI
	//IMGUI STUFF
	ImGui::Begin("Demo_decals");
	//Scene settings...
	ImGui::Text("CAMERA");
	ImGui::DragFloat("Near Plane", &camera.near_plane, 0.1f, 0.001f, 300);
	ImGui::DragFloat("Far Plane", &camera.far_plane, 0.1f, 0.001f, 300);
	ImGui::DragFloat("Zoom Speed", &zoom_speed, 0.1f, 0.f, 10.f);
	ImGui::DragFloat("Speed", &camera_axis_speed, 0.1f, 0.1f, 100.f);
	ImGui::DragFloat2("Rotation Speed", &mouse_velocity[0], 1.f, 1.f, 500.f);

	ImGui::Text("DECALS");
	ImGui::SliderFloat("Angle Limit", &angle_limit, 0.f, 90.f);
	ImGui::SliderInt("Render Mode", &decal_render_mode, 0, 2);
	ImGui::Checkbox("Show Decal Boxes", &show_decal_boxes);
	ImGui::Checkbox("Draw Decals", &draw_decals);

	std::string render_target_name;
	switch (current_showing_texture) {
	case -2:
		render_target_name = "Forward Shading";
		break;
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
	}
	ImGui::Text("RENDER OPTIONS");
	if (ImGui::Button("Recompile Shaders")) {
		graphics.delete_shaders();
		graphics.compile_shaders();
	}
	if (ImGui::BeginMenu(std::string("Render Target: " + render_target_name).c_str())) {
		if (ImGui::MenuItem("Forward Shading"))
			current_showing_texture = -2;
		else if (ImGui::MenuItem("Deferred Shading"))
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
	ImGui::Checkbox("Antialiasing", &show_antialiasing);
	ImGui::Checkbox("Bloom", &show_bloom);

	ImGui::DragInt("Blur Radius", reinterpret_cast<int*>(&blur_radius), 1, 0, 50);


	//Lights
	ImGui::Text("LIGHTS");
	ImGui::DragInt("Light count", reinterpret_cast<int*>(&light_count), 1.f, 0, MAX_LIGHT_COUNT);
	ImGui::ColorEdit3("Ambient", &ambient_light[0]);
	ImGui::DragFloat("Intensity", &light_intensity, 0.1f, 0.1f, 100.f);
	ImGui::Checkbox("Animate", &animate_scene);
	ImGui::DragFloat("Anim Speed", &animation_speed);
	ImGui::DragFloat("Min Att", &min_attenuation, 0.001f, 0.f, 10.f);
	ImGui::Checkbox("Scissor Opt", &do_scissor_test);
	ImGui::SameLine();
	ImGui::Checkbox("Show", &show_scissor_test);
	// skybox
	ImGui::Text("SKYBOX");
	ImGui::DragFloat("Freq", &freq, 0.1f, 0.001f, 100.f);
	ImGui::DragFloat("Lacunarity", &lacunarity, 0.1f, 0.001f, 100.f);
	ImGui::DragFloat("Persistance", &persistance, 0.1f, 0.001f, 100.f);
	ImGui::DragInt("Octaves", &oct, 1, 0, 6);
	ImGui::Text("Colors");
	if (ImGui::Button("Add")) {
		skybox_colors.push_back(vec4{ 0, 0, 0, 1 });
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove")) {
		skybox_colors.pop_back();
	}
	for (u32 i = 0; i < skybox_colors.size(); i++) {
		vec4 &c = skybox_colors[i];
		ImGui::ColorEdit4(std::to_string(i).c_str(), &c[0]);
	}


	ImGui::End();
#pragma endregion

}
void Demo_decals::render() const {

	// camera matrix
	mat4 view_mtx = camera.get_view();
	mat4 proj_mtx = camera.get_proj();

	try {

		if (current_showing_texture == -2) {
			// FORWARD SHADING
			forward_shading(proj_mtx, view_mtx);
		}
		else {
			deferred_shading(proj_mtx, view_mtx);
		}

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
			graphics.primitives[Primitive::primitive_quad].bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, postpo_depth_buffer);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glEnable(GL_DEPTH_TEST);
			pop_gl_errors("G-Buffer texture rendering");
			throw current_showing_texture;
		}
		if (show_decal_boxes) {
			glBindVertexArray(mb_cube.vao);
			glBindFramebuffer(GL_FRAMEBUFFER, postpo_framebuffer);
			// draw the wireframe
			glUseProgram(graphics.shader_program[sh_color]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, graphics.tex[t_white]);
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "PROJ"), 1, GL_FALSE, &(proj_mtx[0][0]));
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "VIEW"), 1, GL_FALSE, &(view_mtx[0][0]));
			Color{}.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
			for (int i = 0; i < decals_transform.size(); i++) {
				const mat4 &world_mtx = decals_transform[i];
				glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "WORLD"), 1, GL_FALSE, &(world_mtx[0][0]));
				glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MODEL"), 1, GL_FALSE, &(mat4()[0][0]));
				// draw wireframe
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				glDrawElements(GL_TRIANGLES, mb_cube.index_count, GL_UNSIGNED_INT, 0);
				glEnable(GL_CULL_FACE);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

#pragma endregion

		post_processing();

		// SKYBOX
		glBindFramebuffer(GL_READ_FRAMEBUFFER, postpo_framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, WINDOW_X, WINDOW_Y, 0, 0, WINDOW_X, WINDOW_Y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		render_skybox(proj_mtx, view_mtx);

	}
	catch (...) {}

	pop_gl_errors(__FUNCTION__);
}

Demo_decals::Demo_decals() {

	// create G-buffer
	create_g_buffer(g_buffer, g_buffer_textures);
	// create framebuffer for postprocessing
	create_hdr_framebuffer(postpo_framebuffer, postpo_texture, &postpo_depth_buffer);
	for (u32 i = 0; i < AUX_BUFF_COUNT; i++)
		create_framebuffer(aux_framebuffer[i], aux_texture[i]);
	//create decals frambuffer
	create_decal_framebuffer(decal_framebuffer, g_buffer_textures[2], g_buffer_textures[1]);

	//skybox params
	skybox_colors.push_back(vec4{ 1, 0, 0, 0 });
	skybox_colors.push_back(vec4{ 1, 1, 0, 98.f / 255 });
	skybox_colors.push_back(vec4{ 1, 0, 1, 100.f / 255 });
	skybox_colors.push_back(vec4{ 1, 1, 1, 110.f / 255 });
	skybox_colors.push_back(vec4{ 65, 183, 16, 119 } / 255.f);
	skybox_colors.push_back(vec4{ 75, 90, 243, 122 } / 255.f);

	freq = 11.f;
	lacunarity = 1.6f;
	persistance = 1.2f;
	oct = 4;

	// camera setting
	camera.pos = { 20.f, 20.f, 0.f };
	camera.target = camera.pos - glm::normalize(camera.pos);

	// scene hack
	std::string model_path;
	json_hack("../resources/scenes/scene.json", model_path, sponza_pos, sponza_scale);

	// decals hack!
	std::vector<std::string> decals_dif, decals_norm;
	std::vector<vec3> decals_pos, decals_rot, decals_sc;
	json_hack_decals("../resources/scenes/sceneDecals.json", decals_dif, decals_norm, decals_pos, decals_rot, decals_sc);
	//compute transform matrices
	for (int i = 0; i < decals_pos.size(); i++) {
		vec3 radianAngles = glm::radians(decals_rot[i]);
		mat4 m = glm::translate(decals_pos[i]) * glm::toMat4(glm::quat(radianAngles)) * glm::scale(decals_sc[i]);
		decals_transform.push_back(m);
	}
	// load textures
	for (const std::string& path : decals_dif) {
		GLuint tex = 0;
		load_texture(&tex, path.c_str(), false);
		decals_diffuse.push_back(tex);
	}
	for (const std::string& path : decals_norm) {
		GLuint tex = 0;
		load_texture(&tex, path.c_str(), false);
		decals_normal.push_back(tex);
	}

	// mesh loading
	create_quad_mesh(&mb_quad, nullptr);
	create_cube_mesh(&mb_cube, nullptr);
	create_sphere_mesh(&mb_sphere, nullptr);
	load_mesh_obj(model_path.c_str(), &model_sponza);

	load_color_texture(&graphics.tex[Texture::t_white], Color{0xffffffff});

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

}
Demo_decals::~Demo_decals() {
	// delete G-buffer
	glDeleteTextures(sizeof(g_buffer_textures) / sizeof(GLuint), g_buffer_textures);
	glDeleteFramebuffers(1, &g_buffer);

	glDeleteTextures(1, &postpo_texture);
	glDeleteRenderbuffers(1, &postpo_depth_buffer);
	glDeleteFramebuffers(1, &postpo_framebuffer);

	glDeleteTextures(AUX_BUFF_COUNT, aux_texture);
	glDeleteFramebuffers(AUX_BUFF_COUNT, aux_framebuffer);

	glDeleteTextures(decals_diffuse.size(), decals_diffuse.data());
	glDeleteTextures(decals_normal.size(), decals_normal.data());
}

#pragma region RENDERING
void Demo_decals::deferred_shading(const mat4& proj_mtx, const mat4& view_mtx) const
{
#pragma region GEOMETRY PASS
	// 1. geometry pass: render all geometric/color data to G-buffer
	glDisable(GL_BLEND);
	//glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(graphics.shader_program[sh_geometry_pass]);

	// common matrices
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_geometry_pass], "VIEW"), 1, false, &(view_mtx[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_geometry_pass], "PROJ"), 1, false, &(proj_mtx[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_geometry_pass], "WORLD"), 1, false, &(glm::scale(sponza_scale)[0][0]));

	// draw model
	model_sponza.draw(sh_geometry_pass);

	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_geometry_pass], "MODEL"), 1, false, &(glm::scale(vec3{ 10.f })[0][0]));
	glBindVertexArray(mb_sphere.vao);
	glDrawElements(GL_TRIANGLES, mb_sphere.index_count, GL_UNSIGNED_INT, 0);


	pop_gl_errors("G-Buffer rendering");
#pragma endregion
#pragma region DECALS
	if (draw_decals) {
		glBindVertexArray(mb_cube.vao);
		glBindFramebuffer(GL_FRAMEBUFFER, decal_framebuffer);
		glDisable(GL_CULL_FACE);

		// draw decals onto gBuffer
		glUseProgram(graphics.shader_program[sh_decal]);

		// bind position texture
		glActiveTexture(GL_TEXTURE2);	// gBuffer position
		glBindTexture(GL_TEXTURE_2D, g_buffer_textures[MAX_G_BUFFER_CHANNELS - 1]);

		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_decal], "texture_w"), WINDOW_X);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_decal], "texture_h"), WINDOW_Y);
		glUniform1f(glGetUniformLocation(graphics.shader_program[sh_decal], "cos_angle_limit"), glm::cos(glm::radians(angle_limit)));
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_decal], "render_mode"), decal_render_mode);
		// matrices
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_decal], "INV_PROJ"), 1, GL_FALSE, &(glm::inverse(proj_mtx)[0][0]));

		for (int i = 0; i < decals_transform.size(); i++) {
			// bind decal textures
			glActiveTexture(GL_TEXTURE0);	// diffuse
			glBindTexture(GL_TEXTURE_2D, decals_diffuse[i]);
			glActiveTexture(GL_TEXTURE1);	// normal map
			glBindTexture(GL_TEXTURE_2D, decals_normal[i]);

			const mat4 &world_mtx = decals_transform[i];
			const mat4 MV = view_mtx * world_mtx;
			const mat4 MVP = proj_mtx * MV;
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_decal], "MVP"), 1, GL_FALSE, &(MVP[0][0]));
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_decal], "MV"), 1, GL_FALSE, &(MV[0][0]));
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_decal], "INV_MV"), 1, GL_FALSE, &(glm::inverse(MV)[0][0]));


			glDrawElements(GL_TRIANGLES, mb_cube.index_count, GL_UNSIGNED_INT, 0);
		}
		glEnable(GL_CULL_FACE);

	}
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
	// 2. lighting pass: use G-buffer to calculate the scene's lighting
	glEnable(GL_BLEND); // enable alpha blending
	glBlendFunc(GL_ONE, GL_ONE);	// additive blending
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, postpo_framebuffer);	// draw to texture for postprocessing
	glClear(GL_COLOR_BUFFER_BIT);
	// bind buffer textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[0]);	// position
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[1]);	// normals
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[2]);	// albedo + specular

	// set lighting uniforms
	glUseProgram(graphics.shader_program[sh_lighting_pass]);
	// no ambient light yet...
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

	// accumulate ambient light ONCE!!!
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LPoint_count"), 0);
	glUniform3fv(glGetUniformLocation(graphics.shader_program[sh_lighting_pass], "LAmbient"), 1, &ambient_light[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#pragma endregion
	// 2.5 Copy content of geometry depth buffer to default framebuffer's depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postpo_framebuffer);
	glBlitFramebuffer(0, 0, WINDOW_X, WINDOW_Y, 0, 0, WINDOW_X, WINDOW_Y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

}
void Demo_decals::forward_shading(const mat4& proj_mtx, const mat4& view_mtx) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, postpo_framebuffer);	// draw to texture for postprocessing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(graphics.shader_program[sh_forward_light]);

	// common matrices
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_forward_light], "VIEW"), 1, false, &(view_mtx[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_forward_light], "PROJ"), 1, false, &(proj_mtx[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_forward_light], "WORLD"), 1, false, &(glm::scale(vec3{ 0.1f })[0][0]));
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_forward_light], "light_intensity"), light_intensity);

#if 1
	glUniform3fv(glGetUniformLocation(graphics.shader_program[sh_forward_light], "LAmbient"), 1, &ambient_light[0]);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_forward_light], "LDirectional_count"), 0);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_forward_light], "LPoint_count"), glm::min(light_count, 32u));
	for (u32 j = 0; j < glm::min(light_count, 32u); j++) {
		point_light[j].set_uniforms(Shader::sh_forward_light, &view_mtx, j);
	}
#else
	const u32 iter_count = light_count / 32;
	const u32 last_iter_light_count = light_count % 32;
	// bind lights
	glUniform3fv(glGetUniformLocation(graphics.shader_program[sh_light], "LAmbient"), 1, &ambient_light[0]);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_light], "LDirectional_count"), 0);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_light], "LPoint_count"), 32);
	for (u32 i = 0; i < iter_count; i++) {
#pragma omp parallel for
		for (u32 j = 0; j < 32; j++) {
			point_light[i * 32 + j].set_uniforms(Shader::sh_light, &view_mtx, j);
		}


		model_sponza.draw(Shader::sh_light);
	}
	// last iteration
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_light], "LPoint_count"), last_iter_light_count);
	for (u32 j = 0; j < last_iter_light_count; j++) {
		point_light[light_count - last_iter_light_count + j].set_uniforms(Shader::sh_light, &view_mtx, j);
	}

#endif
	model_sponza.draw(Shader::sh_forward_light);

	// copy depth buffer
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, postpo_framebuffer);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_buffer);
	//glBlitFramebuffer(0, 0, WINDOW_X, WINDOW_Y, 0, 0, WINDOW_X, WINDOW_Y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	pop_gl_errors(__FUNCTION__);
}
void Demo_decals::forward_rendering(const mat4& proj_mtx, const mat4& view_mtx) const
{
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, postpo_framebuffer);
	// render lights (FORWARD RENDERING PASS)
	glUseProgram(graphics.shader_program[sh_color]);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "VIEW"), 1, false, &(view_mtx[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "PROJ"), 1, false, &(proj_mtx[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MODEL"), 1, false, &(mat4()[0][0]));
	glBindVertexArray(mb_sphere.vao);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_color], "diffuse_texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, graphics.tex[t_white]);
	Color{ 0xffffffff }.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	for (u32 i = 0; i < light_count; i++) {
		mat4 world_mtx = glm::translate(point_light[i].position) * glm::scale(vec3{ 0.1f });
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "WORLD"), 1, false, &(world_mtx[0][0]));
		point_light[i].diffuse.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
		// draw
		glDrawElements(GL_TRIANGLES, mb_sphere.index_count, GL_UNSIGNED_INT, 0);
	}
	pop_gl_errors(__FUNCTION__);
}
void Demo_decals::post_processing() const
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

		glUseProgram(graphics.shader_program[sh_raw]);
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
void Demo_decals::render_skybox(const mat4& proj_mtx, const mat4& view_mtx) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);

	glUseProgram(graphics.shader_program[sh_skybox]);
	glBindVertexArray(mb_cube.vao);
	//pass uniforms
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_skybox], "freq"), freq);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_skybox], "lacunarity"), lacunarity);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_skybox], "persistance"), persistance);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_skybox], "oct"), oct);
	glUniform3fv(glGetUniformLocation(graphics.shader_program[sh_skybox], "offset"), 1, &noise_offset[0]);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_skybox], "LayerCount"), skybox_colors.size());
	for (u32 i = 0; i < skybox_colors.size(); i++) {
		glUniform4fv(
			glGetUniformLocation(graphics.shader_program[sh_skybox], std::string("Layers[" + std::to_string(i) + "]").c_str())
			, 1, &(skybox_colors[i][0]));
	}
	mat4 skybox_mtx = proj_mtx * mat4(mat3(view_mtx));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_skybox], "M"), 1, GL_FALSE, &skybox_mtx[0][0]);
	glDrawElements(GL_TRIANGLES, mb_cube.index_count, GL_UNSIGNED_INT, 0);

	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);

	pop_gl_errors(__FUNCTION__);
}
#pragma endregion