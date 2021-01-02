/**
* @file object.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/22	(eus)
* @brief Implement object struct
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>


/**
*
* @param p
*/
void Object::set_pos(const vec3& p) {
	pos = p;
	is_transform_updated = is_aabb_updated = false;
}
/**
*
* @param _sc
*/
void Object::set_scale(const vec3& _sc) {
	sc = _sc;
	is_transform_updated = is_aabb_updated = false;
}
/**
*
* @param _rot
*/
void Object::set_rot(const vec3& _rot) {
	rot = _rot;
	is_transform_updated = is_aabb_updated = false;
}
/**
*
* @brief updates model matrix if necesary
* @return
*/
const mat4& Object::get_model() const {
	if (!is_transform_updated) {
		model_mtx = glm::translate(pos) * glm::toMat4(glm::quat{ rot }) * glm::scale(sc);
		is_transform_updated = true;
	}
	return model_mtx;
}

/**
*
* @brief updates obb
* @param md
*/
void Object::set_mesh_data(const MeshData* md) {
	// must recompute bv?
	if (md == mesh_data) return;
	mesh_data = md;
	if (md)
		obb = compute_bv_aabb_from_points(md->positions.data(), md->positions.size());
}
/**
*
* @param mb
*/
void Object::set_mesh_buffers(const MeshBuffers* mb) { mesh_buffers = mb; }
/**
*
* @param c
*/
void Object::set_color(Color c) { color = c; }

/**
*
* @brief updates bounding volume aabb if necesary
* @return
*/
const AABB& Object::get_aabb() const {
	if (!is_aabb_updated) {
		aabb = compute_bv_aabb_from_obb(get_model(), obb);
		is_aabb_updated = true;
	}
	return aabb;
}

/**
*
* @brief updates position and velocity, reset acceleration
* @param delta
*/
void Object::update_physics(float delta) {
	set_vel(vel + acc * delta);
	set_pos(pos + vel * delta);
	set_vel(vel * 0.97f);	// hack drag
	acc = {};
}

