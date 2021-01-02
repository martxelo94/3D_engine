/**
* @file geometry.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define geometrical functions
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef GEOMETRY_H
#define GEOMETRY_H

// collision Epsilon
#define cEpsilon (FLT_EPSILON * 10.f)


enum intersection_type { INSIDE = 1, OUTSIDE = 2, OVERLAPS = 4, COPLANAR = 8 };

intersection_type intersection_point_aabb(const vec3& p, const AABB& ab);
intersection_type intersection_point_obb(const vec3& p, const AABB& ab, const mat4& m);
intersection_type intersection_point_sphere(const vec3& p, const Sphere& sph);
intersection_type intersection_point_plane(const vec3& p, const Plane& pl, float epsilon = FLT_EPSILON);
vec3 project_point_plane(const vec3& p, const Plane& pl);
bool get_barycentric_coordinates(const vec3& point_a, const vec3& point_b, const vec3& p, vec2* barycentric_coord);	// line coordinates
bool get_barycentric_coordinates(const Triangle& tri, const vec3& p, vec3* barycentric_coord);						// triangle coordinates
float intersection_ray_plane(const Ray& r, const Plane& pl);
float intersection_ray_triangle(const Ray& r, const Triangle& tri);
float intersection_ray_sphere(const Ray& r, const Sphere& sph);
float intersection_ray_aabb(const Ray& r, const AABB& ab);
float intersection_ray_obb(const Ray& r, const AABB& ab, const mat4 &m);
float intersection_ray_mesh(const Ray& r, const mat4& model_mtx, const vec3* points, const tri_idx* indices, tri_idx i_count);
intersection_type intersection_plane_triangle(const Plane& pl, const Triangle& tri, float epsilon = FLT_EPSILON);
intersection_type intersection_plane_sphere(const Plane& pl, const Sphere& sph);
intersection_type intersection_plane_aabb(const Plane& pl, const AABB& ab);
intersection_type intersection_frustum_triangle(const Frustum& f, const Triangle& tri);
intersection_type intersection_frustum_aabb(const Frustum& f, const AABB& ab);
intersection_type intersection_frustum_sphere(const Frustum& f, const Sphere& sph);
intersection_type intersection_sphere_sphere(const Sphere& s0, const Sphere& s1);
intersection_type intersection_aabb_aabb(const AABB& ab0, const AABB& ab1);


// only used for cases where 3 planes can intersect in a point, like in frustum case
vec3 intersection_3_planes(const Plane& p0, const Plane& p1, const Plane& p2);


#endif