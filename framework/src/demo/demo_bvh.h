#ifndef DEMO_BVH_H
#define DEMO_BVH_H

struct Demo_bvh : public Demo
{
	Demo_bvh();		// initialize stuff
	~Demo_bvh();	// free stuff

	void update() override;
	void render() const override;

	// ------ declare data here -------

	// camera stuff
	Camera camera;
	vec2 mouse_velocity = { 100.f, 100.f };
	float camera_axis_speed = 5.f;

	enum MeshType {
		mesh_no = -1, mesh_triangle = 0, mesh_sphere, mesh_segment, mesh_quad,
		mesh_octohedron, mesh_icosahedron, mesh_gourd, mesh_cylinder,
		mesh_cube, mesh_bunny, mesh_slides, mesh_type_count
	};
	// meshes
	MeshBuffers mesh_buffer[mesh_type_count];
	MeshData mesh_data[mesh_type_count];

	// bounding volumes
	BVType current_bv_type = bv_no;
	Color bv_color = Color{ 0x88888844 };

	void add_object();			// default object at origin
	void add_random_object(int MIN = -50, int MAX = 50);	// random object
	void remove_object(Object& obj);
	void remove_last_object();
	void remove_selected_object(int select_idx);

	void ray_add_force(Object& obj, const Ray& ray, float t, float force = 1.f);

private:
	// object storage (is a list so I don't lose pointers on growth)
	std::list<Object> objects;
	bool move_objects = false;
	double move_time = 0.0;
	TimeInterval timer;
	bounding_volume_hierarchy objects_bvh;
	bool draw_hierarchy = true;
	bool ray_mesh_check = true;

	ImGuizmo::OPERATION guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;


	// Raycast
	vec3 debug_ray_start{ 0.f }, debug_ray_end{ 0.f };

	// last update time in miliseconds
	double tree_find_time = 0.0;
	double linear_find_time = 0.0;
	double raycast_collision_time = 0.0;
	double bottom_up_time = 0.0;
	double top_down_time = 0.0;

	// object picking / selection
	std::vector<Object*> selected;
};

#endif