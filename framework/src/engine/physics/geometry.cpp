/**
* @file geometry.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement geometrical functions
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>

/**
*
* @param p
* @param ab
* @return
*/
intersection_type intersection_point_aabb(const vec3& p, const AABB& ab) {
	// check x
	if (ab.max_point.x < p.x || p.x < ab.min_point.x)
		return OUTSIDE;
	// chekc y
	if (ab.max_point.y < p.y || p.y < ab.min_point.y)
		return OUTSIDE;
	// check z
	if (ab.max_point.z < p.z || p.z < ab.min_point.z)
		return OUTSIDE;

	return intersection_type::INSIDE;
}
/**
*
* @param p
* @param ab
* @param m
* @return
*/
intersection_type intersection_point_obb(const vec3& p, const AABB& ab, const mat4& m) {
	vec3 transform_p = glm::inverse(m) * vec4 { p, 1.f };
	return intersection_point_aabb(transform_p, ab);
}
/**
*
* @param p
* @param sph
* @return
*/
intersection_type intersection_point_sphere(const vec3& p, const Sphere& sph) {
	if (glm::length2(p - sph.center) <= sph.radius * sph.radius)
		return INSIDE;
	return OUTSIDE;
}
/**
*
* @param p
* @param pl
* @param epsilon
* @return
*/
intersection_type intersection_point_plane(const vec3& p, const Plane& pl, float epsilon) {
	vec3 pl_p = pl.get_point();
	vec3 v = p - pl_p;
	float d = glm::dot(v, pl.normal);
	if (glm::epsilonEqual(d, 0.f, epsilon))
		return intersection_type::OVERLAPS;
	if (d > 0.f)
		return intersection_type::INSIDE;
	return intersection_type::OUTSIDE;
}
/**
*
* @param p
* @param pl
* @return
*/
vec3 project_point_plane(const vec3& p, const Plane& pl) {
	vec3 pl_p = pl.get_point();
	vec3 v = p - pl_p;
	float d = glm::dot(v, pl.normal);

	return p - pl.normal * d;
}
/**
*
* @param point_a
* @param point_b
* @param p
* @param barycentric_coord
* @return
*/
bool get_barycentric_coordinates(const vec3& point_a, const vec3& point_b, const vec3& p, vec2* barycentric_coord) {
	assert(barycentric_coord);
	vec3 ab = point_b - point_a;
	vec3 ap = p - point_a;
	// check lengths
	float ab_len2 = glm::length2(ab);
	///assert(ab_len2 > 0.f && "Input a valid line, please!");
	float ap_len2 = glm::length2(ap);
	if (glm::epsilonEqual(ab_len2, 0.f, cEpsilon)) {
		// a == b... so what? degenerate!
		barycentric_coord->x = 0.f;
		barycentric_coord->y = 1.f;
		return false;
	}
	if (glm::epsilonEqual(ap_len2, 0.f, cEpsilon)) {
		//p == a
		barycentric_coord->x = 1.f;
		barycentric_coord->y = 0.f;
		return true;
	}
	float t = glm::dot(ap, ab);
	// normalize t
	t /= ab_len2;
	float s = 1.f - t;
	barycentric_coord->x = s;
	barycentric_coord->y = t;
	return t >= 0.f && t <= 1.f;
}
/**
*
* @param tri
* @param p
* @param barycentric_coord
* @return
*/
bool get_barycentric_coordinates(const Triangle& tri, const vec3& p, vec3* barycentric_coord) {
	assert(barycentric_coord);
	vec3 ab = tri.p1 - tri.p0;
	vec3 ac = tri.p2 - tri.p0;
	vec3 ap = p - tri.p0;
	// check if p is in triangle plane
	vec3 n = glm::cross(ab, ac);
	float d = glm::dot(ap, n);	// remember to return if the original point was on the plane or not
	vec3 pl_p = p - n * d;	// get coordinates from this
	// get barycentric coordinates s, t
	float d_ab_ac = glm::dot(ab, ac);
	ap = pl_p - tri.p0;
	float ab2 = glm::dot(ab, ab);
	float ac2 = glm::dot(ac, ac);

	float divisor = ab2 * ac2 - d_ab_ac * d_ab_ac;
	// check degenerate triangle
	if (glm::epsilonEqual(divisor, 0.f, cEpsilon))
		return false;

	float d_ap_ab = glm::dot(ap, ab);
	float d_ap_ac = glm::dot(ap, ac);
	
	float s = (ac2 * d_ap_ab - d_ab_ac * d_ap_ac) / divisor;
	float t = (ab2 * d_ap_ac - d_ab_ac * d_ap_ab) / divisor;
	float u = 1.f - s - t;
	barycentric_coord->x = u;
	barycentric_coord->y = s;
	barycentric_coord->z = t;

	assert(glm::epsilonEqual(s + t + u, 1.f, cEpsilon));

	// over the plane and inside
	return glm::epsilonEqual(d, 0.f, cEpsilon) && u >= 0.f && s >= 0.f && t >= 0.f;
}
/**
*
* @param r
* @param pl
* @return
*/
float intersection_ray_plane(const Ray& r, const Plane& pl) {
	// check that ray is not coplanar
	float d = glm::dot(pl.normal, r.dir);
	if (glm::epsilonEqual(d, 0.f, cEpsilon)) 
		return -1.f;

	float t = glm::dot(pl.get_point() - r.start, pl.normal) / d;
	return t >= 0.f? t : -1.f;
}
/**
*
* @param r
* @param tri
* @return
*/
float intersection_ray_triangle(const Ray& r, const Triangle& tri) {
	// find intersection with plane
	float t = intersection_ray_plane(r, Plane{tri.p0, tri.p1, tri.p2});
	// check if intersection is inside triangle
	vec3 barycentric_coord;
	bool inside = get_barycentric_coordinates(tri, r.start + t * r.dir, &barycentric_coord);

	return inside ? t : -1.f;
}
/**
*
* @param r
* @param sph
* @return
*/
float intersection_ray_sphere(const Ray& r, const Sphere& sph) {
	// second degree equation problem [ax2 + bx + c = 0]
	vec3 dif = sph.center - r.start;	// this was start - center in the slides, by the way...
	float a = glm::length2(r.dir);
	float b = 2.f * glm::dot(r.dir, dif);
	float c = glm::length2(dif) - sph.radius * sph.radius;
	// check if system has solution
	// [sqrt(discriminant); discriminant = b2 - 4ac >= 0]
	float discriminant = b * b - 4.f * a * c;
	if (discriminant < 0)
		return -1.f;
	float divisor = 2.f * a;
	assert(divisor != 0.f);
	// one solution
	if (glm::epsilonEqual(discriminant, 0.f, cEpsilon))
		return b / divisor;
	float sqrt_discriminant = sqrtf(discriminant);
	float t0 = (b + sqrt_discriminant) / divisor;
	float t1 = (b - sqrt_discriminant) / divisor;
	// two positive solutions
	if (t0 > 0.f && t1 > 0.f)
		return glm::min(t0, t1);
	// two negative solutions
	if (t0 < 0.f && t1 < 0.f)
		return -1.f;	// returning the closest intersection with the line would give more information
	// one positive solution: ray starts inside sphere
	return 0.f; // arbitrarily return 0 when ray originates inside
	/*
	// This should be the correct way!

	float t_min = glm::min(t0, t1);
	float t_max = glm::max(t0, t1);

	return t_min > 0.f ? t_min : t_max;
	*/
}
/**
*
* @param r
* @param ab
* @return
*/
float intersection_ray_aabb(const Ray& r, const AABB& ab) {
	// return 0 if start inside aabb...
	if (intersection_point_aabb(r.start, ab) == intersection_type::INSIDE)
		return 0.f;
	// check foreach axis
	float tmax = std::numeric_limits<float>::max(), tmin = -tmax;
	for (int i = 0; i < 3; ++i) {
		if (r.dir[i] != 0.f) {
			float inv_dir = 1.f / r.dir[i];

			float t0 = (ab.min_point[i] - r.start[i]) * inv_dir;
			float t1 = (ab.max_point[i] - r.start[i]) * inv_dir;
			if (inv_dir < 0.f)
				std::swap(t0, t1);
			tmin = glm::max(tmin, glm::min(t0, t1));
			tmax = glm::min(tmax, glm::max(t0, t1));
		}
		else if (r.start[i] < ab.min_point[i] || r.start[i] > ab.max_point[i])
			return -1.f;
	}
	

	return (tmin <= tmax) && (tmax > 0)? tmin : -1.f;
}
/**
*
* @param r
* @param ab
* @param m
* @return
*/
float intersection_ray_obb(const Ray& r, const AABB& ab, const mat4 &m) {
	mat4 inv_m = glm::inverse(m);
	Ray r_model_space;
	r_model_space.start = inv_m * vec4{ r.start, 1.f };
	r_model_space.dir = inv_m * vec4{ r.dir, 0.f };
	return intersection_ray_aabb(r_model_space, ab);
}
/**
*
* @param r
* @param points
* @param p_count
* @param indices
* @param i_count
* @return
*/
float intersection_ray_mesh(const Ray& r, const mat4& model_mtx, const vec3* points, const tri_idx* indices, tri_idx i_count) {
	float closest_t = FLT_MAX;
	// brutefoce traverse every triangle
	for (tri_idx i = 0; i < i_count; i += 3) {
		// compose triangle
		const vec3 p0 = model_mtx * vec4{ points[indices[i]] , 1.f };
		const vec3 p1 = model_mtx * vec4{ points[indices[i + 1]], 1.f };
		const vec3 p2 = model_mtx * vec4{ points[indices[i + 2]], 1.f };

		Triangle tri{ p0, p1, p2 };
		float t = intersection_ray_triangle(r, tri);
		if (t >= 0.f && closest_t > t) {
			closest_t = t;
		}
	}
	if (closest_t == FLT_MAX)
		return -1.f;
	return closest_t;
}
/**
*
* @param pl
* @param tri
* @param epsilon
* @return
*/

intersection_type intersection_plane_triangle(const Plane& pl, const Triangle& tri, float epsilon) {
	int result_flag = 0;
	result_flag |= intersection_point_plane(tri.p0, pl, epsilon);
	result_flag |= intersection_point_plane(tri.p1, pl, epsilon);
	result_flag |= intersection_point_plane(tri.p2, pl, epsilon);

	if (result_flag == 5 || result_flag == 1)
		return INSIDE;
	if (result_flag == 2 ||result_flag == 6)
		return OUTSIDE;
	if (result_flag == 4)
		return COPLANAR;
	return OVERLAPS;

}
/**
*
* @param pl
* @param sph
* @return
*/
intersection_type intersection_plane_sphere(const Plane& pl, const Sphere& sph) {
	// get distance from sphere center to plane
	vec3 pc = sph.center - pl.get_point();
	float d = glm::dot(pc, pl.normal);
	vec3 dif = pl.normal * d;
	float len2 = glm::length2(dif);
	// if dif lenght > R, sphere doesn't touch
	if (len2 > sph.radius * sph.radius) {
		// if d > 0, OUTSIDE. Otherwise inside
		if (d > 0) return intersection_type::INSIDE;
		return intersection_type::OUTSIDE;
	}
	return intersection_type::OVERLAPS;
}
/**
*
* @param pl
* @param ab
* @return
*/
intersection_type intersection_plane_aabb(const Plane& pl, const AABB& ab) {
	const vec3& MAX = ab.max_point;
	const vec3& MIN = ab.min_point;
	vec3 p[8] = {	MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };
	//check foreach point, but first set the intersection_type with the first point
	intersection_type prev_result = intersection_point_plane(p[0], pl, cEpsilon);
	if (prev_result == intersection_type::OVERLAPS)
		return prev_result;
	for (int i = 1; i < 8; ++i) {
		intersection_type result = intersection_point_plane(p[i], pl, cEpsilon);
		if (result != prev_result)
			return intersection_type::OVERLAPS;
		prev_result = result;
	}
	return prev_result;
}
/**
*
* @param f
* @param tri
* @return
*/
intersection_type intersection_frustum_triangle(const Frustum& f, const Triangle& tri) {

	// Points Outside Of Planes
	int result_flag = 0;
	// check each triangle foreach plane
	for (int i = 0; i < 6; ++i) {
		intersection_type result = intersection_plane_triangle(f.planes[i], tri, cEpsilon);
		result_flag |= (int)result;
	}
	
	if ((result_flag & 2) == 2)
		return OUTSIDE;
	if (result_flag == 1)
		return INSIDE;
	return OVERLAPS;
#if 0	// this could handle corner cases...
	// check if any triangle's edge intersects frustum
	ray rays[3] = { 
		ray{tri.p0, tri.p1 - tri.p0},
		ray{tri.p1, tri.p2 - tri.p1},
		ray{tri.p2, tri.p0 - tri.p2}
	};
	for (int i = 0; i < 3; ++i) {
		// planes +X, -X, +Y, -Y, +Z, -Z
		for (int j = 0; j < 6; ++j) {
			float t = intersection_ray_plane(rays[i], f.planes[j]);
			if (t >= 0) {
				bool p_outside = false;
				// intersects plane, check if point is on frustum
				vec3 p_ray = rays[i].start + rays[i].dir * t;
				// loop in two parts, in order to avoid checking with same axis planes
				for (int k = 0; k < 2 * int(j / 2); ++k) {
					intersection_type result = intersection_point_plane(p_ray, f.planes[k], cEpsilon);
					if (result == OUTSIDE) {
						p_outside = true;
						break;
					}
				}
				if (p_outside)
					continue;	// check next plane
				for (int k = 2 * int(j / 2) + 2; k < 6; ++k) {
					intersection_type result = intersection_point_plane(p_ray, f.planes[k], cEpsilon);
					if (result == OUTSIDE) {
						p_outside = true;
						break;
					}
				}
				if (!p_outside)
					return OVERLAPS;
			}
		}
	}
	return intersection_type::OUTSIDE;
#endif
}
/**
*
* @param f
* @param ab
* @return
*/
intersection_type intersection_frustum_aabb(const Frustum& f, const AABB& ab) {
	const vec3& MAX = ab.max_point;
	const vec3& MIN = ab.min_point;
	// since plane_aabb constructs these points, we do point_plane in order to construct 8 points only once...
	vec3 p[8] = { MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };
	int result_flag = 0;
	// check each point foreach plane
	for (int i = 0; i < 6; ++i) {
		int plane_result_flag = 0;
		for (int j = 0; j < 8; ++j) {
			intersection_type result = intersection_point_plane(p[j], f.planes[i], cEpsilon);
			plane_result_flag |= (int)result;
		}
		// if INSIDE plane, is outside frustum, since normals point outwards
		if (plane_result_flag == OUTSIDE)
			return OUTSIDE;
		result_flag |= plane_result_flag;
	}
	if (result_flag == INSIDE)
		return INSIDE;
	return OVERLAPS;

#if 0	// this could handle corner cases...
	// early discard
	if (std::all_of(poop, poop + 8, [poop](int x) { return x == poop[0]; })) {
		if (poop[0] == -1)
			return intersection_type::INSIDE;
		return intersection_type::OUTSIDE;
	}
	// check if any aabb's edge intersects frustum
	ray rays[12] = {
		ray{p[0], p[1] - p[0]}, ray{p[1], p[2] - p[1]}, ray{p[2], p[3] - p[2]}, ray{p[3], p[0] - p[3]},
		ray{p[4], p[5] - p[4]}, ray{p[5], p[6] - p[5]}, ray{p[6], p[7] - p[6]}, ray{p[7], p[4] - p[7]},
		ray{p[0], p[6] - p[0]}, ray{p[3], p[5] - p[3]}, ray{p[2], p[4] - p[2]}, ray{p[1], p[7] - p[1]}
	};
	for (int i = 0; i < 12; ++i) {
		// planes +X, -X, +Y, -Y, +Z, -Z
		for (int j = 0; j < 6; ++j) {
			float t = intersection_ray_plane(rays[i], f.planes[j]);
			if (t >= 0) {
				bool p_outside = false;
				// intersects plane, check if point is on frustum
				vec3 p_ray = rays[i].start + rays[i].dir * t;
				// loop in two parts, in order to avoid checking with same axis planes
				for (int k = 0; k < 2 * int(j / 2); ++k) {
					intersection_type result = intersection_point_plane(p_ray, f.planes[k], cEpsilon);
					if (result == OUTSIDE) {
						p_outside = true;
						break;
					}
				}
				if (p_outside)
					continue;	// check next plane
				for (int k = 2 * int(j / 2) + 2; k < 6; ++k) {
					intersection_type result = intersection_point_plane(p_ray, f.planes[k], cEpsilon);
					if (result == OUTSIDE) {
						p_outside = true;
						break;
					}
				}
				if (!p_outside)
					return OVERLAPS;
			}
		}
	}
	return intersection_type::OUTSIDE;
#endif
}
/**
*
* @param f
* @param sph
* @return
*/
intersection_type intersection_frustum_sphere(const Frustum& f, const Sphere& sph) {

	int result_flag = 0;
	// check if sphere is outside ANY plane
	for (int i = 0; i < 6; ++i) {
		intersection_type result = intersection_plane_sphere(f.planes[i], sph);
		result_flag |= (int)result;
		if (result == OUTSIDE)
			return OUTSIDE;
	}
	// check if is totally inside
	if (result_flag == 1)
		return INSIDE;

	return intersection_type::OVERLAPS;
}
/**
*
* @param s0
* @param s1
* @return
*/
intersection_type intersection_sphere_sphere(const Sphere& s0, const Sphere& s1) {
	float dist2 = glm::distance2(s0.center, s1.center);
	float r2 = s0.radius + s1.radius;
	r2 *= r2;
	float r3 = abs(s0.radius - s1.radius);
	r3 *= r3;
	// check if one is inside another
	return dist2 > r2? OUTSIDE: dist2 < r3? INSIDE : OVERLAPS;
}
/**
*
* @brief check if ab0 is in, out or intersects ab1
* @param ab0
* @param ab1
* @return
*/
intersection_type intersection_aabb_aabb(const AABB& ab0, const AABB& ab1) {
	intersection_type results[3] = {OVERLAPS, OVERLAPS, OVERLAPS };
	for (int i = 0; i < 3; ++i) {
		if (ab0.max_point[i] < ab1.min_point[i] || ab0.min_point[i] > ab1.max_point[i])
			results[i] = OUTSIDE;
		if (ab0.min_point[i] > ab1.min_point[i] && ab0.max_point[i] < ab1.max_point[i])
			results[i] = INSIDE;
	}
	int result = results[0] | results[1] | results[2];
	if ((result & OUTSIDE) == OUTSIDE)
		return OUTSIDE;
	if ((result & OVERLAPS) == OVERLAPS)
		return OVERLAPS;
	return INSIDE;
}
/**
*
* @param p0
* @param p1
* @param p2
* @return
*/
vec3 intersection_3_planes(const Plane& p0, const Plane& p1, const Plane& p2) {
	vec3 A{ p0.normal.x, p1.normal.x, p2.normal.x};
	vec3 B{ p0.normal.y, p1.normal.y, p2.normal.y };
	vec3 C{ p0.normal.z, p1.normal.z, p2.normal.z };
	vec3 D{ -p0.dot_result, -p1.dot_result, -p2.dot_result };
	
	mat3 m{ A, B, C };
	float det = glm::determinant(m);
	// if determinant = 0, no solution
	assert(det != 0.f);
	det = -det;
	
	m = {D, B, C};
	float x = glm::determinant(m) / det;
	m = {A, D, C};
	float y = glm::determinant(m) / det;
	m = { A, B, D };
	float z = glm::determinant(m) / det;

	return vec3{x, y, z};
}
