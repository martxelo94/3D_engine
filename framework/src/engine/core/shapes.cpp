/**
* @file shapes.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement geometrical shapes
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>


Triangle::Triangle(const vec3& _p0, const vec3& _p1, const vec3& _p2)
	: p0(_p0), p1(_p1), p2(_p2)
{}
Triangle::Triangle(const std::array<vec3, 3>& _p)
	: Triangle(_p[0], _p[1], _p[2])
{}
Plane::Plane(const vec3& _normal, const vec3& _point)
	: normal(_normal), dot_result(glm::dot(_normal, _point))
{
	assert(!glm::epsilonEqual(glm::length2(_normal), 0.f, FLT_EPSILON));
}
Plane::Plane(const vec3 &p0, const vec3 &p1, const vec3 &p2)
	: normal({0.f}), dot_result(0.f)
{
	vec3 ab = p1 - p0;
	vec3 ac = p2 - p0;
	vec3 n = glm::cross(ab, ac);
	if (glm::epsilonEqual(glm::dot(n, n), 0.f, cEpsilon))
		return;
	normal = glm::normalize(n);
	dot_result = glm::dot(normal, p0);
}
Plane::Plane(const Triangle &tri)
	: Plane(tri.p0, tri.p1, tri.p2)
{}
/**
*
* @return
*/
vec3 Plane::get_point() const
{
	float len2 = glm::length2(normal);
	return normal * dot_result / len2;
}
Ray::Ray(const vec3& _start, const vec3& _dir)
	: start(_start), dir(_dir)
{
	assert(!glm::epsilonEqual(glm::length2(_dir), 0.f, FLT_EPSILON));
}
Sphere::Sphere(const vec3& _center, float _radius)
	: center(_center), radius(_radius)
{
	assert(_radius > 0.f);
}
Frustum::Frustum(const std::array<Plane, 6>& _planes)
	: planes(_planes)
{}

Frustum::Frustum(const mat4& view_proj_mtx)
{
	vec3 ndc[8] = {
		{1, 1, -1}, {1, -1, -1}, {-1, -1, -1}, {-1, 1, -1},	// near
		{1, 1, 1}, {1, -1, 1}, {-1, -1, 1}, {-1, 1, 1}	// far
	};
	mat4 inv_mtx = glm::inverse(view_proj_mtx);
	for (int i = 0; i < 8;  ++i) {
		vec4 p = inv_mtx * vec4{ndc[i], 1};
		ndc[i] = p / p.w;
	}
	vec3 plane_midpoints[6] = {
		{(ndc[0] + ndc[1] + ndc[4] + ndc[5]) / 4.f},	// +X
		{(ndc[3] + ndc[2] + ndc[7] + ndc[6]) / 4.f},	// -X
		{(ndc[0] + ndc[3] + ndc[4] + ndc[7]) / 4.f},	// +Y
		{(ndc[1] + ndc[2] + ndc[6] + ndc[5]) / 4.f},	// -Y
		{(ndc[0] + ndc[1] + ndc[2] + ndc[3]) / 4.f},	// +Z
		{(ndc[4] + ndc[5] + ndc[6] + ndc[7]) / 4.f}		// -Z
	};
	// construct planes from triangle
	planes[0] = Plane({ plane_midpoints[0], ndc[4], ndc[5] });	// +X
	planes[1] = Plane({ plane_midpoints[1], ndc[3], ndc[2] });	// -X
	planes[2] = Plane({ plane_midpoints[2], ndc[7], ndc[4] });	// +Y
	planes[3] = Plane({ plane_midpoints[3], ndc[5], ndc[6] });	// -Y
	planes[4] = Plane({ plane_midpoints[4], ndc[0], ndc[1] });	// +Z
	planes[5] = Plane({ plane_midpoints[5], ndc[7], ndc[6] });	// -Z
	
}
/**
*
* @return
*/
std::vector<vec3> Frustum::get_points() const 
{
	std::vector<vec3> points(8, vec3{});

	// {1, 1, 1}
	points[0] = intersection_3_planes(planes[4], planes[0], planes[2]);
	// {1, -1, 1}
	points[1] = intersection_3_planes(planes[4], planes[0], planes[3]);
	// {-1, -1, 1}
	points[2] = intersection_3_planes(planes[4], planes[1], planes[3]);
	// {-1, 1, 1}
	points[3] = intersection_3_planes(planes[4], planes[1], planes[2]);
	// {1, 1, -1}
	points[4] = intersection_3_planes(planes[5], planes[0], planes[2]);
	// {1, -1, -1}
	points[5] = intersection_3_planes(planes[5], planes[0], planes[3]);
	// {-1, -1, -1}
	points[6] = intersection_3_planes(planes[5], planes[1], planes[3]);
	// {-1, 1, -1}
	points[7] = intersection_3_planes(planes[5], planes[1], planes[2]);

	return points;

}


