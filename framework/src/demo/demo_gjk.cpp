/**
* @file demo_gjk.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/04/20	(eus)
* @brief Implement demo
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>

namespace {
	std::vector<vec3> mul_vertices(const mat4 &m, const std::vector<vec3> &points)
	{
		std::vector<vec3> result(points.size(), vec3{});
		for (int i = 0; i < result.size(); ++i) {
			result[i] = m * vec4{ points[i], 1.f };
		}
		return result;
	}
}

Demo_gjk::Demo_gjk()
{
	// DO NOT LOAD CONCAVE OF NON 3D!

	load_mesh_obj("../resources/meshes/sphere.obj", &mesh_buffer[mesh_sphere], &mesh_data[mesh_sphere]);
	load_mesh_obj("../resources/meshes/octohedron.obj", &mesh_buffer[mesh_octohedron], &mesh_data[mesh_octohedron]);
	load_mesh_obj("../resources/meshes/icosahedron.obj", &mesh_buffer[mesh_icosahedron], &mesh_data[mesh_icosahedron]);
	load_mesh_obj("../resources/meshes/cylinder.obj", &mesh_buffer[mesh_cylinder], &mesh_data[mesh_cylinder]);
	load_mesh_obj("../resources/meshes/cube.obj", &mesh_buffer[mesh_cube], &mesh_data[mesh_cube]);

	// setup camera
	camera.pos = { 0, 0, 10 };
	camera.target = -glm::normalize(camera.pos) + camera.pos;
	camera_axis_speed = 20.f;

	// set testing obj0 and obj1
	obj0.set_mesh_buffers(&mesh_buffer[mesh_cube]);
	obj0.set_mesh_data(&mesh_data[mesh_cube]);
	obj0.set_pos(vec3{ -1.6,0,0 });

	obj1.set_mesh_buffers(&mesh_buffer[mesh_octohedron]);
	obj1.set_mesh_data(&mesh_data[mesh_octohedron]);
	obj1.set_pos(vec3{ 0,0,0 });

	gjk3d(mul_vertices(obj0.get_model(), obj0.get_mesh_data()->positions), mul_vertices(obj1.get_model(), obj1.get_mesh_data()->positions), m_simplices);
}

Demo_gjk::~Demo_gjk()
{

}

/**
*
* @brief update
*/
void Demo_gjk::update()
{

	// update camera
	{
		// rotate
		if (mouse.pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
			vec2 ndc_motion = vec2{ mouse.getMove().x / camera.size.x, mouse.getMove().y / camera.size.y };

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

		if (keyboard.pressed('W')) {
			camera.pos += glm::normalize(glm::cross(right, vec3{ 0,1,0 })) *camera_axis_speed * dt;
		}
		else if (keyboard.pressed('S')) {
			camera.pos -= glm::normalize(glm::cross(right, vec3{ 0,1,0 })) * camera_axis_speed * dt;
		}
		if (keyboard.pressed('A')) {
			camera.pos += right * camera_axis_speed * dt;
		}
		if (keyboard.pressed('D')) {
			camera.pos -= right * camera_axis_speed * dt;
		}
		if (keyboard.pressed('Q')) {
			camera.pos -= vec3{ 0, 1, 0 } *camera_axis_speed * dt;
		}
		if (keyboard.pressed('E')) {
			camera.pos += vec3{ 0, 1, 0 } *camera_axis_speed * dt;
		}
		camera.target = camera.pos + forward;
	}

	// ImGui
	{
		ImGui::Begin("CS350");

		// WINDOW
		if (ImGui::CollapsingHeader("WINDOW")) {
			std::string fps_str = "FPS: " + std::to_string(frc.frame_rate);
			ImGui::TextColored(frc.fps > (u32)glm::round(frc.frame_rate) ? ImVec4(1, 0, 0, 1) : ImVec4(0, 1, 0, 1), fps_str.c_str());
			if (ImGui::Button("Toggle Fullscreen")) {
				graphics.window.toggle_fullscreen();
				camera.size = graphics.window.size;
			}
			ImGui::SameLine();
			ImGui::Text(graphics.window.is_fullscreen ? "ON" : "OFF");
			if (ImGui::DragInt2("Window Size", &graphics.window.size[0])) {
				graphics.window.resize(graphics.window.size.x, graphics.window.size.y);
				camera.size = graphics.window.size;
			}
			if (ImGui::DragInt2("Window Pos", &graphics.window.position[0])) {
				glfwSetWindowPos(graphics.window.glfwWindow, graphics.window.position.x, graphics.window.position.y);
			}
		}
		// CAMERA
		if (ImGui::CollapsingHeader("CAMERA")) {
			Camera * edit_cam = &camera;
			bool must_update_frustum = false;
			ImGui::Text("X %f Y %f Z %f", edit_cam->pos.x, edit_cam->pos.y, edit_cam->pos.z);
			float angle = glm::degrees(edit_cam->fov);
			if (ImGui::DragFloat("FoV", &angle, 1.f, 1.f, 180.f))
				must_update_frustum = true;
			edit_cam->fov = glm::radians(angle);
			if (ImGui::DragFloat("Near", &edit_cam->near_plane, 0.01f, 0.0001f, 10.f) ||
				ImGui::DragFloat("Far", &edit_cam->far_plane, 1.f, 10.f, 100000.f) ||
				ImGui::DragFloat2("Size", &edit_cam->size[0])) {
				must_update_frustum = true;
			}
			// camera control
			ImGui::DragFloat2("MouseVel", &mouse_velocity[0]);
			ImGui::DragFloat("AxisSpeed", &camera_axis_speed);

		}

		// GJK
		if (ImGui::CollapsingHeader("GJK")) {
			vec3 pos, sc, rot;
			ImGui::Text("Object A");
			pos = obj0.get_pos(); sc = obj0.get_scale(); rot = obj0.get_rot();
			if (ImGui::DragFloat3("Position A", &pos[0], 0.1f, -100.f, 100.f))
				obj0.set_pos(pos);
			if (ImGui::DragFloat3("Scale A", &sc[0], 0.01f, 0.01f, 100.f))
				obj0.set_scale(sc);
			if (ImGui::DragFloat3("Rotation A", &rot[0], 0.1f, -100.f, 100.f))
				obj0.set_rot(rot);

			ImGui::Text("Object B");
			pos = obj1.get_pos(); sc = obj1.get_scale(); rot = obj1.get_rot();
			if (ImGui::DragFloat3("Position B", &pos[0], 0.1f, -100.f, 100.f))
				obj1.set_pos(pos);
			if (ImGui::DragFloat3("Scale B", &sc[0], 0.01f, 0.01f, 100.f))
				obj1.set_scale(sc);
			if (ImGui::DragFloat3("Rotation B", &rot[0], 0.1f, -100.f, 100.f))
				obj1.set_rot(rot);

			ImGui::Separator();
			ImGui::Checkbox("Draw simplex", &draw_simplex);
			ImGui::SliderInt("Steps", &m_steps, 0, (int)m_simplices.size() - 1);
			ImGui::Separator();
			ImGui::Checkbox("Minkowski", &draw_minkowski);
			ImGui::Separator();
			ImGui::Text("Huls colliding:");
			ImGui::SameLine();
			if (hulls_colliding)
				ImGui::TextColored(ImVec4{ 0,1,0,1 }, "TRUE");
			else
				ImGui::TextColored(ImVec4{ 1,0,0,1 }, "FALSE");
			
			ImGui::Separator();
			ImGui::Checkbox("Rotate Randomly", &rotate_randomly);
		}

		ImGui::End();
	}

	// rotate randomly
	if (rotate_randomly) {
		obj0.set_rot(obj0.get_rot() + rand_rot_0 * dt);
		obj1.set_rot(obj1.get_rot() + rand_rot_1 * dt);
		rot_change_timer += dt;
		if (rot_change_timer > 1.f) {
			rot_change_timer = 0.f;
			rand_rot_0 = vec3{ glm::linearRand(-M_PI, M_PI),glm::linearRand(-M_PI, M_PI),glm::linearRand(-M_PI, M_PI) };
			rand_rot_1 = vec3{ glm::linearRand(-M_PI, M_PI), glm::linearRand(-M_PI, M_PI), glm::linearRand(-M_PI, M_PI)};
		}
	}

	// compute Minkowski difference
	{
		minkowski_points.clear();
		const auto& A = obj0.get_mesh_data()->positions;
		std::vector<vec3> pointsA; pointsA.reserve(A.size());
		const auto& B = obj1.get_mesh_data()->positions;
		std::vector<vec3> pointsB; pointsB.reserve(B.size());
		const mat4 m0 = obj0.get_model();
		const mat4 m1 = obj1.get_model();
		for (int i = 0; i < A.size(); ++i)
			pointsA.push_back(m0 * vec4{ A[i],1.f });
		for (int i = 0; i < B.size(); ++i)
			pointsB.push_back(m1 * vec4{ B[i],1.f });
		for (int i = 0; i < pointsA.size(); ++i) {
			for (int j = 0; j < pointsB.size(); ++j) {
				minkowski_points.push_back(pointsA[i] - pointsB[j]);
			}
		}
	}
	// GJK algorithm
	{
		m_simplices.clear();
		hulls_colliding = gjk3d(mul_vertices(obj0.get_model(), obj0.get_mesh_data()->positions), mul_vertices(obj1.get_model(), obj1.get_mesh_data()->positions), m_simplices);
	}
}

/**
*
* @brief render
*/
void Demo_gjk::render() const
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	//glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// compute camera matrices
	mat4 proj = camera.get_proj();
	mat4 view = camera.get_view();
	const mat4 vp = proj * view;

	glUseProgram(graphics.shader_program[sh_color]);

	// DRAW MAIN MODELs
	int uniform_color = glGetUniformLocation(graphics.shader_program[sh_color], "color");
	int uniform_mvp = glGetUniformLocation(graphics.shader_program[sh_color], "MVP");

	pop_gl_errors("Setup Render");

	// draw origin
	glBindVertexArray(graphics.primitives[primitive_cube].vao);
	glUniformMatrix4fv(uniform_mvp, 1, false, &(vp * glm::scale(vec3{0.1f}))[0][0]);
	Color{ 0xff0000ff }.set_uniform_RGBA(uniform_color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	graphics.primitives[primitive_cube].draw();

	// draw Minkowski
	if (draw_minkowski) {
		Color{ 0x00ff00ff }.set_uniform_RGBA(uniform_color);
		const mat4 mtx_sc = glm::scale(vec3{ 0.1f });
		for (int i = 0; i < minkowski_points.size(); ++i) {
			glUniformMatrix4fv(uniform_mvp, 1, false, &(vp * glm::translate(minkowski_points[i]) * mtx_sc)[0][0]);
			graphics.primitives[primitive_cube].draw();
		}
	}

	// draw simplex
	if (draw_simplex && !m_simplices.empty()) {
		Color color = Color{ 0xffff00ff };
		color.set_uniform_RGBA(uniform_color);
		mat4 mtx_sc = glm::scale(vec3{ 0.1f });
		const gjk_simplex simplex = m_simplices[glm::min(m_steps, (int)m_simplices.size() - 1)];
		const size_t point_count = simplex.point_count();
		// draw points
		glBindVertexArray(graphics.primitives[primitive_cube].vao);
		glDisable(GL_CULL_FACE);
		for (int j = 0; j < point_count; ++j) {
			glUniformMatrix4fv(uniform_mvp, 1, false, &(vp * glm::translate(simplex.points()[j]) * mtx_sc)[0][0]);
			graphics.primitives[primitive_cube].draw();
		}
		if (point_count == 2) {
			// draw line
			graphics.debug_line(simplex.points()[0], simplex.points()[1]);
			graphics.draw_debug_lines(vp, color);
		}
		if (point_count == 3) {
			// draw triangle
			draw_triangle({ simplex.points()[0], simplex.points()[1] ,simplex.points()[2] }, vp, color);
		}
		if (point_count == 4) {
			color = Color{ 0xffff4488 };
			// draw tetrahedron
			draw_triangle({ simplex.points()[0], simplex.points()[1] ,simplex.points()[2] }, vp, color);
			draw_triangle({ simplex.points()[0], simplex.points()[3] ,simplex.points()[1] }, vp, color);
			draw_triangle({ simplex.points()[0], simplex.points()[2] ,simplex.points()[3] }, vp, color);
			draw_triangle({ simplex.points()[1], simplex.points()[3] ,simplex.points()[2] }, vp, color);
		}
		glEnable(GL_CULL_FACE);
	}

	// OBJECT 0
	glBindVertexArray(obj0.get_mesh_buffers()->vao);
	glUniformMatrix4fv(uniform_mvp, 1, false, &(vp * obj0.get_model())[0][0]);
	Color{ vec4{1, 0.5, 0.5, 0.5} }.set_uniform_RGBA(uniform_color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	obj0.get_mesh_buffers()->draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Color{ vec4{1, 0.5, 0.5, 1} }.set_uniform_RGBA(uniform_color);
	obj0.get_mesh_buffers()->draw();
	// OBJECT 1
	glBindVertexArray(obj1.get_mesh_buffers()->vao);
	glUniformMatrix4fv(uniform_mvp, 1, false, &(vp * obj1.get_model())[0][0]);
	Color{ vec4{1, 0.5, 0.5, 0.5} }.set_uniform_RGBA(uniform_color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	obj1.get_mesh_buffers()->draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Color{ vec4{1, 0.5, 0.5, 1} }.set_uniform_RGBA(uniform_color);
	obj1.get_mesh_buffers()->draw();

	pop_gl_errors(__FUNCTION__);
}
