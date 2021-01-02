/**
* @file demo.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement demo
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>


Demo_bvh::Demo_bvh()
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

	// setup camera
	camera.pos = { -3, 5, 7 };
	camera.target = -glm::normalize(camera.pos) + camera.pos;

	camera_axis_speed = 20.f;

	// add some objects
	add_object();
	int COUNT = 6;
	for (int i = 0; i < COUNT; ++i)
		add_random_object();

	// set moving timer
	timer.set(2);
}

Demo_bvh::~Demo_bvh()
{

}

/**
*
* @brief update
*/
void Demo_bvh::update()
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

	// move objects around
	{
		if (move_objects) {
			TIMER_S(move_time);
			bool timeout = timer();
			for (auto& o : objects) {
				vec3 acc{};
				if (timeout) {
					vec3 rand_acc = glm::linearRand(vec3{ -1 }, vec3{ 1 }) * 200.f;
					acc += rand_acc;
				}
				acc += -o.get_pos() * glm::length2(o.get_pos()) * 0.0003f;
				o.set_acc(o.get_acc() + acc);
				//o.set_vel(rand_acc);
				
				//o.update_physics(dt);

				// update bvh
				objects_bvh.remove_object(o);
				objects_bvh.add_object(o);
			}
			TIMER_E(move_time);
		}
		for (auto& o : objects)
			o.update_physics(dt);

	}

	// raycast collision
	{
		if( ImGui::IsAnyItemHovered() == false && ImGui::IsAnyWindowFocused() == false) // not using GUI
		if (mouse.pressed(0)) {
			TIMER_S(raycast_collision_time);

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

			// traverse bvh
			auto ray_check = [&](BVH::node* n) -> BVH::children_list {
				BVH::children_list best_order{ nullptr };
				std::array<float, best_order.size()> best_rays{ FLT_MAX };
				std::array<int, best_order.size()> indices{ -1 };
				int rays_hit = 0;
				for (int i = 0; i < n->children.size(); ++i) {
					indices[i] = i;
					BVH::node* c = n->children[i];
					if (c) {
						float t = intersection_ray_aabb(mouse_ray, c->bounding_volume);
						best_rays[i] = t;
						if (t >= 0) {	// if hit
							rays_hit++;
						}
					}
				}
				// sort by closest hit
				std::sort(indices.begin(), indices.begin() + rays_hit, [&](int l, int r) -> bool {
					return best_rays[l] < best_rays[r];
				});
				// order children
				for (int i = 0; i < best_order.size(); ++i)
					if(best_rays[indices[i]] >= 0.f)
						best_order[i] = n->children[indices[i]];

				return best_order;
			};
			TIMER_S(tree_find_time);
			BVH::node* hit_node = objects_bvh.find(ray_check);
			// is leaf
			if (hit_node && hit_node->objects.empty() == false) {
				if (ray_mesh_check) {
					Object& obj = *const_cast<Object*>(hit_node->objects[0]);	// jeje, const you said?
					// mesh check
					const auto& positions = obj.get_mesh_data()->positions;
					const auto& indices = obj.get_mesh_data()->indices;
					const mat4& model = obj.get_model();
					float ray_t = intersection_ray_mesh(mouse_ray, model, positions.data(), indices.data(), (tri_idx)indices.size());
					if (ray_t >= 0.f) {
						// push object
						float force = 500;
						if (mouse.pressed(1))
							force *= -1;
						ray_add_force(obj, mouse_ray, ray_t, force);
						//assert(hit_node->objects.empty() == false);

						// add to selected list (avoid duplicated)
						auto it = std::find(selected.begin(), selected.end(), &obj);
						if (it == selected.end())
							selected.push_back(const_cast<Object*>(&obj));
					}
				}
				else {
					// add to selected list (avoid duplicated)
					auto it = std::find(selected.begin(), selected.end(), hit_node->objects[0]);
					if (it == selected.end())
						selected.push_back(const_cast<Object*>(hit_node->objects[0]));
				}
				TIMER_E(tree_find_time);
			}
			else {
				TIMER_R(tree_find_time);
				TIMER_S(linear_find_time);
				// traverse object list
				for (Object& o : objects) {
					
					float t = intersection_ray_aabb(mouse_ray, o.get_aabb());
					if (t >= 0.f) {
						if (ray_mesh_check) {
							// mesh check
							const auto& positions = o.get_mesh_data()->positions;
							const auto& indices = o.get_mesh_data()->indices;
							const mat4& model = o.get_model();
							float ray_t = intersection_ray_mesh(mouse_ray, model, positions.data(), indices.data(), (tri_idx)indices.size());
							if (ray_t >= 0.f) {
								float force = 500;
								if (mouse.pressed(1))
									force *= -1;
								ray_add_force(o, mouse_ray, ray_t, force);
								// add to selected list (avoid duplicated)
								auto it = std::find(selected.begin(), selected.end(), &o);
								if (it == selected.end())
									selected.push_back(&o);
							}
						}
						else {
							// add to selected list (avoid duplicated)
							auto it = std::find(selected.begin(), selected.end(), &o);
							if (it == selected.end())
								selected.push_back(&o);
						}
						
					}
				}
				TIMER_E(linear_find_time);
			}

			// TODO: traverse tree and raycast check

			TIMER_E(raycast_collision_time);
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
			mat4 target_mtx = selected[0]->get_model();// glm::translate(manipule_pos);
			const mat4 view = camera.get_view();
			const mat4 proj = camera.get_proj();
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
			if(ImGui::DragInt2("Window Size", &graphics.window.size[0])){
				graphics.window.resize(graphics.window.size.x, graphics.window.size.y);
				camera.size = graphics.window.size;
			}
			if (ImGui::DragInt2("Window Pos", &graphics.window.position[0])) {
				glfwSetWindowPos(graphics.window.glfwWindow, graphics.window.position.x, graphics.window.position.y);
			}
		}
		// CAMERA
		if (ImGui::CollapsingHeader("CAMERA")) {
			ImGui::Text("X %f Y %f Z %f", camera.pos.x, camera.pos.y, camera.pos.z);
			float angle = glm::degrees(camera.fov);
			ImGui::DragFloat("FoV", &angle, 1.f, 1.f, 180.f);
			camera.fov = glm::radians(angle);
			ImGui::DragFloat("Near", &camera.near_plane, 0.01f, 0.0001f, 10.f);
			ImGui::DragFloat("Far", &camera.far_plane, 1.f, 10.f, 100000.f);
			ImGui::DragFloat2("Size", &camera.size[0]);
			// camera control
			ImGui::DragFloat2("MouseVel", &mouse_velocity[0]);
			ImGui::DragFloat("AxisSpeed", &camera_axis_speed);
		}
		// SCENE
		if (ImGui::CollapsingHeader("SCENE")) {
			ImGui::Checkbox("Move Objects", &move_objects);
			ImGui::Text("Move Time = %f", move_time);
			float timer_end = timer.get_end();
			ImGui::DragFloat("Timer End", &timer_end, 0.1f, 0.0f, 10.f);
			timer.set(timer_end);
			ImGui::Text("Ray Tree Find = %f", tree_find_time);
			ImGui::Text("Ray Linear Find = %f", linear_find_time);
			ImGui::Checkbox("Mesh Collision", &ray_mesh_check);
		}
		// BOUNDING VOLUME HIERARCHY
		if (ImGui::CollapsingHeader("BV HIERARCHY")) {
			if (ImGui::Button("Clear"))
				objects_bvh.destroy();
			if (ImGui::Button("Bottom-up")) {
				TIMER_S(bottom_up_time);
				// destroy tree
				objects_bvh.destroy();
				// allocate pointers
				std::vector<const Object*> objs;
				objs.reserve(objects.size());
				for (Object& o : objects)
					objs.push_back(&o);
				objects_bvh.build_bottom_up(objs);
				TIMER_E(bottom_up_time);
			}
			ImGui::Text("Bottom Up Time = %f", bottom_up_time);
			if (ImGui::Button("Top-down")) {
				TIMER_S(top_down_time);
				// destroy tree
				objects_bvh.destroy();
				// allocate pointers
				std::vector<const Object*> objs;
				objs.reserve(objects.size());
				for (Object& o : objects)
					objs.push_back(&o);
				objects_bvh.build_top_down(objs);
				TIMER_E(top_down_time);
			}
			ImGui::Text("Top Down Time = %f", top_down_time);
			// add selected to BVH
			if (!selected.empty()) {
				if (ImGui::Button("Add to BVH")) {
					for(auto o : selected)
						objects_bvh.add_object(*o);
				}
				if (ImGui::Button("Remove from BVH")) {
					for(auto o : selected)
						objects_bvh.remove_object(*o);
				}
			}
			auto set_draw_node = [&](const BVH::node& n) {
				n.draw_bv = draw_hierarchy;
			};
			if (ImGui::Checkbox("Draw BVH", &draw_hierarchy)) {
				objects_bvh.traverse_preorder(set_draw_node);
			}
			ImGui::TextColored({ 0.4f,0.4f,1.f,1.f }, "ROOT");
			ImGui::TextColored({ 1,0,0,1 }, "INTERMEDIATE");
			ImGui::TextColored({ 0,1,0,1 }, "LEAF");
			if (ImGui::TreeNode("BV Nodes")) {
				int node_number = 0;
				auto edit_node = [&](const BVH::node& n) {
					// pick color
					Color c;
					if (&n == objects_bvh.root())
						c = Color{ 0x4444ffff };
					else if (n.objects.empty())
						c = Color{ 0xff0000ff };
					else
						c = Color{ 0x00ff00ff };
					ImGui::PushStyleColor(ImGuiCol_Text, (u32)c);
					std::string obj_name;
					if (n.objects.size() == 1)
						obj_name = n.objects[0]->get_name();
					if (ImGui::TreeNode(std::to_string(node_number).c_str(), "Node %d %s", node_number, obj_name.c_str())){
						ImGui::Text("AABB MIN = %10.3f,%10.3f,%10.3f", n.bounding_volume.min_point.x, n.bounding_volume.min_point.y, n.bounding_volume.min_point.z);
						ImGui::Text("AABB MAX = %10.3f,%10.3f,%10.3f", n.bounding_volume.max_point.x, n.bounding_volume.max_point.y, n.bounding_volume.max_point.z);
						ImGui::Checkbox("Draw", &n.draw_bv);
						ImGui::TreePop();
					}
					ImGui::PopStyleColor();
					node_number++;
				};
				// edit all tree
				objects_bvh.traverse_preorder(edit_node);

				ImGui::TreePop();
			}
		}

		// OBJECT PROPERTIES
		if (ImGui::CollapsingHeader("OBJECT LIST")) {
			const char* mesh_names[mesh_type_count] = {
				"triangle", "sphere", "segment", "quad",
				"octohedron", "icosahedron", "gourd", "cylinder", "cube", "bunny"
			};
			ImGui::Text("COUNT = %d", objects.size());

			static int num = 0;
			if (ImGui::Button("+"))
				for (int i = 0; i < num; ++i)
					add_random_object();
			ImGui::SameLine();
			if (ImGui::Button("-")) {
				const int max_pops = glm::min(num, (int)objects.size());
				for (int i = 0; i < max_pops; ++i)
					remove_last_object();
			}
			ImGui::SameLine();

			ImGui::DragInt("Num", &num, 1, 0, 1000);
			if (ImGui::Button("Add Random")) {
				add_random_object();
			}
			ImGui::SameLine();
			if(objects.empty() == false)
			if (ImGui::Button("Delete last")) {
				remove_last_object();
			}

			// show all object list

			for (Object& o : objects) {
				const auto it = std::find(selected.begin(), selected.end(), &o);
				bool color_changed = false;
				if (it != selected.end()) {
					u32 c = (u32)Color { 0xffff00ff };
					ImGui::PushStyleColor(ImGuiCol_Text,  c);
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
				if(color_changed)
					ImGui::PopStyleColor();
			}
		}
		

		ImGui::End();

		// SHOW SELECTED OBJECTS
		if (selected.empty() == false) {
			ImGui::Begin("Inspector");

			const char* mesh_names[mesh_type_count] = {
	"triangle", "sphere", "segment", "quad",
	"octohedron", "icosahedron", "gourd", "cylinder", "cube", "bunny"
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
					bool update_bvh = false;
					vec3 tmp = o.get_pos();
					if (ImGui::DragFloat3("Pos", &tmp[0], 0.1f)) {
						o.set_pos(tmp);
						update_bvh = true;
					}
					tmp = o.get_scale();
					if (ImGui::DragFloat3("Scale", &tmp[0], 0.01f, 0.01f, 100.f)) {
						o.set_scale(tmp);
						update_bvh = true;
					}
					tmp = o.get_rot();
					if (ImGui::DragFloat3("Rot", &tmp[0], 0.1f, -180.f, 180.f)) {
						o.set_rot(tmp);
						update_bvh = true;
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
								update_bvh = true;
								break;
							}
						}
						ImGui::EndMenu();
					}

					// Update BVH
					if (update_bvh) {
						objects_bvh.remove_object(o);
						objects_bvh.add_object(o);
					}

					ImGui::TreePop();
				}
			}

			if (to_delete > -1) {
				remove_selected_object(to_delete);
			}



			ImGui::End();
		}
	}
}

/**
*
* @bref render
*/
void Demo_bvh::render() const
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	glDisable(GL_CULL_FACE);

	// compute camera matrices
	mat4 proj = camera.get_proj();
	mat4 view = camera.get_view();
	const mat4 vp = proj * view;

	glUseProgram(graphics.shader_program[sh_color]);

	// DRAW MAIN MODEL
	int uniform_color = glGetUniformLocation(graphics.shader_program[sh_color], "color");
	int uniform_mvp = glGetUniformLocation(graphics.shader_program[sh_color], "MVP");
	
	pop_gl_errors("Setup Render");
	
	for (const Object & o : objects) {
		if (o.get_mesh_buffers() == nullptr && o.get_mesh_data() == nullptr)
			continue;
		mat4 mvp = vp * o.get_model();
		glUniformMatrix4fv(uniform_mvp, 1, false, &(mvp)[0][0]);

		if(o.get_mesh_data() && o.get_mesh_data()->positions.size() == 2) {
			const auto &p = o.get_mesh_data()->positions;
			graphics.debug_line(p[0], p[1]);
			graphics.draw_debug_lines(mvp, o.get_color(), 9.f);
		}
		else {

			glBindVertexArray(o.get_mesh_buffers()->vao);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//glLineWidth(10.f);
			Color{ 0x000000ff }.set_uniform_RGBA(uniform_color);
			o.get_mesh_buffers()->draw();

			pop_gl_errors("Render Wireframe");

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			o.get_color().set_uniform_RGBA(uniform_color);
			o.get_mesh_buffers()->draw();

			pop_gl_errors("Render Model");
		}

		// DRAW BOUNDING VOLUME
#if 0
		{
			Color c{ 0xff00ff44 };
			draw_aabb(o.get_aabb(), vp, c);
			//draw_obb(o.get_obb(), vp * o.get_model(), o.get_color());
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//draw_aabb(o.get_aabb(), vp, o.get_color());
			draw_obb(o.get_obb(), vp * o.get_model(), o.get_color());
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


			pop_gl_errors("Render BV");

		}
#endif
	}

	auto draw_bvh_node = [&, vp](const bounding_volume_hierarchy::node& n){
		if (n.draw_bv) {
			// pick color
			Color c;
			if (&n == objects_bvh.root())
				c = Color{ 0x0000ffff };
			else if (n.objects.empty())
				c = Color{ 0xff0000ff };
			else
				c = Color{ 0x00ff00ff };
			draw_aabb(n.bounding_volume, vp, c);
		}
	};
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	objects_bvh.traverse_inorder(draw_bvh_node);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// DEBUG LINES
	graphics.debug_line(debug_ray_start, debug_ray_end);
	graphics.draw_debug_lines(vp, Color{ 0xffff00ff });

	pop_gl_errors(__FUNCTION__);
}

/**
*
* @brief create a default object
*/

void Demo_bvh::add_object()
{
	Object o;
	o.set_name("Object_" + std::to_string(objects.size()));
	o.set_pos(vec3{});
	o.set_scale(vec3{ 1.f });
	o.set_rot(vec3{0.f});
	o.set_color(Color{});
	o.set_mesh_data(&mesh_data[mesh_cube]);
	o.set_mesh_buffers(&mesh_buffer[mesh_cube]);

	objects.emplace_back(std::move(o));

}

/**
*
* @brief create a random object (for testing many objects)
*/
void Demo_bvh::add_random_object(int MIN, int MAX)
{
	Object o;
	o.set_name( "Object_" + std::to_string(objects.size()));
	o.set_pos(vec3{ glm::linearRand(MIN, MAX), 0.f, glm::linearRand(MIN, MAX) });
	o.set_scale(vec3{ glm::linearRand(0.5f, 5.f) });
	o.set_rot(vec3{glm::linearRand(-M_PI, M_PI), glm::linearRand(-M_PI, M_PI),glm::linearRand(-M_PI, M_PI) });
	o.set_color(Color{glm::linearRand((u32)0x00000000, (u32)0xfffffffe)});
	int rand_mesh_type = glm::linearRand(0, (int)mesh_type_count - 1);
	o.set_mesh_data(&mesh_data[rand_mesh_type]);
	o.set_mesh_buffers(&mesh_buffer[rand_mesh_type]);

	objects.emplace_back(std::move(o));
}

void Demo_bvh::remove_object(Object& obj)
{
	// delete from selected
	auto it = std::find(selected.begin(), selected.end(), &obj);
	if (it != selected.end())
		selected.erase(it);
	// delete from bvh
	objects_bvh.remove_object(obj);
	// delete from scene
	auto obj_it = std::find(objects.begin(), objects.end(), obj);
	objects.erase(obj_it);
}
void Demo_bvh::remove_last_object()
{
	// get obj to delete
	const Object& to_delete = objects.back();
	// delete from selected
	auto it = std::find(selected.begin(), selected.end(), &to_delete);
	if (it != selected.end())
		selected.erase(it);
	// delete from bvh
	objects_bvh.remove_object(to_delete);
	// delete from scene
	objects.pop_back();
}

void Demo_bvh::remove_selected_object(int select_idx)
{
	// get index to delete
	Object* obj_to_delete = selected[select_idx];
	// delete from selected
	selected.erase(selected.begin() + select_idx);
	// delete from bvh
	objects_bvh.remove_object(*obj_to_delete);
	// delete from scene (find pointer)
	auto it = std::find_if(objects.begin(), objects.end(), [obj_to_delete](const Object& obj) -> bool { return obj_to_delete == &obj; });
	if (it != objects.end())
		objects.erase(it);
}

void Demo_bvh::ray_add_force(Object& obj, const Ray& ray, float t, float force) {
	// get contact point
	vec3 p = ray.start + ray.dir * t;
	float ray_len = glm::length(ray.dir);
	assert(glm::epsilonEqual(ray_len, 0.f, glm::epsilon<float>()) == false);
	// get direction to center of object (the force will be aplied in this direction
	vec3 dir = obj.get_pos() - p;
	float dir_len = glm::length(dir);
	assert(glm::epsilonEqual(dir_len, 0.f, glm::epsilon<float>()) == false);
	// dot product to the final force
	force *= glm::dot(dir, ray.dir) / (dir_len * ray_len);
	//normalize dir
	dir = force * dir / dir_len;
	obj.set_acc(obj.get_acc() + dir);
}