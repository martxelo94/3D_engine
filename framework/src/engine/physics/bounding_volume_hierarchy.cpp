/**
* @file bounding_volume_hierarchy.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/08	(eus)
* @brief Implement bounding volume hierarchy
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>

/**
*
* @param obj
* @return
*/
bool bounding_volume_hierarchy::add_object(const Object& obj) {
	// if room empty, create first node
	if (m_root == nullptr) {
		m_root = new node;
		m_root->objects.push_back(&obj);
		m_root->bounding_volume = obj.get_aabb();
		return true;
	}
	// use stack to traverse the tree the best way
	std::stack<node*> nodes;
	nodes.push(m_root);

	while (!nodes.empty()) {
		auto n = nodes.top();
		nodes.pop();

		// find best node by delta surface
		// if leaf node, found it
		bool is_leaf = n->children[0] == nullptr && n->children[1] == nullptr;
		if (is_leaf) {
			//check that not already here
			assert(n->objects.size() == 1);
			if (n->objects[0] == &obj)
				return false;
			// create new nodes
			node* new_child0 = new node;
			node* new_child1 = new node;
			//link
			new_child0->parent = new_child1->parent = n;
			n->children = { new_child0, new_child1 };
			//move objects
			new_child0->objects = std::move(n->objects);
			new_child1->objects.push_back(&obj);
			// set aabb
			const AABB &bv_node = new_child0->bounding_volume = n->bounding_volume;
			const AABB &bv_obj = new_child1->bounding_volume = obj.get_aabb();
			// compute new aabb
			n->bounding_volume = AABB{ glm::min(bv_obj.min_point, bv_node.min_point), glm::max(bv_obj.max_point, bv_node.max_point) };

			// also dont forget to rebuild up!
			node* grandparent = n->parent;
			while (grandparent) {
				const AABB &bv0 = grandparent->children[0]->bounding_volume;
				const AABB &bv1 = grandparent->children[1]->bounding_volume;
				grandparent->bounding_volume = AABB{ glm::min(bv0.min_point, bv1.min_point), glm::max(bv0.max_point, bv1.max_point) };

				grandparent = grandparent->parent;
			}
			return true;
		}
		else   // keep iterating... but choose best surface!
		//if (n->children[0] != nullptr) 
		{
			assert(n->children[0]);	// must have children
			assert(n->children[1]);	// must have sibling
			const AABB &bv_obj = obj.get_aabb();
			const AABB &bv0 = n->children[0]->bounding_volume;
			const AABB &bv1 = n->children[1]->bounding_volume;
			// compare addition of aabb bounding volumes
			AABB bv_obj0 = AABB{ glm::min(bv0.min_point, bv_obj.min_point), glm::max(bv0.max_point, bv_obj.max_point) };
			AABB bv_obj1 = AABB{ glm::min(bv_obj.min_point, bv1.min_point), glm::max(bv_obj.max_point, bv1.max_point) };
			// delta surface
			float dSurface = bv_obj0.surface_area() - bv_obj1.surface_area();
			if (dSurface < 0.f) {
				// child 1 is bigger, go to child 0
				nodes.push(n->children[0]);
			}
			else {
				// child 0 is bigger
				nodes.push(n->children[1]);
			}
		}

	}
	return false;
}

/**
*
* @param obj
* @param current
* @return
*/
bool bounding_volume_hierarchy::remove_object(const Object& obj, node* current) {

	// base case: return if node null
	if (current == nullptr)
		return false;

	assert(current != nullptr);
	auto & objs = current->objects;

	std::pair<node*, node*> best_order = select_branch_by_position(obj, *current);
	// leaf check
	bool is_leaf = best_order.first == nullptr && best_order.second == nullptr;
	if (!is_leaf) {
		assert(objs.empty());	// intermediate always empty
		return remove_object(obj, best_order.first) || remove_object(obj, best_order.second);
	}
	assert(objs.size() <= 1);	// one object per node
	// object may be here...
	auto it = std::find(objs.begin(), objs.end(), &obj);
	if (it == objs.end())
		return false;	// not here

	std::swap(*it, objs[objs.size() - 1]);
	objs.pop_back();

	// get grandparent; if dont, pass last single object to root node, its parent
	if (current == m_root) {
		assert(current->parent == nullptr);
		delete m_root;
		m_root = nullptr;
		return true;
	}
	node* parent = current->parent;
	// get sibling before updating grandpa
	node* sibling = parent->children[0] == current ? parent->children[1] : parent->children[0];
	// copy sibling to parent (no need to know grandpa)
	parent->bounding_volume = sibling->bounding_volume;
	parent->objects = std::move(sibling->objects);
	// relink children with parent (if any)
	if (sibling->children[0]) {
		sibling->children[0]->parent = sibling->children[1]->parent = parent;
	}
	parent->children = std::move(sibling->children);	// copy since sibling might be intermediate node
	sibling->children = { nullptr, nullptr };			// make sure not deleting their subtree
	// delete nodes
	delete current;
	delete sibling;
	// also dont forget to rebuild up!
	parent = parent->parent;
	while (parent) {
		const AABB &bv0 = parent->children[0]->bounding_volume;
		const AABB &bv1 = parent->children[1]->bounding_volume;
		parent->bounding_volume = AABB{ glm::min(bv0.min_point, bv1.min_point), glm::max(bv0.max_point, bv1.max_point) };

		parent = parent->parent;
	}

	// done
	return true;
}
/**
*
* @param obj
* @param current
* @return
*/
std::pair<BVH::node*, BVH::node*> bounding_volume_hierarchy::select_branch_by_position(const Object& obj, const node& current)
{
	// if some child null, return null (but be sure the other is null too)
	if (current.children[0] == nullptr) {
		assert(current.children[1] == nullptr);	// sibling null too
		return { nullptr, nullptr };
	}

	// select branch node with aabb closest to obj
	const AABB& obj_bv = obj.get_aabb();
	vec3 obj_center = obj_bv.center();
	intersection_type result = intersection_point_aabb(obj_center, current.children[0]->bounding_volume);
	// if inside, good candidate
	if (result == intersection_type::INSIDE) {
		return { current.children[0], current.children[1] };
	}
	result = intersection_point_aabb(obj_center, current.children[0]->bounding_volume);
	// if inside, good candidate
	if (result == intersection_type::INSIDE) {
		return { current.children[1], current.children[0] };
	}
#if 0	// select method of choice
	// select closest center
	vec3 center0 = current.children[0]->bounding_volume.center();
	vec3 center1 = current.children[1]->bounding_volume.center();
	if (glm::distance2(obj_center, center0) < glm::distance2(obj_center, center1))
		return { current.children[0], current.children[1] };
	else
		return { current.children[1], current.children[0] };
#else
	// select lesser surface area
	AABB ab0 = AABB{ glm::min(obj_bv.min_point, current.children[0]->bounding_volume.min_point),
					 glm::max(obj_bv.max_point, current.children[0]->bounding_volume.max_point) };
	AABB ab1 = AABB{ glm::min(obj_bv.min_point, current.children[1]->bounding_volume.min_point),
					 glm::max(obj_bv.max_point, current.children[1]->bounding_volume.max_point) };
	if (ab0.surface_area() < ab1.surface_area())
		return { current.children[0], current.children[1] };
	return { current.children[1], current.children[0] };
#endif
#if 0
	assert(0 && "Could happen?");	// NO
	return {};
#endif
}
/**
*
* @param objects
* @return
*/
AABB compute_aabb(const std::vector<const Object*>& objects) {
	//assert(objects.size() > 1);	// allow single object
	AABB result = objects[0]->get_aabb();
	for (int i = 1; i < objects.size(); ++i) {
		const AABB& ab = objects[i]->get_aabb();
		result.min_point = glm::min(result.min_point, ab.min_point);
		result.max_point = glm::max(result.max_point, ab.max_point);
	}
	return result;
}
/**
*
* @param objects
* @param bv_all
* @return
*/
std::pair<std::vector<const Object*>, std::vector<const Object*>> partition(const std::vector<const Object*> & objects, const AABB& bv_all) {
	assert(objects.size() > 1);	// must!

	// get biggest axis for partition plane
	vec3 dir = bv_all.max_point - bv_all.min_point;
	vec3 normal = vec3{ 1, 0, 0 };
	float biggest_axis = dir.x;
	if (dir.y > biggest_axis) {
		normal = vec3{ 0, 1, 0 };
		biggest_axis = dir.y;
	}
	if (dir.z > biggest_axis) {
		normal = vec3{ 0, 0, 1 };
		biggest_axis = dir.z;
	}
	// create plane
	Plane pl(normal, bv_all.center());
	// assign each object {INSIDE first, OUTSIDE second}
	std::pair<std::vector<const Object*>, std::vector<const Object*>> result;
	for (int i = (int)objects.size() - 1; i >= 0; --i) {
		const Object* o = objects[i];
		// get bv center
		const AABB ab = o->get_aabb();
		vec3 pos = ab.center();
		// select side
		intersection_type intersection_result = intersection_point_plane(pos, pl);
		if (intersection_result == INSIDE) {
			result.first.push_back(o);
		}
		else
			result.second.push_back(o);
	}
	// balance if one side without objects
	if (result.first.empty()) {
		// randomly?
		result.first.push_back(result.second.back());
		result.second.pop_back();
	}
	else if (result.second.empty()) {
		result.second.push_back(result.first.back());
		result.first.pop_back();
	}

	return result;
}
/**
*
* @param objects
* @param n
* @return
*/
bounding_volume_hierarchy::node* bounding_volume_hierarchy::build_top_down(std::vector <const Object*>& objects, node* n)
{
	recursion_counter++;
	/*
		// compute bounding volume for all
		bv_all = compute_bv(objects);
		//create the node
		node = create_node(objects, m_root);
		// are we finished?
		if end_condition(node)
			node.type = leaf
		else
			// Do the partition and recurse
			[left, right] = partition(objects)
			bvh_topdown(left, node)
			bvh_topdown(right, node)

	*/
	// exit if no objects
	if (objects.empty())
		return nullptr;
	assert(!objects.empty());
	assert(n != nullptr);	// parent node must be valid
	// compute common aabb
	node* new_node = new node;
	new_node->bounding_volume = compute_aabb(objects);
	// check if we are finished
	if (objects.size() == 1) {
		new_node->objects = objects;
		return new_node;
	}
	//assign sides
	auto part = partition(objects, new_node->bounding_volume);
	// clear memory from old objects 
	//objects.clear();
	//objects.shrink_to_fit();
	// recursion
	node* child0 = build_top_down(part.first, new_node);
	node* child1 = build_top_down(part.second, new_node);
	// link children
	assert(child0 && child1);	// should?
	new_node->children[0] = child0;
	new_node->children[1] = child1;
	child0->parent = child1->parent = new_node;
	// return new_node to allow link
	return new_node;
}
/**
*
* @param objects
*/
void bounding_volume_hierarchy::build_top_down(std::vector  <const Object* >& objects)
{
	// exit if no objects
	if (objects.empty())
		return;
	assert(m_root == nullptr);	// tree should be cleared
	m_root = new node;
	// compute common aabb
	m_root->bounding_volume = compute_aabb(objects);
	// check if we are finished
	if (objects.size() == 1) {
		m_root->objects = objects;
		return;
	}
	//assign sides
	auto part = partition(objects, m_root->bounding_volume);
	// clear memory from old objects 
	objects.clear();
	objects.shrink_to_fit();
	// recursion
	node* child0 = build_top_down(part.first, m_root);
	node* child1 = build_top_down(part.second, m_root);
	// link children
	assert(child0 && child1);	// should?
	m_root->children[0] = child0;
	m_root->children[1] = child1;
	child0->parent = child1->parent = m_root;

	recursion_counter = 0;
}
/**
*
* @param nodes
* @return
*/
std::tuple<BVH::node*, BVH::node*, AABB> find_candidates(const std::vector<BVH::node*> &nodes) {
	float best_surface_area = FLT_MAX;	// smaller
	std::tuple<BVH::node*, BVH::node*, AABB> best_pair = { nullptr, nullptr, AABB{} };
	// find pairs O(n^2)
	for (int i = 0; i < nodes.size() - 1; ++i) {
		for (int j = i + 1; j < nodes.size(); ++j) {
			const AABB& ab0 = nodes[i]->bounding_volume;
			const AABB& ab1 = nodes[j]->bounding_volume;
			AABB ab = AABB{ glm::min(ab0.min_point, ab1.min_point),
				 glm::max(ab0.max_point, ab1.max_point) };

			float surf_area = ab.surface_area();
			if (surf_area < best_surface_area) {
				best_surface_area = surf_area;
				best_pair = { nodes[i], nodes[j], ab };
			}
		}
	}
	return best_pair;
}
/**
*
* @param objects
*/
void bounding_volume_hierarchy::build_bottom_up(const std::vector <const Object*>& objects)
{
	assert(m_root == nullptr);	// tree should be cleared
	/*
		nodes = []
		// compute bounding volumes for all
		for obj : objects {
			node = create_node(obj)
			node.compute_bv()
			nodes.add(node)
		}
		while(nodes.size > 1){
			[node_a, node_b] = find_candidates(nodes)
			node = create_node()
			node.children = [node_a, node_b]
			node.compute_bv()
			nodes.erase([node_a, node_b])
		}
		return nodes[0]
	*/
	std::vector<node*> nodes;
	nodes.reserve(objects.size());
	// compute bounding volumes for all and create new nodes
	for (auto obj : objects) {
		node* n = new node;
		n->bounding_volume = obj->get_aabb();
		n->objects.push_back(obj);
		nodes.push_back(n);
	}
	while (nodes.size() > 1) {
		auto best_pair = find_candidates(nodes);
		node* child0 = std::get<0>(best_pair);
		node* child1 = std::get<1>(best_pair);
		node* n = new node;
		//link
		n->children = { child0, child1 };
		child0->parent = child1->parent = n;
		//compute bv (already done in pair find
		n->bounding_volume = std::get<2>(best_pair);
		//erase (back swap) nodes before adding new
		auto it = std::find(nodes.begin(), nodes.end(), child0);
		assert(it != nodes.end());
		nodes.erase(it);
		it = std::find(nodes.begin(), nodes.end(), child1);
		assert(it != nodes.end());
		nodes.erase(it);
		//add new node
		nodes.push_back(n);
	}
	// link the last node to the root
	m_root = nodes.back();
}
