/**
* @file bounding_volume_hierarchy.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/08	(eus)
* @brief Define bounding volume hierarchy
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef BOUNDING_VOLUME_HIERARCHY_H
#define BOUNDING_VOLUME_HIERARCHY_H

struct Object;

class bounding_volume_hierarchy {
public:
	struct node;
	using children_list = std::array<node*, 2>;	// may want to be more than binary?

	struct node {
		mutable bool draw_bv = true;	// debug hack
		std::vector <const Object*> objects{};	// only read
		AABB bounding_volume{};
		node* parent = nullptr;	// needed to relink and rebuild up (TODO: do it without parent)
		children_list children{ nullptr };
		~node() {	// same as destroy but not caring of setting children to NULL
			// delete all subtree (post-order recursive)
			delete children[0];	// calls delete recursively
			delete children[1];
		}
	};
private:
	node* m_root;

#ifndef TRAVERSE_METHODS
	template<typename TRAVERSE_NODE_FN>
	void traverse_preorder(node* n, TRAVERSE_NODE_FN fn) {
		if (!n) return;
		fn(*n);
		traverse_preorder(n->children[0], fn);
		traverse_preorder(n->children[1], fn);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_inorder(node* n, TRAVERSE_NODE_FN fn) {
		if (!n) return;
		traverse_inorder(n->children[0], fn);
		fn(*n);
		traverse_inorder(n->children[1], fn);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_postorder(node* n, TRAVERSE_NODE_FN fn) {
		if (!n) return;
		traverse_postorder(n->children[0], fn);
		traverse_postorder(n->children[1], fn);
		fn(*n);
	}

	// READONLY
	template<typename TRAVERSE_NODE_FN>
	void traverse_preorder(const node* n, TRAVERSE_NODE_FN fn) const {
		if (!n) return;
		fn(*n);
		traverse_preorder(n->children[0], fn);
		traverse_preorder(n->children[1], fn);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_inorder(const node* n, TRAVERSE_NODE_FN fn) const {
		if (!n) return;
		traverse_inorder(n->children[0], fn);
		fn(*n);
		traverse_inorder(n->children[1], fn);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_postorder(const node* n, TRAVERSE_NODE_FN fn) const {
		if (!n) return;
		traverse_postorder(n->children[0], fn);
		traverse_postorder(n->children[1], fn);
		fn(*n);
	}
#endif

	template<typename PREDICATE>
	node* find(node* n, PREDICATE fn) const {
		if (!n) return n;
		children_list pref_children = fn(n);

		// foreach children in best order
		for (auto& c : pref_children)
			if (node* found = find(c, fn))
				return found;
		return nullptr;
	}
	// return parent node to rebuild up
	bool remove_object(const Object& obj, node* current);
	std::pair<node*, node*> select_branch_by_position(const Object& obj, const node& current);
	// return created node
	node* build_top_down(std::vector  <const Object* >& objects, node* n);
	u64 recursion_counter = 0;
public:
	bounding_volume_hierarchy()
		: m_root(nullptr)
	{}
	~bounding_volume_hierarchy() { destroy(); }
	void build_top_down(std::vector  <const Object* >& objects);
	void build_bottom_up(const std::vector <const Object* >& objects);
	bool add_object(const Object& obj);
	inline bool remove_object(const Object& obj) { return remove_object(obj, m_root); }
	void destroy() { delete m_root; m_root = nullptr; }	// calls destroy on nodes recursively
	const node* root() const { return m_root; }

#ifndef TRAVERSE_METHODS
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_preorder(TRAVERSE_NODE_FN fn) { traverse_preorder(m_root, fn); }
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_inorder(TRAVERSE_NODE_FN fn) { traverse_inorder(m_root, fn); }
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_postorder(TRAVERSE_NODE_FN fn) { traverse_postorder(m_root, fn); }

	// readonly
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_preorder(TRAVERSE_NODE_FN fn) const { traverse_preorder(m_root, fn); }
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_inorder(TRAVERSE_NODE_FN fn) const { traverse_inorder(m_root, fn); }
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_postorder(TRAVERSE_NODE_FN fn) const { traverse_postorder(m_root, fn); }
#endif

	template<typename PREDICATE>
	inline node* find(PREDICATE fn) const { return find(m_root, fn); }

};

using BVH = bounding_volume_hierarchy;

#endif