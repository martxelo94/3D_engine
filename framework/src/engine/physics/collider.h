/**
* @file collider.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare Collider class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/
#ifndef COLLIDER_H
#define COLLIDER_H

class Collider : public Component
{
	// bounding volumes in local space (then apply matrix)
	AABB   obb = AABB{ {},{} };		// aabb at model space, apply transformation
	mutable AABB   aabb;	// computed from obb
	mutable bool is_aabb_updated = false;
public:
	// get model space obb (aabb) be carefull! remember that need to be multiplied
	inline const AABB& get_obb() const { return obb; }
	const AABB& get_aabb() const;	// compute aabb if needed

	void set_aabb(const AABB& ab) { aabb = ab; }	// DEBUG: used for assigning value from file, unhack as soon as posible

	RTTI_DCL
};

#endif