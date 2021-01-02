#include <gtest/gtest.h>
#include <pch.h>


#define EXPECT_FLOAT_EQ_DECS(a, b) EXPECT_FLOAT_EQ(glm::floor(100.0f * (a))/100.0f, glm::floor(100.0f * (b))/100.0f)
#define ASSERT_FLOAT_EQ_DECS(a, b) ASSERT_FLOAT_EQ(glm::floor(100.0f * (a))/100.0f, glm::floor(100.0f * (b))/100.0f)

/**
*
* @param stream
* @return
*/
bool read_boolean(std::istream& stream)
{
	std::string type;
	stream >> type;
	return type == "true";
}

/**
*
* @param stream
* @return
*/
float read_epsilon(std::istream& stream)
{
	float       epsilon     = 0.001f;
	auto        current_pos = stream.tellg();
	std::string type;
	stream >> type;
	if (type == "epsilon"){
		stream >> epsilon;
	}
	else{
		if (!stream.eof()){
			stream.seekg(current_pos);
		}
	}
	return epsilon;
}

/**
*
* @param stream
* @param expect_tag
* @return
*/
::vec3 read_point(std::istream& stream, bool expect_tag = false)
{
	if (expect_tag){
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
* @param expect_tag
* @return
*/
::vec2 read_point_2d(std::istream& stream, bool expect_tag = false)
{
	if (expect_tag){
		std::string type;
		stream >> type;
		EXPECT_EQ(type, "point");
	}
	vec2 point;
	stream >> point[0];
	stream >> point[1];
	return point;
}

/**
*
* @param stream
* @return
*/
::Plane read_plane(std::istream& stream, bool expect_tag = true)
{


	// Plane normal
	if (expect_tag){
		std::string type;
		stream >> type;
		EXPECT_EQ(type, "plane");
	}
	vec3 plane_normal = read_point(stream);
	vec3 plane_point  = read_point(stream);

	return {plane_normal, plane_point};
}

/**
*
* @param stream
* @return
*/
Triangle read_triangle(std::istream& stream)
{
	std::string type;

	// Plane normal
	stream >> type;
	EXPECT_EQ(type, "triangle");
	return {
			{
					read_point(stream, false),
					read_point(stream, false),
					read_point(stream, false)
			}
	};
}

/**
*
* @param stream
* @return
*/
::Ray read_ray(std::istream& stream)
{
	std::string type;

	// Plane normal
	stream >> type;
	EXPECT_EQ(type, "ray");
	vec3 start = read_point(stream);
	vec3 dir   = read_point(stream);

	return {start, dir};
}

/**
*
* @param stream
* @return
*/
::Sphere read_sphere(std::istream& stream)
{
	std::string type;

	// Plane normal
	stream >> type;
	EXPECT_EQ(type, "sphere");
	vec3  sphere_center = read_point(stream);
	float sphere_radius = 0.0f;
	stream >> sphere_radius;

	return {sphere_center, sphere_radius};
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
	return {aabb_min, aabb_max};
}

/**
*
* @param stream
* @return
*/
::Frustum read_frustum(std::istream& stream)
{
	std::string type;

	// Plane normal
	stream >> type;
	EXPECT_EQ(type, "frustum");
	std::array<Plane, 6> planes = {};
	for (auto& p : planes){
		p = read_plane(stream, false);
	}
	return {planes};
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
	for (int i = 0; i < point_count; ++i){
		result.push_back(read_point(stream, true));
	}
	return result;
}

/**
 *
 * @param stream
 * @return
 */
std::vector<vec2> read_points_2d(std::istream& stream)
{
	std::vector<vec2> result;
	int               point_count = 0;
	stream >> point_count;
	for (int i = 0; i < point_count; ++i){
		result.push_back(read_point_2d(stream, true));
	}
	return result;
}

/**
 *
 * @param stream
 * @return
 */
mat4 read_mat4(std::istream& stream)
{
	mat4 result{};

	std::string type;

	// Plane normal
	stream >> type;
	EXPECT_EQ(type, "mat4");

	for (int j = 0; j < 4; ++j){
		for (int i = 0; i < 4; ++i){
			stream >> result[i][j];
		}
	}

	return result;
}

/**
*
* @param stream
* @return
*/
::intersection_type read_intersection_type(std::ifstream& stream)
{
	std::string intersection;
	stream >> intersection;
	if (intersection == "inside"){
		return ::intersection_type::INSIDE;
	}
	else if (intersection == "outside"){
		return ::intersection_type::OUTSIDE;
	}
	else if (intersection == "overlaps"){
		return ::intersection_type::OVERLAPS;
	}
	else if (intersection == "coplanar"){
		return ::intersection_type::COPLANAR;
	}
	else{
		throw std::runtime_error("Invalid intersection type");
	}
}

/**
 * Checks if all points lie inside (epsilon applied for surface points)
 * @param points
 * @param sph
 * @return
 */
bool are_points_inside(const std::vector<vec3>& points, const AABB& sph)
{
	for (const auto pt : points){
		if (intersection_point_aabb(pt, sph) != intersection_type::INSIDE){
			return false;
		}
	}
	return true;
}

/**
 * Checks if all points lie inside (epsilon applied for surface points)
 * @param points
 * @param sph
 * @return
 */
bool are_points_inside(const std::vector<vec3>& points, Sphere sph)
{
	sph.radius += cEpsilon;
	for (const auto pt : points){
		if (intersection_point_sphere(pt, sph) != intersection_type::INSIDE){
			return false;
		}
	}
	return true;
}

/**
 *
 */
std::vector<AABB> read_aabbs(std::istream& stream)
{
	std::vector<AABB> ret;
	// Read the number of aabbs
	int               count = 0;
	stream >> count;
	for (int i = 0; i < count; ++i){
		ret.push_back(read_aabb(stream));
	}
	return ret;
}

/**
*
* @param os
* @param intersection
* @return
*/
std::ostream& operator<<(std::ostream& os, const ::intersection_type& intersection)
{
	switch (intersection){
		case intersection_type::INSIDE: os << "inside";
			break;
		case intersection_type::OVERLAPS: os << "overlaps";
			break;
		case intersection_type::OUTSIDE: os << "outside";
			break;
		case intersection_type::COPLANAR: os << "coplanar";
			break;
	}
	return os;
}

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
* @param os
* @param intersection
* @return
*/
std::ostream& operator<<(std::ostream& os, const ::Plane& plane)
{
	os << "plane(" << plane.normal << "," << plane.dot_result << ")";
	return os;
}

namespace{
	TEST(geometry, in_plane_point)
	{
		std::ifstream file("../tests/geometry/in_plane_point", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			const auto  plane             = read_plane(file);
			const auto  point             = read_point(file, true);
			const auto  intersection_type = read_intersection_type(file);
			const float epsilon           = read_epsilon(file);
			EXPECT_EQ(intersection_point_plane(point, plane, epsilon), intersection_type) << "[Line " << line << "]" "With:" << point << " vs " << plane;
		}
	}

	TEST(geometry, in_project_point_plane)
	{
		std::ifstream file("../tests/geometry/in_project_point_plane", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			const auto plane      = read_plane(file);
			const auto point      = read_point(file, true);
			const auto projection = read_point(file, true);
			const auto result     = project_point_plane(point, plane);
			EXPECT_NEAR(result[0], projection[0], 0.01f) << "[Line " << line << "]";
			EXPECT_NEAR(result[1], projection[1], 0.01f) << "[Line " << line << "]";
			EXPECT_NEAR(result[2], projection[2], 0.01f) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_barycentric_line)
	{
		std::ifstream file("../tests/geometry/in_barycentric_line", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  point_a = read_point(file, true);
			const auto  point_b = read_point(file, true);

			float u = 0.0f;
			file >> placeholder;
			EXPECT_EQ(placeholder, "u");
			file >> u;
			file >> placeholder;
			EXPECT_EQ(placeholder, "v");
			float v = 0.0f;
			file >> v;
			const bool expected    = read_boolean(file);

			EXPECT_NEAR(u + v, 1.0f, 0.05f) << "[Line " << line << "]With: [" << point_a << "," << point_b << "] vs [" << u << "," << v << "]";
			// Test
			const auto point       = point_a * u + point_b * v;
			vec2       barycentric = {u, v};
			EXPECT_EQ(get_barycentric_coordinates(point_a, point_b, point, &barycentric), expected) << "[Line " << line << "]With: [" << point_a << "," << point_b << "] vs [" << u << "," << v << "]";
			EXPECT_NEAR(barycentric.x, u, 0.05f) << "[Line " << line << "]With: [" << point_a << "," << point_b << "] vs [" << u << "," << v << "]";
			EXPECT_NEAR(barycentric.y, v, 0.05f) << "[Line " << line << "]With: [" << point_a << "," << point_b << "] vs [" << u << "," << v << "]";
		}
	}

	TEST(geometry, in_barycentric_triangle)
	{
		std::ifstream file("../tests/geometry/in_barycentric_triangle", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  point_a = read_point(file, true);
			const auto  point_b = read_point(file, true);
			const auto  point_c = read_point(file, true);

			float u = 0.0f;
			file >> placeholder;
			EXPECT_EQ(placeholder, "u");
			file >> u;
			file >> placeholder;
			EXPECT_EQ(placeholder, "v");
			float v = 0.0f;
			file >> v;
			file >> placeholder;
			EXPECT_EQ(placeholder, "w");
			float w = 0.0f;
			file >> w;

			const bool expected    = read_boolean(file);

			EXPECT_NEAR(u + v + w, 1.0f, 0.05f) << "[Line " << line << "]";
			// Test
			const auto point       = point_a * u + point_b * v + point_c * w;
			vec3       barycentric = {u, v, w};
			EXPECT_EQ(get_barycentric_coordinates({point_a, point_b, point_c}, point, &barycentric), expected) << "[Line " << line << "]";
			EXPECT_NEAR(barycentric.x, u, 0.05f) << "[Line " << line << "]";
			EXPECT_NEAR(barycentric.y, v, 0.05f) << "[Line " << line << "]";
			EXPECT_NEAR(barycentric.z, w, 0.05f) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_sphere_point)
	{
		std::ifstream file("../tests/geometry/in_sphere_point", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  sphere   = read_sphere(file);
			const auto  point    = read_point(file, true);
			const auto  expected = read_intersection_type(file);
			EXPECT_EQ(intersection_point_sphere(point, sphere), expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_aabb_point)
	{
		std::ifstream file("../tests/geometry/in_aabb_point", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  aabb     = read_aabb(file);
			const auto  point    = read_point(file, true);
			const auto  expected = read_intersection_type(file);
			EXPECT_EQ(intersection_point_aabb(point, aabb), expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_ray_plane)
	{
		std::ifstream file("../tests/geometry/in_ray_plane", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  ray      = read_ray(file);
			const auto  plane    = read_plane(file);
			float       expected = 0.0f;
			file >> expected;
			const float intersection_time = intersection_ray_plane(ray, plane);
			EXPECT_FLOAT_EQ(intersection_time, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_ray_triangle)
	{
		std::ifstream file("../tests/geometry/in_ray_triangle", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  ray      = read_ray(file);
			const auto  triangle = read_triangle(file);
			float       expected = 0.0f;
			file >> expected;
			const float intersection_time = intersection_ray_triangle(ray, triangle);
			EXPECT_FLOAT_EQ_DECS(intersection_time, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_ray_sphere)
	{
		std::ifstream file("../tests/geometry/in_ray_sphere", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  ray      = read_ray(file);
			const auto  sphere   = read_sphere(file);
			float       expected = 0.0f;
			file >> expected;
			const float intersection_time = intersection_ray_sphere(ray, sphere);
			EXPECT_FLOAT_EQ_DECS(intersection_time, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_ray_aabb)
	{
		std::ifstream file("../tests/geometry/in_ray_aabb", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  ray      = read_ray(file);
			const auto  aabb     = read_aabb(file);
			float       expected = 0.0f;
			file >> expected;
			const float intersection_time = intersection_ray_aabb(ray, aabb);
			EXPECT_FLOAT_EQ_DECS(intersection_time, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_plane_triangle)
	{
		std::ifstream file("../tests/geometry/in_plane_triangle", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  plane    = read_plane(file);
			const auto  triangle = read_triangle(file);
			const auto  epsilon  = read_epsilon(file);
			const auto  expected = read_intersection_type(file);
			const auto  result   = intersection_plane_triangle(plane, triangle, epsilon);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_plane_sphere)
	{
		std::ifstream file("../tests/geometry/in_plane_sphere", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  plane    = read_plane(file);
			const auto  sphere   = read_sphere(file);
			const auto  expected = read_intersection_type(file);
			const auto  result   = intersection_plane_sphere(plane, sphere);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_plane_aabb)
	{
		std::ifstream file("../tests/geometry/in_plane_aabb", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  plane    = read_plane(file);
			const auto  aabb     = read_aabb(file);
			const auto  expected = read_intersection_type(file);
			const auto  result   = intersection_plane_aabb(plane, aabb);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_frustum_triangle)
	{
		std::ifstream file("../tests/geometry/in_frustum_triangle", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  frustum  = read_frustum(file);
			const auto  triangle = read_triangle(file);
			const auto  expected = read_intersection_type(file);
			const auto  result   = intersection_frustum_triangle(frustum, triangle);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_frustum_aabb)
	{
		std::ifstream file("../tests/geometry/in_frustum_aabb", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  frustum  = read_frustum(file);
			const auto  aabb     = read_aabb(file);
			const auto  expected = read_intersection_type(file);
			const auto  result   = intersection_frustum_aabb(frustum, aabb);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_frustum_sphere)
	{
		std::ifstream file("../tests/geometry/in_frustum_sphere", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			std::string placeholder;
			const auto  frustum  = read_frustum(file);
			const auto  sphere   = read_sphere(file);
			const auto  expected = read_intersection_type(file);
			const auto  result   = intersection_frustum_sphere(frustum, sphere);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
	}

	TEST(geometry, in_sphere_sphere)
	{
#if 0	// desactivated since return type changed to more usefull type
		std::ifstream file("../tests/geometry/in_sphere_sphere", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			const auto sphere_a = read_sphere(file);
			const auto sphere_b = read_sphere(file);
			const auto expected = read_boolean(file);
			const auto result   = intersection_sphere_sphere(sphere_a, sphere_b);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
#endif
	}

	TEST(geometry, in_aabb_aabb)
	{
#if 0	// desactivated since return type changed to more usefull type
		std::ifstream file("../tests/geometry/in_aabb_aabb", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()){
			line++;
			const auto aabb_a   = read_aabb(file);
			const auto aabb_b   = read_aabb(file);
			const auto expected = read_boolean(file);
			const auto result   = intersection_aabb_aabb(aabb_a, aabb_b);
			EXPECT_EQ(result, expected) << "[Line " << line << "]";
		}
#endif
	}

	TEST(geometry, plane_from_triangle)
	{
		std::ifstream file("../tests/geometry/in_plane_triangle", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()) {
			line++;
			std::string placeholder;
			// most of the read file is unused
			const auto  plane = read_plane(file);
			const auto  triangle = read_triangle(file);
			const auto  epsilon = read_epsilon(file);
			const auto  expected = read_intersection_type(file);
			
			// input triangle is coplanar with generated plane
			::Plane pl(triangle.p0, triangle.p1, triangle.p2);
			intersection_type result = intersection_plane_triangle(pl, triangle, epsilon);

			EXPECT_EQ(result, intersection_type::COPLANAR) << "[Line " << line << "]";
		}
	}

	TEST(geometry, frustum_points)
	{
		std::ifstream file("../tests/geometry/in_frustum_aabb", std::ios::in);
		ASSERT_TRUE(file.is_open());

		int line = 0;
		while (!file.eof()) {
			line++;
			std::string placeholder;
			const auto  frustum = read_frustum(file);
			const auto  aabb = read_aabb(file);
			const auto  expected = read_intersection_type(file);
			
			const auto points = frustum.get_points();

			// each point OVERLAPS 3 planes exactly
			EXPECT_EQ(points.size(), 8);
			for (int i = 0; i < 8; ++i) {
				int coplanars = 0;
				for (int j = 0; j < 6; ++j) {
					intersection_type result = intersection_point_plane(points[i], frustum.planes[j], cEpsilon);
					if (result == OVERLAPS)
						coplanars++;
				}
				EXPECT_EQ(coplanars, 3);
			}
		}
	}

	TEST(geometry, frustum_from_matrix)
	{
		float near_plane = 1.f;
		float far_plane = 100.f;
		mat4 view = glm::lookAt(vec3{ 0, 0, 100 }, vec3{ 0, 0, 0 }, vec3{ 0, 1, 0 });
		mat4 proj = glm::perspective(glm::radians(60.f), 16.f / 9.f, near_plane, far_plane);

		Frustum f(proj * view);

		// oposite planes inside
		for (int i = 0; i < 6; i += 2) {
			intersection_type res0 = intersection_point_plane(f.planes[i].get_point(), f.planes[i + 1]);
			intersection_type res1 = intersection_point_plane(f.planes[i + 1].get_point(), f.planes[i]);
			int res = res0 & res1;
			EXPECT_EQ(res, INSIDE) << "at planes " << i;
		}

		// get distance from near to far plane
		vec3 near_p = f.planes[Frustum::Z_minus].get_point();
		vec3 far_p = f.planes[Frustum::Z_plus].get_point();
		float dist = glm::distance(near_p, far_p);
		EXPECT_EQ(far_plane - near_plane, dist);
	}
}