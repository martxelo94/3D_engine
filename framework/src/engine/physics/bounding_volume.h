/**
* @file bounding_volume.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define bounding volume creators and BVH
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H


enum BVType {
	bv_no = -1, bv_aabb_from_points = 0, bv_aabb_from_obb, bv_obb,
	bv_sphere_centroid, bv_sphere_ritter, bv_sphere_iterative,
	bv_sphere_pca,
	bv_type_count
};



// from local points
AABB compute_bv_aabb_from_points(const vec3* points, const size_t point_count);
// from transformed points
AABB compute_bv_aabb_from_points(const mat4& transform, const vec3* points, const size_t point_count);
// from local aabb transformed
AABB compute_bv_aabb_from_obb(const mat4& transform, const vec3* points, const size_t point_count);
AABB compute_bv_aabb_from_obb(const mat4& transform, const AABB& obb);

Sphere compute_bv_sphere_centroid(const mat4 &transform, const vec3* points, const size_t point_count);
Sphere compute_bv_sphere_ritter(const mat4 &transform, const vec3* points, const size_t point_count);
Sphere compute_bv_sphere_iterative(const mat4 &transform, const vec3* points, const size_t point_count);
Sphere compute_bv_sphere_pca(const mat4 &transform, const vec3* points, const size_t point_count);


#endif