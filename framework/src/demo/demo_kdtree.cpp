/**
* @file demo.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement demo
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>
namespace
{
	// set draw to all subnodes
	void set_draw_kdtree(bool b_draw, const kdtree &_kdtree, std::vector<int> &draw_nodes, int node_idx) {
		draw_nodes[node_idx] = (int)b_draw;
		const kdtree::node& n = _kdtree.nodes()[node_idx];
		if (n.is_internal()) {
			set_draw_kdtree(b_draw, _kdtree, draw_nodes, node_idx + 1);
			set_draw_kdtree(b_draw, _kdtree, draw_nodes, n.next_child());
		}
	}

	void gui_kdtree(const kdtree &_kdtree, std::vector<int> &draw_nodes, int node_idx)
	{
		const auto& nodes = _kdtree.nodes();
		const kdtree::node &n = nodes[node_idx];
		bool b_draw = (bool)draw_nodes[node_idx];
		std::string id = "Draw " + std::to_string(node_idx);
		if (ImGui::Checkbox(id.c_str(), &b_draw))
			set_draw_kdtree(b_draw, _kdtree, draw_nodes, node_idx);
		ImGui::SameLine();
		if (ImGui::TreeNode(std::to_string(node_idx).c_str())) {
			if (n.is_internal()) {
				ImGui::Text("Internal");
				ImGui::Text("Split point: %f", n.split());
				ImGui::Text("Subnode idx: %d", n.next_child());
				ImGui::Text("Axis: %s", n.axis() == 0 ? "X" : n.axis() == 1 ? "Y" : "Z");

				gui_kdtree(_kdtree, draw_nodes, node_idx + 1);
				gui_kdtree(_kdtree, draw_nodes, n.next_child());

			}
			else {
				ImGui::Text("Leaf");
				ImGui::Text("Primitive idx: %d", n.primitive_start());
				ImGui::Text("Primitive count: %d", n.primitive_count());
			}


			ImGui::TreePop();
		}
	}
}

Demo_kdtree::Demo_kdtree()
{

	load_mesh_obj("../resources/meshes/triangle.obj", &mesh_buffer[mesh_triangle], &mesh_data[mesh_triangle]);
	load_mesh_obj("../resources/meshes/sphere.obj", &mesh_buffer[mesh_sphere], &mesh_data[mesh_sphere]);
	load_mesh_obj("../resources/meshes/segment.obj", &mesh_buffer[mesh_segment], &mesh_data[mesh_segment]);
	load_mesh_obj("../resources/meshes/quad.obj", &mesh_buffer[mesh_quad], &mesh_data[mesh_quad]);
	load_mesh_obj("../resources/meshes/octohedron.obj", &mesh_buffer[mesh_octohedron], &mesh_data[mesh_octohedron]);
	load_mesh_obj("../resources/meshes/icosahedron.obj", &mesh_buffer[mesh_icosahedron], &mesh_data[mesh_icosahedron]);
	load_mesh_obj("../resources/meshes/gourd.obj", &mesh_buffer[mesh_gourd], &mesh_data[mesh_gourd]);
	load_mesh_obj("../resources/meshes/cylinder.obj", &mesh_buffer[mesh_cylinder], &mesh_data[mesh_cylinder]);
	load_mesh_obj("../resources/meshes/cube.obj", &mesh_buffer[mesh_cube], &mesh_data[mesh_cube]);
	load_mesh_obj("../resources/meshes/bunny.obj", &mesh_buffer[mesh_bunny], &mesh_data[mesh_bunny]);

	// models from test
	std::vector<Triangle> triangles;
	triangles.push_back({ {{{0, 0, 0}, {0.5, 0.5, 0}, {0.5, 0, 0}}} });       // Most left triangle
	triangles.push_back({ {{{2.0, 2.0, 0}, {2.5, 2.5, 0}, {2.5, 0, 0}}} });   // Right cluster
	triangles.push_back({ {{{2.5, 2.0, 0}, {3.0, 2.5, 0}, {3.0, 2, 0}}} });   // Right cluster
	triangles.push_back({ {{{2.0, 2.5, 0}, {2.5, 2.5, 0}, {2.5, 2.5, 0}}} }); // Right cluster
	triangles.push_back({ {{{2.5, 2.5, 0}, {3.0, 3.0, 0}, {3.0, 2.5, 0}}} }); // Right cluster
	create_mesh_from_triangles(&mesh_buffer[mesh_slides], &mesh_data[mesh_slides], triangles);

	// setup camera
	camera.pos = { 0, 0, 10 };
	camera.target = -glm::normalize(camera.pos) + camera.pos;
	camera_axis_speed = 20.f;


	auto &obj0 = add_object();
	obj0.set_mesh_data(&mesh_data[MeshType::mesh_gourd]);
	obj0.set_mesh_buffers(&mesh_buffer[MeshType::mesh_gourd]);

	// build kdtree for the first object
	const MeshData* md = objects.front().get_mesh_data();
	const auto& positions = md->positions;
	const auto& indices = md->indices;
	triangles.clear();
	triangles.reserve(indices.size() / 3);
	for (int i = 0; i < indices.size(); i += 3) {
		Triangle tri{ positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]] };
		triangles.push_back(tri);
	}
	kd_tree.build(triangles, kdtree_config);
	kdtree_draw_nodes = std::vector<int>(kd_tree.nodes().size(), 1);
}

Demo_kdtree::~Demo_kdtree()
{

}

/**
*
* @brief update
*/
void Demo_kdtree::update()
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

	// update BV
	{
		if (keyboard.pressed(' ')) {
			// keep track of updating time
			bv_update_time = get_time();

			// TODO: update tree

			// keep track of updating time
			bv_update_time = get_time() - bv_update_time;
		}
	}

	// raycast collision
	{
		if (ImGui::IsAnyItemHovered() == false && ImGui::IsAnyWindowFocused() == false) // not using GUI
			if (mouse.pressed(0)) {
				raycast_collision_time = get_time();

				Ray mouse_ray = mouse.ray_cast();
				mat4 proj = camera.get_proj();
				mat4 view = camera.get_view();
				mat4 inv_proj = glm::inverse(proj);
				mat4 inv_view = glm::inverse(view);
				vec4 start = vec4{ mouse_ray.start, 1.f };
				vec4 end = vec4{ mouse_ray.start.x, mouse_ray.start.y, 1.f, 1.f };
				start = inv_proj * start;
				start /= start.w;
				start = inv_view * start;
				end = inv_proj * end;
				end /= end.w;
				end = inv_view * end;
				mouse_ray.start = start;
				mouse_ray.dir = end - start;

				// update debug line
				debug_ray_start = start;
				debug_ray_end = end;

				// add to the selection?
				if (!keyboard.pressed(GLFW_KEY_LEFT_SHIFT))
					selected.clear();

					// traverse object list
					for (Object& o : objects) {

						float t = intersection_ray_aabb(mouse_ray, o.get_aabb());
						if (t >= 0.f) {
							// add to selected list (avoid duplicated)
							auto it = std::find(selected.begin(), selected.end(), &o);
							if (it == selected.end())
								selected.push_back(&o);
						}


					}

				// TODO: traverse tree and raycast check


				raycast_collision_time = get_time() - raycast_collision_time;
			}

	}

	// ImGuizmo
#if 0	// TOFIX:  performs a division 0 and crashes...
	{
		if (selected.empty() == false) {
			// compute center
			vec3 manipule_pos = selected[0]->get_pos();
			for (int i = 1; i < selected.size(); ++i)
				manipule_pos += selected[i]->get_pos();
			manipule_pos /= selected.size();
			//mat4 target_mtx = glm::transpose(selected[0]->get_model());// glm::translate(manipule_pos);
			//const mat4 view_trans = glm::transpose(camera.get_view());
			//const mat4 proj_trans = glm::transpose(camera.get_proj());

			mat4 target_mtx = glm::translate(selected[0]->get_pos());
			//mat4 target_mtx = selected[0]->get_model();
			mat4 view = camera.get_view();
			mat4 proj = camera.get_proj();
			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, &target_mtx[0][0]);
			if (ImGuizmo::IsUsing()) {
				vec3 tr, sc, rot;
				ImGuizmo::DecomposeMatrixToComponents(&target_mtx[0][0], &tr[0], &sc[0], &rot[0]);
				// apply to all selected
				for (auto o : selected) {
					o->set_pos(o->get_pos() + tr);
				}
			}
		}
	}
#endif

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

		// KD-TREE
		if (ImGui::CollapsingHeader("KD-TREE")) {
			bool update_kdtree = false;
			// SELECT MESH
			{
				const char* mesh_names[mesh_type_count] = {
	"triangle", "sphere", "segment", "quad",
	"octohedron", "icosahedron", "gourd", "cylinder", "cube", "bunny", "slides"
				};
				std::string title = "Mesh: ";
				Object& o = objects.front();	// take the first allways
				// is mesh a known type?
				if (o.get_mesh_data() >= &mesh_data[0] && o.get_mesh_data() <= &mesh_data[mesh_type_count]) {
					title.append(mesh_names[int(o.get_mesh_data() - &mesh_data[0])]);
				}
				else
					title.append("IO K SE ILLO");
				if (ImGui::BeginMenu(title.c_str())) {
					for (int j = 0; j < mesh_type_count; ++j) {
						if (ImGui::MenuItem(mesh_names[j])) {
							o.set_mesh_data(&mesh_data[j]);	// updates bv (obb)
							o.set_mesh_buffers(&mesh_buffer[j]);

							update_kdtree = true;
							break;
						}
					}
					ImGui::EndMenu();
				}
			}
			if (ImGui::Button("Build") || update_kdtree) {
				if (objects.empty() == false) {
					// build kdtree for the first object
					const MeshData* md = objects.front().get_mesh_data();
					const auto& positions = md->positions;
					const auto& indices = md->indices;
					std::vector<Triangle> triangles;
					triangles.reserve(indices.size() / 3);
					for (int i = 0; i < indices.size(); i += 3) {
						Triangle tri{ positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]] };
						triangles.push_back(tri);
					}
					kd_tree.build(triangles, kdtree_config);
					kdtree_draw_nodes = std::vector<int>(kd_tree.nodes().size(), 1);
				}
			}
			ImGui::Checkbox("Draw AABBs", &kdtree_draw_aabbs);
			ImGui::SliderInt("Max levels", &kdtree_config.max_depth, 0, 32);
			ImGui::DragFloat("Intersection cost", &kdtree_config.cost_intersection, 0.1f, 0.1f, 1000.f);
			ImGui::DragFloat("Traversal cost", &kdtree_config.cost_traversal, 0.1f, 0.1f, 1000.f);

			if (kd_tree.nodes().empty() == false)
				gui_kdtree(kd_tree, kdtree_draw_nodes, 0);
		}

		// OBJECT PROPERTIES
		if (ImGui::CollapsingHeader("OBJECT LIST")) {
			ImGui::Text("COUNT = %d", objects.size());
			if (ImGui::Button("Add Random")) {
				add_random_object();
			}
			ImGui::SameLine();
			if (objects.empty() == false)
				if (ImGui::Button("Delete last")) {
					// get obj to delete
					const auto& to_delete = objects.back();
					// delete from selected
					auto it = std::find(selected.begin(), selected.end(), &to_delete);
					if (it != selected.end())
						selected.erase(it);
					// delete from scene
					objects.pop_back();
				}

			// show all object list

			for (Object& o : objects) {
				const auto it = std::find(selected.begin(), selected.end(), &o);
				bool color_changed = false;
				if (it != selected.end()) {
					u32 c = (u32)Color { 0xffff00ff };
					ImGui::PushStyleColor(ImGuiCol_Text, c);
					color_changed = true;
				}
				if (ImGui::TreeNode(o.get_name().c_str())) {

					// add to selected
					//auto it = std::find(selected.begin(), selected.end(), &o);
					if (it == selected.end())
						selected.push_back(&o);

					ImGui::TreePop();
				}
				/*	DON'T

				else {
					// delete from selected
					//auto it = std::find(selected.begin(), selected.end(), &o);
					if(it != selected.end())
						selected.erase(it);
				}
				*/
				if (color_changed)
					ImGui::PopStyleColor();
			}
		}


		ImGui::End();

		// SHOW SELECTED OBJECTS
		if (selected.empty() == false) {
			ImGui::Begin("Inspector");

			const char* mesh_names[mesh_type_count] = {
	"triangle", "sphere", "segment", "quad",
	"octohedron", "icosahedron", "gourd", "cylinder", "cube", "bunny", "slides"
			};

			int to_delete = -1;
			for (int i = 0; i < selected.size(); ++i) {
				Object & o = *selected[i];

				if (ImGui::TreeNode(o.get_name().c_str())) {

					char name_buff[16] = { 0 };
					strcpy_s(name_buff, 16, o.get_name().c_str());
					ImGui::InputText("Name", name_buff, 16);
					o.set_name(name_buff);
					ImGui::SameLine();
					if (ImGui::Button("Delete")) {
						to_delete = i;
						ImGui::TreePop();
						break;
					}
					if (ImGui::Button("Focus")) {
						vec3 dir = o.get_pos() - camera.pos;
						camera.pos += dir * 0.7f;
						camera.target = camera.pos + glm::normalize(dir);
					}
					vec3 tmp = o.get_pos();
					if (ImGui::DragFloat3("Pos", &tmp[0], 0.1f)) {
						o.set_pos(tmp);
					}
					tmp = o.get_scale();
					if (ImGui::DragFloat3("Scale", &tmp[0], 0.01f, 0.01f, 100.f)) {
						o.set_scale(tmp);
					}
					tmp = o.get_rot();
					if (ImGui::DragFloat3("Rot", &tmp[0], 0.1f, -180.f, 180.f)) {
						o.set_rot(tmp);
					}
					Color c = o.get_color();
					vec4 color = vec4{ c.r, c.g, c.b, c.a } / 255.f;
					ImGui::ColorEdit4("Color", &color[0]);
					o.set_color(Color(color));

					std::string title = "Mesh: ";
					// is mesh a known type?
					if (o.get_mesh_data() >= &mesh_data[0] && o.get_mesh_data() <= &mesh_data[mesh_type_count]) {
						title.append(mesh_names[int(o.get_mesh_data() - &mesh_data[0])]);
					}
					else
						title.append("IO K SE ILLO");
					if (ImGui::BeginMenu(title.c_str())) {
						for (int j = 0; j < mesh_type_count; ++j) {
							if (ImGui::MenuItem(mesh_names[j])) {
								o.set_mesh_data(&mesh_data[j]);	// updates bv (obb)
								o.set_mesh_buffers(&mesh_buffer[j]);
								break;
							}
						}
						ImGui::EndMenu();
					}

					ImGui::TreePop();
				}
			}

			if (to_delete > -1) {
				// get index to delete
				Object* obj_to_delete = selected[to_delete];
				// delete from selected
				selected.erase(selected.begin() + to_delete);
				// delete from scene (find pointer)
				auto it = std::find_if(objects.begin(), objects.end(), [obj_to_delete](const Object& obj) -> bool { return obj_to_delete == &obj; });
				if (it != objects.end())
					objects.erase(it);
			}



			ImGui::End();
		}
	}
}

/**
*
* @brief render
*/
void Demo_kdtree::render() const
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	// DRAW MAIN MODEL
	int uniform_color = glGetUniformLocation(graphics.shader_program[sh_color], "color");
	int uniform_mvp = glGetUniformLocation(graphics.shader_program[sh_color], "MVP");

	pop_gl_errors("Setup Render");

#if 1
	// DRAW KD-TREE
	if (objects.empty() == false) {
		const auto& nodes = kd_tree.nodes();
		if (nodes.empty() == false) {
			glDisable(GL_CULL_FACE);
			const Object &obj = objects.front();
			mat4 mvp = vp * obj.get_model();
			// draw wireframe
			glUniformMatrix4fv(uniform_mvp, 1, false, &(mvp)[0][0]);
			glBindVertexArray(obj.get_mesh_buffers()->vao);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//glLineWidth(10.f);
			Color{ 0x000000ff }.set_uniform_RGBA(uniform_color);
			obj.get_mesh_buffers()->draw();

			// DRAW AABBs
			if(kdtree_draw_aabbs)
			for (int i = 0; i < kd_tree.aabbs().size(); ++i) {
				if (kdtree_draw_nodes[i] == (int)true) {
					draw_aabb(kd_tree.aabbs()[i], vp, Color{ 0xffff00ff });
				}
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			const auto& indices = obj.get_mesh_data()->indices;
			const auto& positions = obj.get_mesh_data()->positions;
			const auto& triangles = kd_tree.indices();
			for (int i = 0; i < nodes.size(); ++i) {
				const auto& n = nodes[i];
				if (n.is_leaf() && kdtree_draw_nodes[i] == (int)true) {
					const int tri_count = n.primitive_count();
					const int tri_start = n.primitive_start();
					for (int j = 0; j < tri_count; ++j) {
						const tri_idx idx = (tri_idx)triangles[tri_start + j] * 3;
						draw_triangle(Triangle{ positions[indices[idx]], positions[indices[idx + 1]], positions[indices[idx + 2]] }, mvp, Color{ 0xffffffff });
					}
				}
			}


			glEnable(GL_CULL_FACE);
			pop_gl_errors("KD-TREE draw");
		}
	}
#endif

	// DEBUG LINES
	graphics.debug_line(debug_ray_start, debug_ray_end);
	graphics.draw_debug_lines(vp, Color{ 0xffff00ff });

	pop_gl_errors(__FUNCTION__);
}

/**
*
* @brief create a default object
*/

Object& Demo_kdtree::add_object()
{
	Object o;
	o.set_name("Object_" + std::to_string(objects.size()));
	o.set_pos(vec3{});
	o.set_scale(vec3{ 1.f });
	o.set_rot(vec3{ 0.f });
	o.set_color(Color{});
	o.set_mesh_data(&mesh_data[mesh_cube]);
	o.set_mesh_buffers(&mesh_buffer[mesh_cube]);

	objects.emplace_back(std::move(o));
	return objects.back();
}

/**
*
* @brief create a random object (for testing many objects)
*/
Object& Demo_kdtree::add_random_object(int MIN, int MAX)
{
	Object o;
	o.set_name("Object_" + std::to_string(objects.size()));
	o.set_pos(vec3{ glm::linearRand(MIN, MAX), glm::linearRand(MIN, MAX), glm::linearRand(MIN, MAX) });
	o.set_scale(vec3{ glm::linearRand(0.5f, 5.f) });
	o.set_rot(vec3{ glm::linearRand(-M_PI, M_PI), glm::linearRand(-M_PI, M_PI),glm::linearRand(-M_PI, M_PI) });
	o.set_color(Color{ glm::linearRand((u32)0x00000000, (u32)0xfffffffe) });
	int rand_mesh_type = glm::linearRand(0, (int)mesh_type_count - 1);
	o.set_mesh_data(&mesh_data[rand_mesh_type]);
	o.set_mesh_buffers(&mesh_buffer[rand_mesh_type]);

	objects.emplace_back(std::move(o));
	return objects.back();
}
