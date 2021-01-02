/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: Demo_tesselation.cpp
Purpose: run assignment 2
Language: c++
Platform: windows 10
Project: cs562_markel.p_2
Author: Markel Pisano Berrojalbiz
Creation date: 10/24/2019
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
				line << buffer;
				line >> word;	// "x":
				line >> word;	// 0.0,
				pos.x = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "y": 0.0,
				line << buffer;
				line >> word;	// "y":
				line >> word;	// 0.0,
				pos.y = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "z": 0.0,
				line << buffer;
				line >> word;	// "z":
				line >> word;	// 0.0,
				pos.z = std::stof(word.substr(0, word.size() - 1));

			}
			else if (word == "\"scale\":") {
				file.getline(buffer, 256);	// {
				file.getline(buffer, 256);	// "x": 0.0,
				line << buffer;
				line >> word;	// "x":
				line >> word;	// 0.0,
				scale.x = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "y": 0.0,
				line << buffer;
				line >> word;	// "y":
				line >> word;	// 0.0,
				scale.y = std::stof(word.substr(0, word.size() - 1));
				file.getline(buffer, 256);	// "z": 0.0,
				line << buffer;
				line >> word;	// "z":
				line >> word;	// 0.0,
				scale.z = std::stof(word.substr(0, word.size() - 1));

			}
		}
		return true;
	}
}

void Demo_tesselation::update() {
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
		camera.pos += forward * camera_axis_speed * dt;
	}
	else if (keyboard.pressed(GLFW_KEY_S)) {
		camera.pos -= forward * camera_axis_speed * dt;
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

	// light animation
	//point_light[0].position = glm::rotate(animation_speed * dt, vec3{ 0, 1, 0 }) * vec4 { 10, 10, 0, 1 };

#pragma region IMGUI
	//IMGUI STUFF
	ImGui::Begin("Demo_tesselation");
	//Scene settings...
	ImGui::Text("CAMERA");
	ImGui::DragFloat("Near Plane", &camera.near_plane, 0.1f, 0.001f, 300);
	ImGui::DragFloat("Far Plane", &camera.far_plane, 0.1f, 0.001f, 300);
	ImGui::DragFloat("Zoom Speed", &zoom_speed, 0.1f, 0.f, 10.f);
	ImGui::DragFloat("Speed", &camera_axis_speed, 0.1f, 0.1f, 100.f);
	ImGui::DragFloat2("Rotation Speed", &mouse_velocity[0], 1.f, 1.f, 500.f);

	// RENDER MODES
	ImGui::Text("TESSELLATION");
	ImGui::Checkbox("Wireframe", &show_wireframe);
	ImGui::SameLine();
	ImGui::Checkbox("Normals", &show_normals);
	ImGui::Checkbox("Adaptive", &adaptive_tessellation);
	if (adaptive_tessellation) {
		ImGui::SameLine();
		ImGui::Checkbox("AdaptiveColor", &adaptive_color);
	}
	else {
		ImGui::DragFloat("TessLevels", &tess_levels, 0.01f, 0.f, 10.f);
	}
	ImGui::SliderFloat("TessAlpha", &tess_alpha, 0.f, 1.f);

	//Lights
	ImGui::Text("LIGHTS");
	ImGui::DragInt("Light count", reinterpret_cast<int*>(&light_count), 1.f, 0, MAX_LIGHT_COUNT);
	ImGui::ColorEdit3("Ambient", &ambient_light[0]);
	ImGui::DragFloat("Intensity", &light_intensity, 0.1f, 0.1f, 100.f);
	ImGui::DragFloat("Min Att", &min_attenuation, 0.001f, 0.f, 10.f);
	ImGui::Checkbox("Scissor Opt", &do_scissor_test);
	ImGui::SameLine();
	ImGui::Checkbox("Show", &show_scissor_test);


	ImGui::End();
#pragma endregion

}
void Demo_tesselation::render() const {

	// camera matrix
	mat4 view_mtx = camera.get_view();
	mat4 proj_mtx = camera.get_proj();

#pragma region GEOMETRY PASS

	// 1. geometry pass: render all geometric/color data to G-buffer
	glDisable(GL_BLEND);
	//glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(graphics.shader_program[sh_phong_tess]);


	// common matrices
	mat4 MV = view_mtx * glm::scale(vec3{ 10.1f });
	mat4 MVP = proj_mtx * MV;
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "MV"), 1, false, &(MV[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "MVP"), 1, false, &(MVP[0][0]));
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "light_intensity"), light_intensity);

	//tessselation stuff
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "uTessLevels"), tess_levels);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "uTessAlpha"), tess_alpha);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "adaptive"), (int)adaptive_tessellation);
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "adaptiveColor"), (int)adaptive_color);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "cam_far"), camera.far_plane);
	glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess], "cam_near"), camera.near_plane);

	material.set_uniforms(sh_phong_tess);

	if (show_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	glBindVertexArray(mb_suzanne.vao);
	//glDrawElements(GL_TRIANGLES, mb_suzanne.index_count, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_PATCHES, mb_suzanne.index_count, GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

#if 0 // TODO: FIX shader!
	if (show_normals) {
		glBindFramebuffer(GL_FRAMEBUFFER, postpo_framebuffer);
		glUseProgram(graphics.shader_program[sh_phong_tess_normals]);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "MV"), 1, false, &(MV[0][0]));
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "MVP"), 1, false, &(MVP[0][0]));

		//tessselation stuff
		glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "uTessLevels"), tess_levels);
		glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "uTessAlpha"), tess_alpha);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "adaptive"), (int)adaptive_tessellation);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "adaptiveColor"), (int)adaptive_color);
		glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "cam_far"), camera.far);
		glUniform1f(glGetUniformLocation(graphics.shader_program[sh_phong_tess_normals], "cam_near"), camera.near);

		glPatchParameteri(GL_PATCH_VERTICES, 3);

		glBindVertexArray(mb_suzanne.vao);

		glDisable(GL_DEPTH_TEST);
		//glDrawElements(GL_TRIANGLES, mb_suzanne.index_count, GL_UNSIGNED_INT, 0);
		glDrawElements(GL_PATCHES, mb_suzanne.index_count, GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
	}
#endif
	// draw light
#if 1
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
#endif

	// postprocess
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(graphics.shader_program[sh_raw]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postpo_texture);
	graphics.primitives[primitive_quad].bind();
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_raw], "M"), 1, false, &(mat4()[0][0]));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	pop_gl_errors(__FUNCTION__);
}

Demo_tesselation::Demo_tesselation() {

	// create G-buffer
	create_g_buffer(g_buffer, g_buffer_textures);
	// create framebuffer for postprocessing
	create_hdr_framebuffer(postpo_framebuffer, postpo_texture, &postpo_depth_buffer);

	// camera setting
	camera.pos = { 20.f, 20.f, 0.f };
	camera.target = camera.pos - glm::normalize(camera.pos);

	// light setting
	point_light[0].position = { 10, 10, 0 };
	for (u32 i = 1; i < MAX_LIGHT_COUNT; i++) {
		point_light[i].position = vec3{ glm::linearRand(-10, 10), glm::linearRand(-10, 10), glm::linearRand(-10, 10) };
		point_light[i].diffuse = Color{ (u8)glm::linearRand(0, 255), (u8)glm::linearRand(0, 255) , (u8)glm::linearRand(0, 255) , (u8)255 };
		point_light[i].attenuation = { 1, 1, 1 };
	}
	light_count = 1;
	// scene hack
	std::string model_path;
	vec3 tmp_pos, tmp_scale;
	json_hack("../resources/scenes/suzanne.json", model_path, tmp_pos, tmp_scale);

	// mesh loading
	create_sphere_mesh(&mb_sphere, nullptr);
	load_mesh_obj(model_path.c_str(), &mb_suzanne);

	load_color_texture(&graphics.tex[Texture::t_white], Color{0xffffffff});

}
Demo_tesselation::~Demo_tesselation() {
	// delete G-buffer
	glDeleteTextures(sizeof(g_buffer_textures) / sizeof(GLuint), g_buffer_textures);
	glDeleteFramebuffers(1, &g_buffer);

	glDeleteTextures(1, &postpo_texture);
	glDeleteRenderbuffers(1, &postpo_depth_buffer);
	glDeleteFramebuffers(1, &postpo_framebuffer);

}
