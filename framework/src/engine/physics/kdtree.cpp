/**
* @file kdtree.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/03/25	(eus)
* @brief Implement kdtree data structure
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>

AABB compute_aabb_from_triangle_indices(const std::vector<Triangle>& triangles, const std::vector<size_t>& indices)
{
	// just compute AABB with repeated vertices (most likely)
	vec3 min_p = vec3{ FLT_MAX }, max_p = vec3{ -FLT_MAX };

	for (size_t i : indices) {
		const Triangle& tri = triangles[i];
		// x
		if (tri.p0.x > max_p.x) max_p.x = tri.p0.x;
		if (tri.p0.x < min_p.x) min_p.x = tri.p0.x;
		// y
		if (tri.p0.y > max_p.y) max_p.y = tri.p0.y;
		if (tri.p0.y < min_p.y) min_p.y = tri.p0.y;
		// z
		if (tri.p0.z > max_p.z) max_p.z = tri.p0.z;
		if (tri.p0.z < min_p.z) min_p.z = tri.p0.z;

		// x
		if (tri.p1.x > max_p.x) max_p.x = tri.p1.x;
		if (tri.p1.x < min_p.x) min_p.x = tri.p1.x;
		// y
		if (tri.p1.y > max_p.y) max_p.y = tri.p1.y;
		if (tri.p1.y < min_p.y) min_p.y = tri.p1.y;
		// z
		if (tri.p1.z > max_p.z) max_p.z = tri.p1.z;
		if (tri.p1.z < min_p.z) min_p.z = tri.p1.z;

		// x
		if (tri.p2.x > max_p.x) max_p.x = tri.p2.x;
		if (tri.p2.x < min_p.x) min_p.x = tri.p2.x;
		// y
		if (tri.p2.y > max_p.y) max_p.y = tri.p2.y;
		if (tri.p2.y < min_p.y) min_p.y = tri.p2.y;
		// z
		if (tri.p2.z > max_p.z) max_p.z = tri.p2.z;
		if (tri.p2.z < min_p.z) min_p.z = tri.p2.z;
		
	}
	AABB ab = AABB{ min_p, max_p };

	return ab;
}

vec2 get_endpoints(const Triangle& tri, int axis) {
	assert(axis < 3 && axis >= 0);
	float min_p = FLT_MAX, max_p = -FLT_MAX;
	if (tri.p0[axis] > max_p) max_p = tri.p0[axis];
	if (tri.p0[axis] < min_p) min_p = tri.p0[axis];

	if (tri.p1[axis] > max_p) max_p = tri.p1[axis];
	if (tri.p1[axis] < min_p) min_p = tri.p1[axis];

	if (tri.p2[axis] > max_p) max_p = tri.p2[axis];
	if (tri.p2[axis] < min_p) min_p = tri.p2[axis];

	return {min_p, max_p};
}

float heuristic(float c_traversal, float c_intersection, float l_prob, float r_prob, size_t l_count, size_t r_count)
{
	return c_traversal + c_intersection * (l_prob * l_count + r_prob * r_count);
}

void assign_side(const Plane& pl, const std::vector<Triangle>& triangles, const std::vector<size_t>& indices, std::vector<size_t> &l_candidates, std::vector<size_t> &r_candidates)
{
	for (size_t i : indices) {
		const Triangle& tri = triangles[i];
		intersection_type result = intersection_plane_triangle(pl, tri, cEpsilon);
		if (result == INSIDE)
			r_candidates.push_back(i);
		else if (result == OUTSIDE)
			l_candidates.push_back(i);
		else {
			r_candidates.push_back(i);
			l_candidates.push_back(i);
		}
	}
	// let dis japens...
	//assert(!l_candidates.empty());
	//assert(!r_candidates.empty());
}

vec3 get_axis(int axis_idx) { vec3 v{ 0.f }; v[axis_idx] = 1.f; return v; }

void kdtree::build(const std::vector<Triangle>& triangles,
	const kdtree::config&                config)
{
	_config = config;
	// clear current data
	_nodes.clear();
	_aabbs.clear();
	_indices.clear();
	//check if empty
	if (triangles.empty())
		return;

	std::vector<size_t> indices(triangles.size());
	std::iota(indices.begin(), indices.end(), 0);
	AABB root_aabb = compute_bv_aabb_from_points(reinterpret_cast<const vec3*>(triangles.data()), triangles.size() * 3);
	const float root_aabb_area = root_aabb.surface_area();
	_aabbs.push_back(root_aabb);
	_nodes.push_back(node());

	// check if single triangle
	if (triangles.size() < 2 || (u32)_config.max_depth <= 0u || root_aabb_area == 0.f) {
		_nodes.back().set_leaf(0, (int)triangles.size());
		_indices = indices;
		return;
	}
	std::vector<size_t> best_l_candidates, best_r_candidates;
	AABB l_aabb = root_aabb, r_aabb = root_aabb;
	AABB best_l_aabb, best_r_aabb;
	//compute node's AABB
	vec3 dif_aabb = root_aabb.max_point - root_aabb.min_point;
	vec3 midpoint = root_aabb.min_point + (dif_aabb) / 2.f;
	const int axis = dif_aabb[0] == 0.f ? dif_aabb[1] == 0.f ? 2 : 1 : 0;
	float best_h = FLT_MAX;
	float split_point = 0;	// this will be setted for sure
	// iterate through endpoints and find best spliting point
	for (int i = 0; i < triangles.size(); ++i) {
		vec2 endpoints = get_endpoints(triangles[i], axis);
		std::vector<size_t> l_candidates, r_candidates;
		// min endpoint
		if (glm::epsilonNotEqual(endpoints[axis], root_aabb.min_point[axis], cEpsilon))
		{
			midpoint[axis] = endpoints[0];
			assign_side(Plane(get_axis(axis), midpoint), triangles, indices, l_candidates, r_candidates);
			l_aabb.max_point[axis] = endpoints[0];
			r_aabb.min_point[axis] = endpoints[0];
			float l_prob = l_aabb.surface_area() / root_aabb_area;
			float r_prob = r_aabb.surface_area() / root_aabb_area;
			
			float h = heuristic(_config.cost_traversal, _config.cost_intersection, l_prob, r_prob, l_candidates.size(), r_candidates.size());
			if (h < best_h) {
				best_h = h;
				best_l_candidates = std::move(l_candidates);
				best_r_candidates = std::move(r_candidates);
				best_l_aabb = l_aabb;
				best_r_aabb = r_aabb;
				split_point = endpoints[0];
			}
		}
		// max endpoint
		if (glm::epsilonNotEqual(endpoints[1], root_aabb.max_point[axis], cEpsilon))
		{
			l_candidates.clear(); r_candidates.clear();
			midpoint[axis] = endpoints[1];
			assign_side(Plane(get_axis(axis), midpoint), triangles, indices, l_candidates, r_candidates);
			l_aabb.max_point[axis] = endpoints[1];
			r_aabb.min_point[axis] = endpoints[1];
			float l_prob = l_aabb.surface_area() / root_aabb_area;
			float r_prob = r_aabb.surface_area() / root_aabb_area;
			float h = heuristic(_config.cost_traversal, _config.cost_intersection, l_prob, r_prob, l_candidates.size(), r_candidates.size());
			if (h < best_h) {
				best_h = h;
				best_l_candidates = std::move(l_candidates);
				best_r_candidates = std::move(r_candidates);
				best_l_aabb = l_aabb;
				best_r_aabb = r_aabb;
				split_point = endpoints[1];
			}
		}
	}
	// make leaf if one of the sides is empty
	if (best_l_candidates.empty() || best_r_candidates.empty()) {
		_nodes.back().set_leaf(0, (int)triangles.size());
		_indices = indices;
		return;
	}

	// call recursion
	split(1, best_l_aabb, 1, triangles, best_l_candidates);
	//set as internal
	_nodes[0].set_internal(axis, split_point, (int)_nodes.size());
	split(_nodes.size(), best_r_aabb, 1, triangles, best_r_candidates);
}


void kdtree::split(size_t        node_index,
	const AABB&                  parent_aabb,
	int                          depth,
	const std::vector<Triangle>& all_triangles,
	const std::vector<size_t>&   candidate_triangles)
{
	assert(candidate_triangles.empty() == false);

	_aabbs.push_back(parent_aabb);
	_nodes.push_back(node());

	// check if single triangle
	if (candidate_triangles.size() < 2 || (u32)_config.max_depth <= (u32)depth) {
		_nodes.back().set_leaf((int)_indices.size(), (int)candidate_triangles.size());
		_indices.insert(_indices.end(), candidate_triangles.begin(), candidate_triangles.end());
		return;
	}
	std::vector<size_t> best_l_candidates, best_r_candidates;
	//compute node's AABB
	vec3 dif_aabb = parent_aabb.max_point - parent_aabb.min_point;
	vec3 midpoint = parent_aabb.min_point + (dif_aabb) / 2.f;
	AABB l_aabb = parent_aabb, r_aabb = parent_aabb;
	AABB best_l_aabb, best_r_aabb;
	int axis = depth % 3;
	axis = dif_aabb[axis] == 0.f ? dif_aabb[(depth + 1) % 3] == 0.f ? (depth + 2) % 3 : (depth + 1) % 3 : axis;
	const float parent_aabb_area = parent_aabb.surface_area();
	float best_h = FLT_MAX;
	float split_point = 0;	// this will be setted for sure
	// iterate through endpoints and find best spliting point
	for (size_t i : candidate_triangles) {
		vec2 endpoints = get_endpoints(all_triangles[i], axis);
		std::vector<size_t> l_candidates, r_candidates;
		// min endpoint
		if (glm::epsilonNotEqual(endpoints[0], parent_aabb.min_point[axis], cEpsilon)) 
		{
			midpoint[axis] = endpoints[0];
			assign_side(Plane(get_axis(axis), midpoint), all_triangles, candidate_triangles, l_candidates, r_candidates);
			l_aabb.max_point[axis] = endpoints[0];
			r_aabb.min_point[axis] = endpoints[0];
			float l_prob = l_aabb.surface_area() / parent_aabb_area;
			float r_prob = r_aabb.surface_area() / parent_aabb_area;
			float h = heuristic(_config.cost_traversal, _config.cost_intersection, l_prob, r_prob, l_candidates.size(), r_candidates.size());
			if (h < best_h) {
				best_h = h;
				best_l_candidates = std::move(l_candidates);
				best_r_candidates = std::move(r_candidates);
				best_l_aabb = l_aabb;
				best_r_aabb = r_aabb;
				split_point = endpoints[0];
			}
		}
		// max endpoint
		if (glm::epsilonNotEqual(endpoints[1], parent_aabb.max_point[axis], cEpsilon)) 
		{
			l_candidates.clear(); r_candidates.clear();
			midpoint[axis] = endpoints[1];
			assign_side(Plane(get_axis(axis), midpoint), all_triangles, candidate_triangles, l_candidates, r_candidates);
			l_aabb.max_point[axis] = endpoints[1];
			r_aabb.min_point[axis] = endpoints[1];
			float l_prob = l_aabb.surface_area() / parent_aabb_area;
			float r_prob = r_aabb.surface_area() / parent_aabb_area;
			float h = heuristic(_config.cost_traversal, _config.cost_intersection, l_prob, r_prob, l_candidates.size(), r_candidates.size());
			if (h < best_h) {
				best_h = h;
				best_l_candidates = std::move(l_candidates);
				best_r_candidates = std::move(r_candidates);
				best_l_aabb = l_aabb;
				best_r_aabb = r_aabb;
				split_point = endpoints[1];
			}
		}
	}
	// make leaf if one of the sides is empty
	if (best_l_candidates.empty() || best_r_candidates.empty()) {
		_nodes.back().set_leaf((int)_indices.size(), (int)candidate_triangles.size());
		_indices.insert(_indices.end(), candidate_triangles.begin(), candidate_triangles.end());
		return;
	}
	// call recursion
	split(node_index + 1, best_l_aabb, depth + 1, all_triangles, best_l_candidates);
	//set as internal
	_nodes[node_index].set_internal(axis, split_point, (int)_nodes.size());
	split(_nodes.size(), best_r_aabb, depth + 1, all_triangles, best_r_candidates);
}

std::vector<AABB>          kdtree::get_aabbs(int depth) const
{
	// iterate from root (0)
	if (depth == 0) {
		// get root aabb
		std::vector<AABB> depth_aabbs({_aabbs[0]});
		return depth_aabbs;
	}
	std::vector<AABB> l_aabbs = get_aabbs(depth, 1, 1);
	std::vector<AABB> r_aabbs = get_aabbs(depth, 1, _nodes[0].next_child());
	l_aabbs.insert(l_aabbs.end(), r_aabbs.begin(), r_aabbs.end());
	return l_aabbs;
}
std::vector<AABB>          kdtree::get_aabbs(int depth, int current_depth, int node_idx) const
{
	if (depth == current_depth || _nodes[node_idx].is_leaf()) {
		// get root aabb
		std::vector<AABB> depth_aabbs({ _aabbs[node_idx] });
		return depth_aabbs;
	}
	std::vector<AABB> l_aabbs = get_aabbs(depth, current_depth + 1, node_idx + 1);
	std::vector<AABB> r_aabbs = get_aabbs(depth, current_depth + 1, _nodes[node_idx].next_child());
	l_aabbs.insert(l_aabbs.end(), r_aabbs.begin(), r_aabbs.end());
	return l_aabbs;
}
