/**
* @file bounding_volume.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement bounding volume creators
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>

namespace {
	/**
*
* @param sph
* @param pt
*/
	void sphere_of_sphere_and_pt(Sphere &sph, const vec3 &pt)
	{
		vec3 diff = pt - sph.center;
		float dist2 = glm::dot(diff, diff);
		// only update sph if point p is outside it
		if (dist2 > sph.radius * sph.radius) {
			float dist = sqrt(dist2);
			float new_radius = (sph.radius + dist) * 0.5f;
			float t = (new_radius - sph.radius) / dist;
			sph.radius = new_radius;
			sph.center += diff * t;
		}
	}
	/**
*
* @param _aabb
*/
	void increment_epsilon(AABB &_aabb, float epsilon_mul)
	{
		_aabb.min_point -= vec3{ cEpsilon * epsilon_mul};
		_aabb.max_point += vec3{ cEpsilon * epsilon_mul};
	}
	/**
*
* @param sph
*/
	void increment_epsilon(Sphere &sph, float epsilon_mul)
	{
		sph.radius += cEpsilon * epsilon_mul;
	}
	// Give the user the option to fit th BV perfectly (this makes some tests to fail)
#define BV_TIGHT 0
#if BV_TIGHT
#define INC_EPSILON(bv)
#else
#define INC_EPSILON(bv, epsilon_mul) increment_epsilon(bv, epsilon_mul);
#endif
}
/**
*
* @param points
* @param point_count
* @return
*/
AABB compute_bv_aabb_from_points(const vec3* points, const size_t point_count
)
{
	vec3 min_p = vec3{ FLT_MAX }, max_p = vec3{ -FLT_MAX };

	for (int i = 0; i < point_count; ++i) {
		const vec3 &p = points[i];
		// x
		if (p.x > max_p.x) max_p.x = p.x;
		if (p.x < min_p.x) min_p.x = p.x;
		// y
		if (p.y > max_p.y) max_p.y = p.y;
		if (p.y < min_p.y) min_p.y = p.y;
		// z
		if (p.z > max_p.z) max_p.z = p.z;
		if (p.z < min_p.z) min_p.z = p.z;
	}
	AABB ab = AABB{ min_p, max_p };

	//INC_EPSILON(ab, 1);
	
	return ab;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
AABB compute_bv_aabb_from_points(const mat4& transform, const vec3* points, const size_t point_count
)
{
	vec3 min_p = vec3{ FLT_MAX }, max_p = vec3{ -FLT_MAX };

	for (int i = 0; i < point_count; ++i) {
		const vec3 p = transform * vec4{ points[i], 1.f };
		// x
		if (p.x > max_p.x) max_p.x = p.x;
		if (p.x < min_p.x) min_p.x = p.x;
		// y
		if (p.y > max_p.y) max_p.y = p.y;
		if (p.y < min_p.y) min_p.y = p.y;
		// z
		if (p.z > max_p.z) max_p.z = p.z;
		if (p.z < min_p.z) min_p.z = p.z;
	}
	AABB ab = AABB{ min_p, max_p };

	INC_EPSILON(ab, 1);

	return ab;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
AABB compute_bv_aabb_from_obb(const mat4& transform, const vec3* points, const size_t point_count)
{
	AABB ab = compute_bv_aabb_from_points(points, point_count);
	// transform aabb
	const vec3& MAX = ab.max_point;
	const vec3& MIN = ab.min_point;
	std::array<vec3, 8> p = { MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };
	for (int i = 0; i < 8; ++i) {
		p[i] = transform * vec4{ p[i], 1.f };
	}
	return compute_bv_aabb_from_points(mat4(1.f), p.data(), p.size());
}
/**
*
* @param transform
* @param obb
* @return
*/
AABB compute_bv_aabb_from_obb(const mat4& transform, const AABB& obb) 
{
	// transform aabb
	const vec3& MAX = obb.max_point;
	const vec3& MIN = obb.min_point;
	std::array<vec3, 8> p = { MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };
	for (int i = 0; i < 8; ++i) {
		p[i] = transform * vec4{ p[i], 1.f };
	}
	return compute_bv_aabb_from_points(mat4(1.f), p.data(), p.size());
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_centroid(const mat4 &transform, const vec3* points, const size_t point_count)
{
	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// compute midpoint
	vec3 midpoint = vec3{ 0.f };
	for (const vec3& pos : p)
		midpoint += pos;
	midpoint /= p.size();
	// find fartest vertex
	float max_dist2 = FLT_MIN;
	for (const vec3& pos : p) {
		float dist2 = glm::distance2(pos, midpoint);
		if (dist2 > max_dist2)
			max_dist2 = dist2;
	}
	Sphere sph = Sphere{ midpoint, sqrt(max_dist2) };

	INC_EPSILON(sph, 10);

	return sph;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_ritter(const mat4 &transform, const vec3* points, const size_t point_count)
{
	// 1st pass: Get sphere encompassing two approximately most distant points

	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// most separated points on aabb
	int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	for (int i = 0; i < point_count; ++i) {
		if (p[i].x < p[minx].x) minx = i;
		if (p[i].x > p[maxx].x) maxx = i;
		if (p[i].y < p[miny].y) miny = i;
		if (p[i].y > p[maxy].y) maxy = i;
		if (p[i].z < p[minz].z) minz = i;
		if (p[i].z > p[maxz].z) maxz = i;
	}
	float dist2x = glm::distance2(p[maxx], p[minx]);
	float dist2y = glm::distance2(p[maxy], p[miny]);
	float dist2z = glm::distance2(p[maxz], p[minz]);
	// pick the pair (min, max) of points most distant
	int minp = minx;
	int maxp = maxx;
	if (dist2y > dist2x && dist2y > dist2z) {
		maxp = maxy; minp = miny;
	}
	if (dist2z > dist2x && dist2z > dist2y) {
		maxp = maxz; minp = minz;
	}
	// set up sphere to just encompass these two points
	Sphere sph;
	sph.center = (p[minp] + p[maxp]) * 0.5f;
	sph.radius = glm::distance(p[maxp], sph.center);

	// 2nd pass: Grow sphere to include all points
	for (int i = 0; i < point_count; ++i) {
		// compute squared distance between point and sphere center
		sphere_of_sphere_and_pt(sph, p[i]);
	}

	INC_EPSILON(sph, 10);

	return sph;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_iterative(const mat4 &transform, const vec3* points, const size_t point_count)
{
	const int NUM_ITER = 20;

	// Ritter Sphere

	// 1st pass: Get sphere encompassing two approximately most distant points

	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// most separated points on aabb
	int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	for (int i = 0; i < point_count; ++i) {
		if (p[i].x < p[minx].x) minx = i;
		if (p[i].x > p[maxx].x) maxx = i;
		if (p[i].y < p[miny].y) miny = i;
		if (p[i].y > p[maxy].y) maxy = i;
		if (p[i].z < p[minz].z) minz = i;
		if (p[i].z > p[maxz].z) maxz = i;
	}
	float dist2x = glm::distance2(p[maxx], p[minx]);
	float dist2y = glm::distance2(p[maxy], p[miny]);
	float dist2z = glm::distance2(p[maxz], p[minz]);
	// pick the pair (min, max) of points most distant
	int minp = minx;
	int maxp = maxx;
	if (dist2y > dist2x && dist2y > dist2z) {
		maxp = maxy; minp = miny;
	}
	if (dist2z > dist2x && dist2z > dist2y) {
		maxp = maxz; minp = minz;
	}
	// set up sphere to just encompass these two points
	Sphere sph;
	sph.center = (p[minp] + p[maxp]) * 0.5f;
	sph.radius = glm::distance(p[maxp], sph.center);

	// 2nd pass: Grow sphere to include all points
	for (int i = 0; i < point_count; ++i) {
		// only update sph if point p is outside it
		sphere_of_sphere_and_pt(sph, p[i]);
	}
	Sphere sph_tmp = sph;

	for (int j = 0; j < NUM_ITER; ++j) {
		// shrink sphere somewhat to make it an underestimate (not bound) // THIS FAILED ON TESTS!!!
		sph_tmp.radius *= 1.0f;
		// make sphere bound data again
		for (int i = 0; i < point_count - 1; ++i) {
			int random_idx = glm::linearRand(i + 1, (int)point_count - 1);
			std::swap(p[i], p[random_idx]);
			// compute squared distance between point and sphere center
			sphere_of_sphere_and_pt(sph_tmp, p[i]);
		}
		// update sph whenever a tighter sphere is found
		if (sph_tmp.radius < sph.radius)
			sph = sph_tmp;
	}

	INC_EPSILON(sph, 10);

	return sph;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_pca(const mat4 &transform, const vec3* points, const size_t point_count)
{
	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// 1st pass: find pair most separated
	int minp = 0, maxp = 0;
	float max_dist2 = FLT_MIN;
	for (int i = 0; i < point_count - 1; ++i) {
		for (int j = i + 1; j < point_count; ++j) {
			float dist2 = glm::distance2(p[i], p[j]);
			if (dist2 > max_dist2) {
				max_dist2 = dist2;
				minp = i;
				maxp = j;
			}
		}
	}
	vec3 diff = p[maxp] - p[minp];
	Sphere sph = { p[minp] + diff * 0.5f, sqrt(max_dist2) * 0.5f };

	// 2nd pass: Grow sphere to include all points
	for (int i = 0; i < point_count; ++i) {
		// only update sph if point p is outside it
		sphere_of_sphere_and_pt(sph, p[i]);
	}

	INC_EPSILON(sph, 20);

	return sph;
}


