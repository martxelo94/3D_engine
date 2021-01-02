#ifndef DEMO_KDTREE_H
#define DEMO_KDTREE_H


struct Demo_kdtree : public Demo
{
	Demo_kdtree();		// initialize stuff
	~Demo_kdtree();	// free stuff

	void update()override;
	void render()const override;

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

	Object& add_object();			// default object at origin
	Object& add_random_object(int MIN = -50, int MAX = 50);	// random object

private:
	// object storage (is a list so I don't lose pointers on growth)
	std::list<Object> objects;

	// KD-TREE
	kdtree kd_tree;
	bool kdtree_draw_aabbs = false;
	kdtree::config kdtree_config;
	std::vector<int> kdtree_draw_nodes; // use them like booleans


	ImGuizmo::OPERATION guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;


	// Raycast
	vec3 debug_ray_start{ 0.f }, debug_ray_end{ 0.f };

	// last update time in miliseconds
	double bv_update_time = 0.0;
	double raycast_collision_time = 0.0;

	// object picking / selection
	std::vector<Object*> selected;
};

#endif