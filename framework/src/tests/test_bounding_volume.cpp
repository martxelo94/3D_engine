/**
* @file test_bounding_volume.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/01	(eus)
* @brief Define bounding volume creators
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <gtest\gtest.h>
#include <pch.h>



namespace {
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
	std::vector<vec3> read_points(std::istream& stream)
	{
		std::vector<vec3> result;
		int               point_count = 0;
		stream >> point_count;
		for (int i = 0; i < point_count; ++i) {
			result.push_back(read_point(stream, true));
		}
		return result;
	}
	/**
*
* @param m
* @param points
* @param point_count
* @param _aabb
* @return
*/
	u32 intersection_mesh_aabb(const mat4 &m, const vec3 *points, size_t point_count, const AABB &_aabb){
		u32 intersections = 0;
		for (int i = 0; i < point_count; ++i) {
			vec3 p = m * vec4{ points[i], 1.f };
			intersection_type result = intersection_point_aabb(p, _aabb);
			if (result != INSIDE)
				intersections++;
		}
		return intersections;
	}
	/**
*
* @param m
* @param points
* @param point_count
* @param _aabb
* @param obb_mtx
* @return
*/
	u32 intersection_mesh_obb(const mat4 &m, const vec3 *points, size_t point_count, const AABB &_aabb, const mat4 &obb_mtx) {
		u32 intersections = 0;
		for (int i = 0; i < point_count; ++i) {
			vec3 p = m * vec4{ points[i], 1.f };
			intersection_type result = intersection_point_obb(p, _aabb, obb_mtx);
			if (result != INSIDE)
				intersections++;
		}
		return intersections;
	}
	/**
*
* @param m
* @param points
* @param point_count
* @param sph
* @return
*/
	u32 intersection_mesh_sphere(const mat4 &m, const vec3 *points, size_t point_count, const Sphere &sph) {
		u32 intersections = 0;
		for (int i = 0; i < point_count; ++i) {
			vec3 p = m * vec4{ points[i], 1.f };
			intersection_type result = intersection_point_sphere(p, sph);
			if (result != INSIDE) {
				printf("in_mesh_sphere failed by %f\n", glm::length(p - sph.center) - sph.radius);
				intersections++;
			}
		}
		return intersections;
	}
	/**
*
* @param type
* @return
*/
	std::string mesh_type_to_string(Demo_bvh::Demo_bvh::MeshType type) {
		switch (type)
		{
		case Demo_bvh::mesh_no:
			return "NO";
		case Demo_bvh::mesh_triangle:
			return "triangle";
		case Demo_bvh::mesh_sphere:
			return "sphere";
		case Demo_bvh::mesh_segment:
			return "segment";
		case Demo_bvh::mesh_quad:
			return "quad";
		case Demo_bvh::mesh_octohedron:
			return "octohedron";
		case Demo_bvh::mesh_icosahedron:
			return "icosahedron";
		case Demo_bvh::mesh_gourd:
			return "gourd";
		case Demo_bvh::mesh_cylinder:
			return "cylinder";
		case Demo_bvh::mesh_cube:
			return "cube";
		case Demo_bvh::mesh_bunny:
			return "bunny";
		default:
			break;
		}
		assert(0);

		return "";
	}
	/**
*
* @param mesh_data
*/
	void load_meshes(MeshData mesh_data[Demo_bvh::Demo_bvh::MeshType::mesh_bunny + 1]) {
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
* @param points
* @return
*/
	void load_random_points(std::vector<vec3> & points) {
		std::ifstream file("../tests/bounding_volumes/in_bv_point", std::ios::in);
		ASSERT_TRUE(file.is_open());
		while (!file.eof()) {
			const auto  point = read_point(file, true);
			points.push_back(point);
		}
	}

	TEST(bounding_volumes, mesh_in_aabb_from_points)
	{
		// LOAD DATA
		MeshData mesh_data[Demo_bvh::Demo_bvh::MeshType::mesh_bunny + 1];
		load_meshes(mesh_data);
		std::vector<vec3> rand_points;
		load_random_points(rand_points);
		// arbitrary transform (from the random processes of my neocortex)
		vec3 pos = { 100, -65, 13 };
		vec3 sc{ 23.f };
		vec3 rot{ 10, -7, 90 };
		mat4 transform = glm::translate(pos) * glm::toMat4(glm::quat(rot)) * glm::scale(sc);

		AABB bv[Demo_bvh::Demo_bvh::MeshType::mesh_bunny + 1];

		u32 intersections = 0;
		for (int i = 0; i < Demo_bvh::Demo_bvh::MeshType::mesh_bunny + 1; ++i) {
			const auto& points = mesh_data[i].positions;
			bv[i] = compute_bv_aabb_from_points(transform, points.data(), points.size());
			// TESTING
			intersections = intersection_mesh_aabb(transform, points.data(), points.size(), bv[i]);
			EXPECT_EQ(0u, intersections)
				<< "Failed aabb_from_points, " << intersections << "points outside. Mesh " << mesh_type_to_string((Demo_bvh::MeshType)i);
		}
		// random points
		AABB bv_rand = compute_bv_aabb_from_points(transform, rand_points.data(), rand_points.size());
		intersections = intersection_mesh_aabb(transform, rand_points.data(), rand_points.size(), bv_rand);
		EXPECT_EQ(0u, intersections)
			<< "Failed aabb_from_points, " << intersections << "points outside. Mesh RANDOM";
		
	}

	TEST(bounding_volumes, mesh_in_aabb_from_obb)
	{
		// LOAD DATA
		MeshData mesh_data[Demo_bvh::MeshType::mesh_bunny + 1];
		load_meshes(mesh_data);
		std::vector<vec3> rand_points;
		load_random_points(rand_points);
		// arbitrary transform (from the random processes of my neocortex)
		vec3 pos = { 100, -65, 13 };
		vec3 sc{ 23.f };
		vec3 rot{ 10, -7, 90 };
		mat4 transform = glm::translate(pos) * glm::toMat4(glm::quat(rot)) * glm::scale(sc);

		u32 intersections = 0;
		for (int i = 0; i < Demo_bvh::MeshType::mesh_bunny + 1; ++i) {
			const auto& points = mesh_data[i].positions;
			AABB bv = compute_bv_aabb_from_obb(transform, points.data(), points.size());
			// TESTING
			intersections = intersection_mesh_aabb(transform, points.data(), points.size(), bv);
			EXPECT_EQ(0u, intersections)
				<< "Failed aabb_from_obb, " << intersections << "points outside. Mesh " << mesh_type_to_string((Demo_bvh::MeshType)i);
		}
		// random points
		AABB bv_rand = compute_bv_aabb_from_obb(transform, rand_points.data(), rand_points.size());
		intersections = intersection_mesh_aabb(transform, rand_points.data(), rand_points.size(), bv_rand);
		EXPECT_EQ(0u, intersections)
			<< "Failed aabb_from_obb, " << intersections << "points outside. Mesh RANDOM";

	}

	TEST(bounding_volumes, mesh_in_obb)
	{
		// LOAD DATA
		MeshData mesh_data[Demo_bvh::MeshType::mesh_bunny + 1];
		load_meshes(mesh_data);
		std::vector<vec3> rand_points;
		load_random_points(rand_points);
		// arbitrary transform (from the random processes of my neocortex)
		vec3 pos = { 100, -65, 13 };
		vec3 sc{ 23.f };
		vec3 rot{ 10, -7, 90 };
		mat4 transform = glm::translate(pos) * glm::toMat4(glm::quat(rot)) * glm::scale(sc);

		u32 intersections = 0;
		for (int i = 0; i < Demo_bvh::MeshType::mesh_bunny + 1; ++i) {
			const auto& points = mesh_data[i].positions;
			AABB bv = compute_bv_aabb_from_points(mat4(1.f), points.data(), points.size());
			// TESTING
			intersections = intersection_mesh_obb(transform, points.data(), points.size(), bv, transform);
			EXPECT_EQ(0u, intersections)
				<< "Failed obb, " << intersections << "points outside. Mesh " << mesh_type_to_string((Demo_bvh::MeshType)i);
		}
		// random points
		AABB bv_rand = compute_bv_aabb_from_points(mat4(1.f), rand_points.data(), rand_points.size());
		intersections = intersection_mesh_obb(transform, rand_points.data(), rand_points.size(), bv_rand, transform);
		EXPECT_EQ(0u, intersections)
			<< "Failed obb, " << intersections << "points outside. Mesh RANDOM";

	}

	TEST(bounding_volumes, mesh_in_sphere_centroid)
	{
		// LOAD DATA
		MeshData mesh_data[Demo_bvh::MeshType::mesh_bunny + 1];
		load_meshes(mesh_data);
		std::vector<vec3> rand_points;
		load_random_points(rand_points);
		// arbitrary transform (from the random processes of my neocortex)
		vec3 pos = { 100, -65, 13 };
		vec3 sc{ 23.f };
		vec3 rot{ 10, -7, 90 };
		mat4 transform = glm::translate(pos) * glm::toMat4(glm::quat(rot)) * glm::scale(sc);

		u32 intersections = 0;
		for (int i = 0; i < Demo_bvh::MeshType::mesh_bunny + 1; ++i) {
			const auto& points = mesh_data[i].positions;
			Sphere bv = compute_bv_sphere_centroid(transform, points.data(), points.size());
			// TESTING
			intersections = intersection_mesh_sphere(transform, points.data(), points.size(), bv);
			EXPECT_EQ(0u, intersections)
				<< "Failed sphere_centroid, " << intersections << "points outside. Mesh " << mesh_type_to_string((Demo_bvh::MeshType)i);
		}
		// random points
		Sphere bv_rand = compute_bv_sphere_centroid(transform, rand_points.data(), rand_points.size());
		intersections = intersection_mesh_sphere(transform, rand_points.data(), rand_points.size(), bv_rand);
		EXPECT_EQ(0u, intersections)
			<< "Failed sphere_centroid, " << intersections << "points outside. Mesh RANDOM";

	}

	TEST(bounding_volumes, mesh_in_sphere_ritter)
	{
		// LOAD DATA
		MeshData mesh_data[Demo_bvh::MeshType::mesh_bunny + 1];
		load_meshes(mesh_data);
		std::vector<vec3> rand_points;
		load_random_points(rand_points);
		// arbitrary transform (from the random processes of my neocortex)
		vec3 pos = { 100, -65, 13 };
		vec3 sc{ 23.f };
		vec3 rot{ 10, -7, 90 };
		mat4 transform = glm::translate(pos) * glm::toMat4(glm::quat(rot)) * glm::scale(sc);

		u32 intersections = 0;
		for (int i = 0; i < Demo_bvh::MeshType::mesh_bunny + 1; ++i) {
			const auto& points = mesh_data[i].positions;
			Sphere bv = compute_bv_sphere_ritter(transform, points.data(), points.size());
			// TESTING
			intersections = intersection_mesh_sphere(transform, points.data(), points.size(), bv);
			EXPECT_EQ(0u, intersections)
				<< "Failed sphere_ritter, " << intersections << "points outside. Mesh " << mesh_type_to_string((Demo_bvh::MeshType)i);
		}
		// random points
		Sphere bv_rand = compute_bv_sphere_ritter(transform, rand_points.data(), rand_points.size());
		intersections = intersection_mesh_sphere(transform, rand_points.data(), rand_points.size(), bv_rand);
		EXPECT_EQ(0u, intersections)
			<< "Failed sphere_ritter, " << intersections << "points outside. Mesh RANDOM";

	}

	TEST(bounding_volumes, mesh_in_sphere_iterative)
	{
		// LOAD DATA
		MeshData mesh_data[Demo_bvh::MeshType::mesh_bunny + 1];
		load_meshes(mesh_data);
		std::vector<vec3> rand_points;
		load_random_points(rand_points);
		// arbitrary transform (from the random processes of my neocortex)
		vec3 pos = { 100, -65, 13 };
		vec3 sc{ 23.f };
		vec3 rot{ 10, -7, 90 };
		mat4 transform = glm::translate(pos) * glm::toMat4(glm::quat(rot)) * glm::scale(sc);

		u32 intersections = 0;
		for (int i = 0; i < Demo_bvh::MeshType::mesh_bunny + 1; ++i) {
			const auto& points = mesh_data[i].positions;
			Sphere bv = compute_bv_sphere_iterative(transform, points.data(), points.size());
			// TESTING
			intersections = intersection_mesh_sphere(transform, points.data(), points.size(), bv);
			EXPECT_EQ(0u, intersections)
				<< "Failed sphere_iterative, " << intersections << "points outside. Mesh " << mesh_type_to_string((Demo_bvh::MeshType)i);
		}
		// random points
		Sphere bv_rand = compute_bv_sphere_iterative(transform, rand_points.data(), rand_points.size());
		intersections = intersection_mesh_sphere(transform, rand_points.data(), rand_points.size(), bv_rand);
		EXPECT_EQ(0u, intersections)
			<< "Failed sphere_iterative, " << intersections << "points outside. Mesh RANDOM";

	}

	TEST(bounding_volumes, mesh_in_sphere_pca)
	{
		// LOAD DATA
		MeshData mesh_data[Demo_bvh::MeshType::mesh_bunny + 1];
		load_meshes(mesh_data);
		std::vector<vec3> rand_points;
		load_random_points(rand_points);
		// arbitrary transform (from the random processes of my neocortex)
		vec3 pos = { 100, -65, 13 };
		vec3 sc{ 23.f };
		vec3 rot{ 10, -7, 90 };
		mat4 transform = glm::translate(pos) * glm::toMat4(glm::quat(rot)) * glm::scale(sc);

		u32 intersections = 0;
		for (int i = 0; i < Demo_bvh::MeshType::mesh_bunny + 1; ++i) {
			const auto& points = mesh_data[i].positions;
			Sphere bv = compute_bv_sphere_pca(transform, points.data(), points.size());
			// TESTING
			intersections = intersection_mesh_sphere(transform, points.data(), points.size(), bv);
			EXPECT_EQ(0u, intersections)
				<< "Failed sphere_pca, " << intersections << "points outside. Mesh " << mesh_type_to_string((Demo_bvh::MeshType)i);
		}
		// random points
		Sphere bv_rand = compute_bv_sphere_pca(transform, rand_points.data(), rand_points.size());
		intersections = intersection_mesh_sphere(transform, rand_points.data(), rand_points.size(), bv_rand);
		EXPECT_EQ(0u, intersections)
			<< "Failed sphere_pca, " << intersections << "points outside. Mesh RANDOM";

	}
}