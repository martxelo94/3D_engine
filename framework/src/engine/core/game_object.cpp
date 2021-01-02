/**
* @file game_object.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Implement GameObject class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#include <pch.h>

RTTI_IMPL(GameObject, Object)

/**
*
* @param p
*/
void GameObject::set_pos(const vec3& p) {
	pos = p;
	is_transform_updated = is_aabb_updated = false;
}
/**
*
* @param _sc
*/
void GameObject::set_scale(const vec3& _sc) {
	sc = _sc;
	is_transform_updated = is_aabb_updated = false;
}
/**
*
* @param _rot
*/
void GameObject::set_rot(const vec3& _rot) {
	rot = _rot;
	is_transform_updated = is_aabb_updated = false;
}
/**
*
* @brief updates model matrix if necesary
* @return
*/
const mat4& GameObject::get_model() const {
	if (!is_transform_updated) {
		model_mtx = glm::translate(pos) * glm::toMat4(glm::quat{ rot }) * glm::scale(sc);
		is_transform_updated = true;
	}
	return model_mtx;
}
