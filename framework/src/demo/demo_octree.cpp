/**
* @file demo.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement demo
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>


Demo_octree::Demo_octree()
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
	camera.pos = { 0, 0, 10 };
	camera.target = -glm::normalize(camera.pos) + camera.pos;
	camera_axis_speed = 20.f;


	const float OCTREE_SIZE = 60;
	// add some objects
	Object& obj0 = add_object();
	obj0.set_pos(vec3{ OCTREE_SIZE / 2 + OCTREE_SIZE / 4 + OCTREE_SIZE / 8});
	int COUNT = 2;
	for (int i = 0; i < COUNT; ++i)
		add_random_object();

	// set moving timer
	timer.set(2);

	// octree stuff
	octree.get_size() = OCTREE_SIZE;
	std::vector<const Object*> object_vec; object_vec.reserve(objects.size());
	for (auto & o : objects)
		object_vec.push_back(&o);
	octree.build(object_vec);
}

Demo_octree::~Demo_octree()
{

}

/**
*
* @brief update
*/
void Demo_octree::update()
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


		// if not detached, update frustum
		if (!is_camera_detached) {
			aux_cam.pos = camera.pos;
			aux_cam.target = camera.target;
		}
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

	// move objects around
	{
		if (move_objects) {
			move_time = get_time();
			bool timeout = timer();
			for (auto& o : objects) {
				vec3 acc{};
				if (timeout) {
					vec3 rand_acc = glm::linearRand(vec3{ -1 }, vec3{ 1 }) * 200.f;
					acc += rand_acc;
				}
				acc += -o.get_pos() * glm::length2(o.get_pos()) * 0.0003f;
				o.set_acc(acc);

				octree.remove_object(o);	// update octree
				//o.set_vel(rand_acc);
				o.update_physics(dt);
				octree.add_object(o);	// update octree

				// update bvh
				//objects_bvh.remove_object(o);
				//objects_bvh.add_object(o);

				// TODO: update octree (take the node before moving!)
			}
			move_time = get_time() - move_time;
		}

	}

	// raycast collision
	{
		if( ImGui::IsAnyItemHovered() == false && ImGui::IsAnyWindowFocused() == false) // not using GUI
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
			Camera * edit_cam = &aux_cam;
			if (is_camera_detached)
				edit_cam = &aux_cam;
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
			// detacheable camera
			if (ImGui::Checkbox("Detach", &is_camera_detached) || must_update_frustum) {
				mb_frustum.clear();	// hide or update
				if (is_camera_detached) {
					// update frustum
					create_frustum_mesh(Frustum{ aux_cam.get_proj() * aux_cam.get_view() }, &mb_frustum);
				}
			}

		}
		// SCENE
		if (ImGui::CollapsingHeader("SCENE")) {
			ImGui::Checkbox("Move Objects", &move_objects);
			ImGui::Text("Move Time = %f", move_time);
			float timer_end = timer.get_end();
			ImGui::DragFloat("Timer End", &timer_end, 0.1f, 0.0f, 10.f);
			timer.set(timer_end);
		}

		// OCTREE
		if (ImGui::CollapsingHeader("OCTREE")) {
			if (ImGui::Button("Clear"))
				octree.destroy();
			if (ImGui::Button("Build")) {
				std::vector<const Object*> object_vec; object_vec.reserve(objects.size());
				for (auto & o : objects)
					object_vec.push_back(&o);
				octree.build(object_vec);
			}
			if (!selected.empty()) {
				if (ImGui::Button("Add to Octree")) {
					for (auto o : selected)
						octree.add_object(*o);
				}
				if (ImGui::Button("Remove from Octree")) {
					for (auto o : selected)
						octree.remove_object(*o);
				}
			}
			ImGui::DragInt("Max Levels", &octree.get_max_level_count(), 1, 1, OCT_MAX_LEVEL);
			ImGui::DragFloat3("OcT Pos", &octree.get_center()[0], 0.1f, -100.f, 100.f);
			ImGui::DragFloat("OcT Size", &octree.get_size(), 0.1f, 1.f, 100.f);
			// level colors
			if (ImGui::TreeNode("Color Codes")) {

				ImGui::TextColored({ 0.5f,0.5f,0.5f,1 }, "000");	ImGui::SameLine();
				ImGui::TextColored({ 0,0,1,1 }, "001");
				ImGui::TextColored({ 0,1,0,1 }, "010"); ImGui::SameLine();
				ImGui::TextColored({ 0,1,1,1 }, "011");
				ImGui::TextColored({ 1,0,0,1 }, "100"); ImGui::SameLine();
				ImGui::TextColored({ 1,0,1,1 }, "101");
				ImGui::TextColored({ 1,1,0,1 }, "110"); ImGui::SameLine();
				ImGui::TextColored({ 1,1,1,1 }, "111");

				ImGui::TreePop();
			}
			// show hierarchy and codes
			if (ImGui::TreeNode("Octree Nodes")) {
				auto edit_node = [&](const Octree::node& n) {
					// pick color
					Octree::loc_code local_code = n.code & OCT_BIT_CODE_LOCAL_MASK;
					Color c = vec4{ local_code & OCT_BIT_CODE_X, local_code & OCT_BIT_CODE_Y, local_code & OCT_BIT_CODE_Z, 1 };
					if (c.r == 0 && c.g == 0 && c.b == 0)
						c = vec4{ 0.5f, 0.5f, 0.5f, 1.f };
					ImGui::PushStyleColor(ImGuiCol_Text, (u32)c);
					std::string code_str = "Code:" + octree.get_code_string(n.code);
					// print all object names
					if (ImGui::TreeNode(code_str.c_str())) {
						ImGui::Text("Pos=(%f,%f,%f)", n.pos.x, n.pos.y, n.pos.z);
						ImGui::Text("Size=(%f)", n.size);
						for(const Object* o : n.objects)
							ImGui::Text(o->get_name().c_str());

						ImGui::TreePop();
					}
					ImGui::PopStyleColor();
				};
				// edit all tree
				octree.traverse_preorder(edit_node);
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
			if (ImGui::Button("Add Random")) {
				add_random_object();
			}
			ImGui::SameLine();
			if(objects.empty() == false)
			if (ImGui::Button("Delete last")) {
				// get obj to delete
				const auto& to_delete = objects.back();
				// delete from selected
				auto it = std::find(selected.begin(), selected.end(), &to_delete);
				if(it != selected.end())
					selected.erase(it);
				// delete from octree
				octree.remove_object(to_delete);
				// delete from scene
				objects.pop_back();
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
						octree.remove_object(o);	// update octree
						o.set_pos(tmp);
						octree.add_object(o);	// update octree
						update_bvh = true;
					}
					tmp = o.get_scale();
					if (ImGui::DragFloat3("Scale", &tmp[0], 0.01f, 0.01f, 100.f)) {
						octree.remove_object(o);	// update octree
						o.set_scale(tmp);
						octree.add_object(o);	// update octree
						update_bvh = true;
					}
					tmp = o.get_rot();
					if (ImGui::DragFloat3("Rot", &tmp[0], 0.1f, -180.f, 180.f)) {
						octree.remove_object(o);	// update octree
						o.set_rot(tmp);
						octree.add_object(o);	// update octree
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
								octree.remove_object(o);	// update octree
								o.set_mesh_data(&mesh_data[j]);	// updates bv (obb)
								o.set_mesh_buffers(&mesh_buffer[j]);
								octree.add_object(o);	// update octree
								update_bvh = true;
								break;
							}
						}
						ImGui::EndMenu();
					}
					// TODO: Update Octree


					ImGui::TreePop();
				}
			}

			if (to_delete > -1) {
				// get index to delete
				Object* obj_to_delete = selected[to_delete];
				// delete from selected
				selected.erase(selected.begin() + to_delete);
				//delete from octree
				octree.remove_object(*obj_to_delete);
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
* @bref render
*/
void Demo_octree::render()const 
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

	// DRAW QUERY OCTREE
	Frustum frustum{ aux_cam.get_proj() * aux_cam.get_view() };
	auto draw_octree_objects = [&](const Octree::node& n) ->bool {
		intersection_type result = intersection_frustum_aabb(frustum, n.get_aabb());
		if (result != OUTSIDE) {
			// draw aabb
			Octree::loc_code local_code = n.code & OCT_BIT_CODE_LOCAL_MASK;
			Color c = Color{ vec4{local_code & 4, local_code & 2, local_code & 1, 1} };	// pick color
			if (n.code == 1)
				c = Color{};
			AABB aabb = n.get_aabb();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			draw_aabb(aabb, vp, c);
			glEnable(GL_CULL_FACE);
			// draw all objects
			for (const Object * o_ptr : n.objects) {
				const Object &o = *o_ptr;
				if (o.get_mesh_buffers() == nullptr && o.get_mesh_data() == nullptr)
					continue;
				mat4 mvp = vp * o.get_model();
				glUniformMatrix4fv(uniform_mvp, 1, false, &(mvp)[0][0]);

				if (o.get_mesh_data() && o.get_mesh_data()->positions.size() == 2) {
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
				{
					c = Color{ 0xff00ff44 };
					draw_aabb(o.get_aabb(), vp, c);
					//draw_obb(o.get_obb(), vp * o.get_model(), o.get_color());
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					//draw_aabb(o.get_aabb(), vp, o.get_color());
					draw_obb(o.get_obb(), vp * o.get_model(), o.get_color());
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


					pop_gl_errors("Render BV");

				}
			}

			return true;
		}
		return false;
	};
	octree.traverse_if(draw_octree_objects);

	// DRAW OCTREE
#if 0
	auto draw_octree_node = [&, vp](const Octree::node& n) {
		Octree::loc_code local_code = n.code & OCT_BIT_CODE_LOCAL_MASK;
		Color c = Color{ vec4{local_code & 4, local_code & 2, local_code & 1, 1} };	// pick color
		if (n.code == 1)
			c = Color{};
#if 0
		int level = n.level;//octree.get_level(n.code);
		if (level < octree_level_colors.size()) {
			assert(level < octree_level_colors.size());
			c = Color{ octree_level_colors[level] };
		}
#endif
		AABB aabb = n.get_aabb();
		//aabb.min_point *= octree.get_size();
		//aabb.min_point += octree.get_center();
		//aabb.max_point *= octree.get_size();
		//aabb.max_point += octree.get_center();
		draw_aabb(aabb, vp, c);
	};
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	octree.traverse_preorder(draw_octree_node);
	glEnable(GL_CULL_FACE);
#endif

	// DEBUG FRUSTUM
	if (is_camera_detached) {
		assert(mb_frustum.vao != 0);
		glEnable(GL_CULL_FACE);
		glUseProgram(graphics.shader_program[sh_color]);
		glUniformMatrix4fv(uniform_mvp, 1, false, &vp[0][0]);
		Color{ 0x66666644 }.set_uniform_RGBA(uniform_color);
		glBindVertexArray(mb_frustum.vao);
		//glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mb_frustum.draw();
		Color{ 0xFF0000FF }.set_uniform_RGBA(uniform_color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		mb_frustum.draw();
		glEnable(GL_CULL_FACE);
	}

	// DEBUG LINES
	graphics.debug_line(debug_ray_start, debug_ray_end);
	graphics.draw_debug_lines(vp, Color{ 0xffff00ff });


	pop_gl_errors(__FUNCTION__);
}

/**
*
* @brief create a default object
*/

Object& Demo_octree::add_object()
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
	return objects.back();
}

/**
*
* @brief create a random object (for testing many objects)
*/
Object& Demo_octree::add_random_object(int MIN, int MAX)
{
	Object o;
	o.set_name( "Object_" + std::to_string(objects.size()));
	o.set_pos(vec3{ glm::linearRand(MIN, MAX), glm::linearRand(MIN, MAX), glm::linearRand(MIN, MAX) });
	o.set_scale(vec3{ glm::linearRand(0.5f, 5.f) });
	o.set_rot(vec3{glm::linearRand(-M_PI, M_PI), glm::linearRand(-M_PI, M_PI),glm::linearRand(-M_PI, M_PI) });
	o.set_color(Color{glm::linearRand((u32)0x00000000, (u32)0xfffffffe)});
	int rand_mesh_type = glm::linearRand(0, (int)mesh_type_count - 1);
	o.set_mesh_data(&mesh_data[rand_mesh_type]);
	o.set_mesh_buffers(&mesh_buffer[rand_mesh_type]);

	objects.emplace_back(std::move(o));
	return objects.back();
}

