/**
* @file shapes.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define geometrical shapes
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef SHAPES_H
#define SHAPES_H


struct Triangle
{
#if NOT_NAMED_UNIONS
	union {
		struct {
		vec3 p0, p1, p2;
		};
		std::array<vec3, 3> p;
	};
#else
	vec3 p0, p1, p2;
#endif

	Triangle(const vec3& _p0, const vec3& _p1, const vec3& _p2);
	Triangle(const std::array<vec3, 3>& _p);
};
struct Plane
{
#if NOT_NAMED_UNIONS
	union {
		vec4 equation;
		struct {
		vec3 normal;
		float dot_result;
		};
	};
#else
	vec3 normal;
	float dot_result;
#endif

	Plane() = default;
	Plane(const vec3& _normal, const vec3& _point);
	Plane(const vec3 &p0, const vec3 &p1, const vec3 &p2);
	Plane(const Triangle &tri);

	vec3 get_point() const;
};
struct Ray
{
	vec3 start, dir;

	Ray() = default;
	Ray(const vec3& _start, const vec3& _dir);
};
// {center, radius)
struct Sphere
{
	vec3 center;
	float radius = 0.f;

	Sphere() = default;
	Sphere(const vec3& _center, float _radius);
};
// Axis Aligned Bounding Box
struct AABB
{
	vec3 min_point, max_point;

	inline float surface_area() const { return 2.f * glm::compAdd(max_point - min_point); }
	inline vec3 center() const { return (max_point - min_point) / 2.f + min_point; }
};
struct Frustum
{
	// enum to know the damed order of planes
	enum PlaneOrder { X_plus = 0, X_minus = 1, Y_plus = 2, Y_minus = 3, Z_plus = 4, Z_minus = 5};

	// define 6 planes
	std::array<Plane, 6> planes;

	Frustum(const std::array<Plane, 6>& _planes);
	Frustum(const mat4& view_proj_mtx);

	std::vector<vec3> get_points() const;
};


#endif
