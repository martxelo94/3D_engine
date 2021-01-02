/**
* @file test_bounding_volume_hierarchies.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/01	(eus)
* @brief Test bounding volume hierarchy
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>
#include <gtest\gtest.h>



namespace {
	/**
*
* @param os
* @param intersection
* @return
*/
	std::ostream& operator<<(std::ostream& os, const ::vec3& point)
	{
		os << point.x << " " << point.y << " " << point.z;
		return os;
	}

	/**
	*
	* @param os
	* @param intersection
	* @return
	*/
	std::ostream& operator<<(std::ostream& os, const ::AABB& aabb)
	{
		os << "aabb " << aabb.min_point << " " << aabb.max_point;
		return os;
	}



	/**
	*
	* @param stream
	* @param expect_tag
	* @return
	*/
	::vec3 read_point(std::istream& stream, bool expect_tag = false)
	{
		if (expect_tag) {
			std::string type;
			stream >> type;
			EXPECT_EQ(type, "point");
		}
		vec3 point;
		stream >> point[0];
		stream >> point[1];
		stream >> point[2];
		return point;
	}
	/**
*
* @param stream
* @return
*/
	::AABB read_aabb(std::istream& stream)
	{
		std::string type;

		// Plane normal
		stream >> type;
		EXPECT_EQ(type, "aabb");
		vec3 aabb_min = read_point(stream);
		vec3 aabb_max = read_point(stream);
		return { aabb_min, aabb_max };
	}


	/**
*
* @param mesh_data
*/
	void load_meshes(MeshData mesh_data[Demo_bvh::MeshType::mesh_type_count]) {
		load_mesh_obj("../resources/meshes/triangle.obj", nullptr, &mesh_data[Demo_bvh::mesh_triangle]);
		load_mesh_obj("../resources/meshes/sphere.obj", nullptr, &mesh_data[Demo_bvh::mesh_sphere]);
		load_mesh_obj("../resources/meshes/segment.obj", nullptr, &mesh_data[Demo_bvh::mesh_segment]);
		load_mesh_obj("../resources/meshes/quad.obj", nullptr, &mesh_data[Demo_bvh::mesh_quad]);
		load_mesh_obj("../resources/meshes/octohedron.obj", nullptr, &mesh_data[Demo_bvh::mesh_octohedron]);
		load_mesh_obj("../resources/meshes/icosahedron.obj", nullptr, &mesh_data[Demo_bvh::mesh_icosahedron]);
		load_mesh_obj("../resources/meshes/gourd.obj", nullptr, &mesh_data[Demo_bvh::mesh_gourd]);
		load_mesh_obj("../resources/meshes/cylinder.obj", nullptr, &mesh_data[Demo_bvh::mesh_cylinder]);
		load_mesh_obj("../resources/meshes/cube.obj", nullptr, &mesh_data[Demo_bvh::mesh_cube]);
		load_mesh_obj("../resources/meshes/bunny.obj", nullptr, &mesh_data[Demo_bvh::mesh_bunny]);
	}
	/**
*
* @param MIN
* @param MAX
* @param index
* @param mesh_data
* @return
*/
	Object add_random_object(int MIN, int MAX, int index, MeshData* mesh_data)
	{
		Object o;
		o.set_name("Object_" + std::to_string(index));
		o.set_pos(vec3{ glm::linearRand(MIN, MAX), 0.f, glm::linearRand(MIN, MAX) });
		o.set_scale(vec3{ glm::linearRand(0.5f, 5.f) });
		o.set_rot(vec3{ glm::linearRand(-M_PI, M_PI), glm::linearRand(-M_PI, M_PI),glm::linearRand(-M_PI, M_PI) });
		int rand_mesh_type = glm::linearRand(0, (int)Demo_bvh::mesh_type_count - 1);
		o.set_mesh_data(&mesh_data[rand_mesh_type]);

		// update bv
		o.get_aabb();

		return o;
	}
	/**
*
* @param stream
* @param obj 
*/
	void write_object(std::ostream& stream, const Object& obj) {
		stream << "obj ";
		stream << obj.get_name();
		stream << std::endl;
		// transform
		stream << "point " << obj.get_pos();
		stream << "point " << obj.get_scale();
		stream << "point " << obj.get_rot();
		stream << std::endl;
		// aabb
		stream << obj.get_aabb();
		stream << std::endl;
	}
	/**
*
* @param filepath
* @return
*/
	std::vector<Object> read_objects(std::string filepath) {
		std::vector<Object> objects;
		std::ifstream file(filepath);

		assert(file.is_open());

		std::string word;
		while (file.eof() == false) {
			file >> word;
			if (word == "obj") {
				Object o;
				file >> word;	// name
				o.set_name(word);
				// transform
				o.set_pos(read_point(file, true));
				o.set_scale(read_point(file, true));
				o.set_rot(read_point(file, true));
				// aabb
				o.set_aabb(read_aabb(file));
				objects.push_back(o);
			}
		}

		file.close();

		return objects;
	}

#if 0
	TEST(bv_hierarchy, create_random_objects_once)
	{
		// LOAD DATA
		MeshData mesh_data[MeshType::mesh_type_count];
		load_meshes(mesh_data);
		std::vector<Object> objects;
		int boundary_MAX = 10, boundary_MIN = -10;
		int OBJ_COUNT = 100;
		objects.reserve(OBJ_COUNT);
		for (int i = 0; i < OBJ_COUNT; ++i) {
			objects.push_back(add_random_object(boundary_MIN, boundary_MAX, i, mesh_data));
		}
		// write to file 
		std::string filepath = "../tests/bounding_volume_hierarchy/random_objects_";
		filepath.append(std::to_string(OBJ_COUNT));
		std::ofstream file;
		file.open(filepath, std::ios::out | std::ios::binary);
		ASSERT_TRUE(file.is_open());
		for (auto o : objects)
			write_object(file, o);
		
		EXPECT_EQ(1, 1);

		file.close();
	}
#else

	TEST(bv_hierarchy, bottom_up_contained_100)
	{
		auto objects = read_objects("../tests/bounding_volume_hierarchy/random_objects_100");
		// create bvh
		bounding_volume_hierarchy bvh;
		// create pointer array
		std::vector<const Object*> objects_ptr; objects_ptr.reserve(objects.size());
		for (auto &o : objects)
			objects_ptr.push_back(&o);
		// build BOTTOM UP
		bvh.build_bottom_up(objects_ptr);
		// check containment
		auto is_contained = [](const BVH::node& n) {
			// if bv added but surface area same as parent, contained
			float surf_area = n.bounding_volume.surface_area();
			for (auto c : n.children) {
				if (c) {
					const AABB& ab0 = n.bounding_volume;
					const AABB& ab1 = c->bounding_volume;
					AABB ab = AABB{ glm::min(ab0.min_point, ab1.min_point),
						 glm::max(ab0.max_point, ab1.max_point) };
					float c_surf = ab.surface_area();

					EXPECT_EQ(surf_area, c_surf);
				}
			}
		};

		bvh.traverse_preorder(is_contained);
	}
	TEST(bv_hierarchy, top_down_contained_1000)
	{
		auto objects = read_objects("../tests/bounding_volume_hierarchy/random_objects_1000");
		// create bvh
		bounding_volume_hierarchy bvh;
		// create pointer array
		std::vector<const Object*> objects_ptr; objects_ptr.reserve(objects.size());
		for (auto &o : objects)
			objects_ptr.push_back(&o);
		// build TOP DOWN
		bvh.build_top_down(objects_ptr);
		// check containment
		auto is_contained = [](const BVH::node& n) {
			// if bv added but surface area same as parent, contained
			float surf_area = n.bounding_volume.surface_area();
			for (auto c : n.children) {
				if (c) {
					const AABB& ab0 = n.bounding_volume;
					const AABB& ab1 = c->bounding_volume;
					AABB ab = AABB{ glm::min(ab0.min_point, ab1.min_point),
						 glm::max(ab0.max_point, ab1.max_point) };
					float c_surf = ab.surface_area();

					EXPECT_EQ(surf_area, c_surf);
				}
			}
		};
		bvh.traverse_preorder(is_contained);
	}
	TEST(bv_hierarchy, insertion_contained_1000)
	{
		auto objects = read_objects("../tests/bounding_volume_hierarchy/random_objects_1000");
		// create bvh
		bounding_volume_hierarchy bvh;
		// build INSERTION
		for (auto &o : objects)
			bvh.add_object(o);
		// check containment
		auto is_contained = [](const BVH::node& n) {
			// if bv added but surface area same as parent, contained
			float surf_area = n.bounding_volume.surface_area();
			for (auto c : n.children) {
				if (c) {
					const AABB& ab0 = n.bounding_volume;
					const AABB& ab1 = c->bounding_volume;
					AABB ab = AABB{ glm::min(ab0.min_point, ab1.min_point),
						 glm::max(ab0.max_point, ab1.max_point) };
					float c_surf = ab.surface_area();

					EXPECT_EQ(surf_area, c_surf);
				}
			}
		};
		bvh.traverse_preorder(is_contained);
	}

#endif
}