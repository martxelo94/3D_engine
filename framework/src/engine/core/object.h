/**
* @file object.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/22	(eus)
* @brief Declare Object class
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#ifndef OBJECT_H
#define OBJECT_H

class Object
{
	// BASIC
private:
	std::string name;
	bool enabled = true;

public:
	bool operator==(const Object& rhs) const { return this == &rhs; }

	void set_enabled(bool val) { enabled = val; }
	bool get_enabled() const { return enabled; }

	void set_name(const std::string& _name) { name = _name; }
	const std::string& get_name() const { return name; }

	// TRANSFORM
private:
	vec3 pos = vec3{ 0.f }, sc = vec3{ 1.f }, rot = vec3{ 0.f };
	mutable mat4 model_mtx = mat4(1.f);
	mutable bool is_transform_updated = true;
public:
	inline const vec3& get_pos() const { return pos; }
	inline const vec3& get_scale() const { return sc; }
	inline const vec3& get_rot() const { return rot; }
	void set_pos(const vec3& p);
	void set_scale(const vec3& sc);
	void set_rot(const vec3& rot);
	const mat4& get_model() const;	// updates matrix

	// RENDERING
private:
	const MeshData* mesh_data = nullptr;
	const MeshBuffers* mesh_buffers = nullptr;
	Color color{ 0xffffffff };	// TODO: sustituye por Material

public:
	inline const MeshData* get_mesh_data() const { return mesh_data; }
	inline const MeshBuffers* get_mesh_buffers() const { return mesh_buffers; }
	inline Color get_color() const { return color; }

	void set_mesh_data(const MeshData* md);
	void set_mesh_buffers(const MeshBuffers* mb);
	void set_color(Color c);

	// COLLIDER
private:
	// bounding volumes in local space (then apply matrix)
	AABB   obb = AABB{ {},{} };		// aabb at model space, apply transformation
	mutable AABB   aabb;	// computed from obb
	mutable bool is_aabb_updated = false;
public:
	// get model space obb (aabb) be carefull! remember that need to be multiplied
	inline const AABB& get_obb() const { return obb; }
	const AABB& get_aabb() const;	// compute aabb if needed

	void set_aabb(const AABB& ab) { aabb = ab; }	// DEBUG: used for assigning value from file, unhack as soon as posible

	// RIGIDBODY
private:
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

};

#endif	// OBJECT_H