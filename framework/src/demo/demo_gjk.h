#ifndef DEMO_GJK_H
#define DEMO_GJK_H


struct Demo_gjk : public Demo
{
	Demo_gjk();		// initialize stuff
	~Demo_gjk();	// free stuff

	void update()override;
	void render()const override;

	// ------ declare data here -------

	// camera stuff
	Camera camera;
	vec2 mouse_velocity = { 100.f, 100.f };
	float camera_axis_speed = 5.f;

	enum MeshType {
		mesh_no = -1, mesh_sphere = 0, mesh_octohedron, mesh_icosahedron, mesh_cylinder,
		mesh_cube, mesh_type_count
	};
	// meshes
	MeshBuffers mesh_buffer[mesh_type_count];
	MeshData mesh_data[mesh_type_count];

private:
	// object storage (is a list so I don't lose pointers on growth)
	Object obj0, obj1;

	// Simplex
	std::vector<gjk_simplex> m_simplices;
	bool draw_simplex = true;
	int m_steps = 0;

	bool draw_minkowski = true;

	bool hulls_colliding = false;

	bool rotate_randomly = false;
	float rot_change_timer = 0.f;
	vec3 rand_rot_0 = {}, rand_rot_1 = {};


	std::vector<vec3> minkowski_points;
};

#endif