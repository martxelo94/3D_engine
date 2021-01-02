/**
* @file game_object.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare GameObject class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

class GameObject : public Component
{
private:
	// hierarchy
	std::vector<GameObject*> children;
	std::vector<Component*> components;
	// transform
	vec3 pos = vec3{ 0.f }, sc = vec3{ 1.f }, rot = vec3{ 0.f };
	mutable mat4 model_mtx = mat4(1.f);
	mutable bool is_transform_updated = true;

public:
	// flow control
	void init();
	void free();

	// transform
	inline const vec3& get_pos() const { return pos; }
	inline const vec3& get_scale() const { return sc; }
	inline const vec3& get_rot() const { return rot; }
	void set_pos(const vec3& p);
	void set_scale(const vec3& sc);
	void set_rot(const vec3& rot);
	const mat4& get_model() const;	// updates matrix


	// hierarchy
	void add_child(GameObject* child);
	void remove_child(GameObject* child, bool mantain_transform = false);
	GameObject* find_child(const std::string &id);

	// components
	template<typename T>
	T* add_component() {
		
	}

	RTTI_DCL
};


#endif