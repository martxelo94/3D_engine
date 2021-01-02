/**
* @file rigidbody.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare RigidBody class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

class RigidBody : public Component
{
	vec3 vel = vec3{};	// velocity
	vec3 acc = vec3{};	// acceleration

public:
	// get velocity (readonly)
	inline const vec3& get_vel() const { return vel; }
	// get acceleration (readonly)
	inline const vec3& get_acc() const { return acc; }
	//set velocity (updated in update_physics)
	inline void set_vel(const vec3& v) { vel = v; }
	//set acceleration (updated in update_physics)
	inline void set_acc(const vec3& a) { acc = a; }

	void update_physics(float delta);

	RTTI_DCL
};

#endif