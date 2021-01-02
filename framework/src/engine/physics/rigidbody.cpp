/**
* @file rigidbody.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Implement RigidBody class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#include <pch.h>

RTTI_IMPL(RigidBody, Component)

/**
*
* @brief updates position and velocity, reset acceleration
* @param delta
*/
void RigidBody::update_physics(float delta) {
	set_vel(vel + acc * delta);
	set_pos(pos + vel * delta);
	set_vel(vel * 0.97f);	// hack drag
	acc = {};
}
