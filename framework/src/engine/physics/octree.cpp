/**
* @file octree.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/03/08	(eus)
* @brief Implement octree data structure
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>

/**
* @brief
* @param code
*/
Octree::node::node(loc_code code)
	: code(code), level(0)
{
	std::memset(&children[0], 0, sizeof(node*) * 8);
}
/**
*
* @brief
*/
Octree::node::~node() {
	for (int i = 0; i < 8; ++i)
		delete children[i];
}
/**
*
* @brief
* @param objects
*/
void Octree::build(const std::vector<const Object*>& objects)
{
	for (const Object* o : objects)
		add_object(*o);
}
/**
* @brief
* @param obj
* @return
*/
Octree::node* Octree::add_object(const Object& obj)
{
	// check if not completely inside root
	intersection_type result = intersection_aabb_aabb(obj.get_aabb(), m_root.get_aabb());
	//if not inside BV, add to root
	if (result != INSIDE) {
		auto it = std::find(m_root.objects.begin(), m_root.objects.end(), &obj);
		if (it == m_root.objects.end())	// add only if not added yet
			m_root.objects.push_back(&obj);
		return &m_root;
	}
	// check children by 3 plane intersection instead of aabb check
	Plane planes[3] = {
		Plane{ vec3{1,0,0}, m_root.pos },
		Plane{ vec3{0,1,0}, m_root.pos },
		Plane{ vec3{0,0,1}, m_root.pos }
	};
	intersection_type results[3];
	for (int i = 0; i < 3; ++i) {
		results[i] = intersection_plane_aabb(planes[i], obj.get_aabb());
	}
	// if not intersection, choose child
	int planes_intersected = results[0] | results[1] | results[2];
	if ((planes_intersected & OVERLAPS) == OVERLAPS) {
		auto it = std::find(m_root.objects.begin(), m_root.objects.end(), &obj);
		if (it == m_root.objects.end())	// add only if not added yet
			m_root.objects.push_back(&obj);
		return &m_root;
	}
	// make code
	loc_code local_code = ((results[0] == INSIDE) << 2) | ((results[1] == INSIDE) << 1) | (results[2] == INSIDE);
	// choose child node and recursion
	auto & child = m_root.children[local_code];
	// create node?
	if (!child) {
		loc_code child_code = (m_root.code << OCT_LEVEL_SHIFT) | local_code;
		child = m_root.add_child(local_code, child_code);
		child->size = m_root.size * 0.5f;
		child->pos = m_root.pos + get_offset((u8)local_code) * child->size;
	}
	return add_object(child, obj);

}

/**
* @brief
* @param obj
* @return
*/
bool Octree::remove_object(const Object& obj)
{
	// if not inside, might be in root
	intersection_type result = intersection_aabb_aabb(obj.get_aabb(), m_root.get_aabb());
	if (result != INSIDE) {
		auto it = std::find(m_root.objects.begin(), m_root.objects.end(), &obj);
		if (it == m_root.objects.end())
			return false;
		m_root.objects.erase(it);
		return true;
	}
	// check children by 3 plane intersection instead of aabb check
	Plane planes[3] = {
		Plane{ vec3{1,0,0}, m_root.pos },
		Plane{ vec3{0,1,0}, m_root.pos },
		Plane{ vec3{0,0,1}, m_root.pos }
	};
	intersection_type results[3];
	for (int i = 0; i < 3; ++i) {
		results[i] = intersection_plane_aabb(planes[i], obj.get_aabb());
	}
	// if not intersection, choose child
	int planes_intersected = results[0] | results[1] | results[2];
	// already checked if in root
	if ((planes_intersected & OVERLAPS) == OVERLAPS) {
		// might be in node
		auto it = std::find(m_root.objects.begin(), m_root.objects.end(), &obj);
		if (it == m_root.objects.end())
			return false;
		m_root.objects.erase(it);
		return true;
	}
	// make code
	loc_code local_code = ((results[0] == INSIDE) << 2) | ((results[1] == INSIDE) << 1) | (results[2] == INSIDE);
	// choose child node and recursion
	node* child = m_root.children[local_code];
	// null node?
	if (!child) {
		return false;
	}

	// else, find child for recursion
	node* erased_node = remove_object(&m_root, child, obj);
	if (erased_node && erased_node != &m_root) {
		// remove empty nodes, iterate parents until node not empty
		local_code = erased_node->code & OCT_BIT_CODE_LOCAL_MASK;
		node* parent = get_node(erased_node->code >> OCT_LEVEL_SHIFT);
		const int start_level = erased_node->level;
		for (int i = 1;
			i < start_level;
			++i)
		{
			if (erased_node->empty()) {
				parent->delete_child(local_code);
				erased_node = parent;
				if (erased_node->code == 1)
					break;
				local_code = erased_node->code & OCT_BIT_CODE_LOCAL_MASK;
				parent = get_node(erased_node->code >> OCT_LEVEL_SHIFT);
			}
			else
				break;
		}

		return true;
	}
	return false;
}

/**
* @brief
* @param n
* @param obj
* @return
*/
Octree::node* Octree::add_object(node* n, const Object& obj)
{
	// assert that obj is INSIDE node, otherwise something went wrong
	assert(INSIDE == intersection_aabb_aabb(obj.get_aabb(), n->get_aabb()));

	// if reached max level, insert anyway
	if (n->level == m_max_level_count) {
		auto it = std::find(n->objects.begin(), n->objects.end(), &obj);
		if (it == n->objects.end())	// add only if not added yet
			n->objects.push_back(&obj);
		return n;
	}
	// check children by 3 plane intersection instead of aabb check
	Plane planes[3] = {
		Plane{ vec3{1,0,0}, n->pos },
		Plane{ vec3{0,1,0}, n->pos },
		Plane{ vec3{0,0,1}, n->pos }
	};
	intersection_type results[3];
	for (int i = 0; i < 3; ++i) {
		results[i] = intersection_plane_aabb(planes[i], obj.get_aabb());
	}
	// if not intersection, choose child
	int planes_intersected = results[0] | results[1] | results[2];
	if ((planes_intersected & OVERLAPS) == OVERLAPS) {
		// add to current node
		auto it = std::find(n->objects.begin(), n->objects.end(), &obj);
		if (it == n->objects.end())	// add only if not added yet
			n->objects.push_back(&obj);
		return n;
	}
	// make code
	loc_code local_code = ((results[0] == INSIDE) << 2) | ((results[1] == INSIDE) << 1) | (results[2] == INSIDE);
	// choose child node and recursion
	auto & child = n->children[local_code];
	// create node?
	if (!child) {
		child = n->add_child(local_code, (n->code << OCT_LEVEL_SHIFT) | local_code);
		child->size = n->size * 0.5f;
		child->pos = n->pos + get_offset((u8)local_code) * child->size;
	}
	return add_object(child, obj);
}
/**
* @brief
* @param parent
* @param n
* @param obj
* @return
*/
Octree::node* Octree::remove_object(node* parent, node* n, const Object& obj)
{
	// must be inside!
	intersection_type result = intersection_aabb_aabb(obj.get_aabb(), n->get_aabb());
	assert(result == INSIDE);
	// check children by 3 plane intersection instead of aabb check
	Plane planes[3] = {
		Plane{ vec3{1,0,0}, n->pos },
		Plane{ vec3{0,1,0}, n->pos },
		Plane{ vec3{0,0,1}, n->pos }
	};
	intersection_type results[3];
	for (int i = 0; i < 3; ++i) {
		results[i] = intersection_plane_aabb(planes[i], obj.get_aabb());
	}
	// if not intersection, choose child
	int planes_intersected = results[0] | results[1] | results[2];
	// already checked if in root
	if ((planes_intersected & OVERLAPS) == OVERLAPS) {
		// might be in node
		auto it = std::find(n->objects.begin(), n->objects.end(), &obj);
		if (it == n->objects.end())
			return nullptr;
		n->objects.erase(it);
		return n;
	}
	// make code
	loc_code local_code = ((results[0] == INSIDE) << 2) | ((results[1] == INSIDE) << 1) | (results[2] == INSIDE);
	// choose child node and recursion
	node* child = n->children[local_code];
	// null node?
	if (!child) {
		return nullptr;
	}

	// else, find child for recursion
	return remove_object(n, child, obj);

}

/**
* @brief
*/
void Octree::destroy()
{
	// destroy root node's children and remove objects
	for (int i = 0; i < 8; ++i)
		m_root.delete_child(i);
	m_root.objects.clear();
	m_level_count = 1;
}

/**
* @brief
* @param code
* @return
*/
vec3 Octree::get_center(loc_code code) const
{
	vec3 pos = vec3{};
	float size = get_size(get_level(code));
	for (int i = 0;
		(code != 1) && (i < OCT_MAX_LEVEL);
		++i, code = code >> OCT_LEVEL_SHIFT)
	{
		assert(code != 0);
		// get position for current level
		u8 local_coord = u8(code & OCT_BIT_CODE_LOCAL_MASK);
		vec3 offset = get_offset(local_coord);
		pos += offset * size;
		// update level size
		size *= 2.f;
	}
	return pos + m_root.pos;
}

/**
* @brief
* @param local_coord
* @return
*/
vec3 Octree::get_offset(u8 local_coord) const
{
	u8 x = local_coord & OCT_BIT_CODE_X;
	u8 y = local_coord & OCT_BIT_CODE_Y;
	u8 z = local_coord & OCT_BIT_CODE_Z;
	//return vec3{x? 0.5f : -0.5f , y? 0.5f : -0.5f , z? 0.5f : -0.5f };
	return vec3{ x ? 1.f : -1.f , y ? 1.f : -1.f , z ? 1.f : -1.f };
}

/**
* @brief
* @param code
* @return
*/
int  Octree::get_level(loc_code code) const
{
	assert(code != 0);
	// find the sentinel bit
	int level = 0;
	for (;
		(code != 1) && (level < OCT_MAX_LEVEL);
		code = code >> OCT_LEVEL_SHIFT, ++level)
	{
		assert(code != 0);
	}
	return level;
}

/**
* @brief
* @param code
* @return
*/
AABB Octree::get_aabb(loc_code code) const
{
	vec3 pos = vec3{};
	float size = get_size(get_level(code));
	for (int i = 0;
		(code != 1) && (i < OCT_MAX_LEVEL);
		++i, code = code >> OCT_LEVEL_SHIFT)
	{
		assert(code != 0);
		// get position for current level
		u8 local_coord = u8(code & OCT_BIT_CODE_LOCAL_MASK);
		vec3 offset = get_offset(local_coord);
		pos += offset * size;
		// update level size
		size *= 2.f;
	}
	pos += m_root.pos;
	vec3 diagonal{ size };
	return AABB{ pos - diagonal, pos + diagonal };
}

/**
* @brief
* @param code
* @return
*/
Octree::node* Octree::get_node(loc_code code)
{
	node* found_node = &m_root;
	int level = get_level(code);
	assert((code >> (level * OCT_LEVEL_SHIFT)) == 1);	// find the sentinel bit
	if (level == 0)
		return found_node;
	loc_code found_code = code >> ((level - 1) * OCT_LEVEL_SHIFT);
	// traverse levels
	for (int i = level - 2;
		i >= 0;
		--i)
	{
		// find children
		found_node = found_node->children[(found_code)& OCT_BIT_CODE_LOCAL_MASK];
		found_code = code >> (i * OCT_LEVEL_SHIFT);
	}
	assert(found_node != nullptr);
	return found_node->children[code & OCT_BIT_CODE_LOCAL_MASK];
}

/**
* @brief
* @param code
* @return
*/
std::string Octree::get_code_string(loc_code code)const
{
	std::string code_str;
	code_str.reserve(OCT_MAX_LEVEL * 3);
	// traverse levels
	assert(code != 0);	// code is valid
	// find the sentinel bit
	for (int i = 0;
		i < OCT_MAX_LEVEL, code != 1;
		++i, code = code >> OCT_LEVEL_SHIFT)
	{
		assert(code != 0);	// code still valid
		// find children
		loc_code local_coord = code & OCT_BIT_CODE_LOCAL_MASK;
		u8 z = local_coord & OCT_BIT_CODE_Z;
		u8 y = local_coord & OCT_BIT_CODE_Y; y = y >> 1;
		u8 x = local_coord & OCT_BIT_CODE_X; x = x >> 2;
		code_str.append(z ? "1" : "0");
		code_str.append(y ? "1" : "0");
		code_str.append(x ? "1" : "0");
	}
	std::reverse(code_str.begin(), code_str.end());
	return code_str;

}
