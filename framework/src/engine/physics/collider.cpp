/**
* @file collider.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Implement Collider class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#include <pch.h>

RTTI_IMPL(Collider, Component)

/**
*
* @brief updates bounding volume aabb if necesary
* @return
*/
const AABB& Collider::get_aabb() const {
	if (!is_aabb_updated) {
		aabb = compute_bv_aabb_from_obb(gameObject->get_model(), obb);
		is_aabb_updated = true;
	}
	return aabb;
}
