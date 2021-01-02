/**
* @file octree.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/03/08	(eus)
* @brief Define octree data structure
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef OCTREE_H
#define OCTREE_H

#define OCT_MAX_LEVEL ((sizeof(Octree::loc_code) * 8) / 3 - 1)
#define OCT_LEVEL_SHIFT 3
#define OCT_BIT_CODE_X	(1 << 2)
#define OCT_BIT_CODE_Y	(1 << 1)
#define OCT_BIT_CODE_Z	(1 << 0)
#define OCT_BIT_CODE_LOCAL_MASK (8u - 1u)


struct Octree {
	using loc_code = unsigned int;

	struct node {
		// position code respect to parent
		loc_code code = 0u;
		int level;	//easy to create code with bitwise operations
		// spatial data (very handy)
		vec3 pos = vec3{};	// inside unit cube
		float size = 1.f;	// [-0.5, 0.5]
		// objects in this node
		std::vector<const Object*> objects;
		// children (own the memory)
		std::array<node*, 8> children;

		node(loc_code code);
		~node();
		node* add_child(u32 idx, loc_code _code) {
			assert((_code & OCT_BIT_CODE_LOCAL_MASK) == idx);
			assert(children[idx] == nullptr);
			children[idx] = new node(_code);
			children[idx]->level = level + 1;
			return children[idx];
		}
		void delete_child(int child_idx) {
			assert(child_idx < 8);
			//assert(children[child_idx] != nullptr);
			if (children[child_idx] != nullptr) {
				delete children[child_idx];
				children[child_idx] = nullptr;
			}
		}
		AABB get_aabb() const { vec3 diagonal{ size }; return AABB{ pos - diagonal, pos + diagonal }; }
		inline bool is_leaf() const {
			for (int i = 0; i < 8; ++i)
				if (children[i] != nullptr)
					return false;
			return true;
		}
		inline bool empty() const { return objects.empty() && is_leaf(); }
	};
private:
	node m_root = node(1);
	int m_level_count = 1;
	int m_max_level_count = (sizeof(Octree::loc_code) * 8) / 3 - 1;

#ifndef TRAVERSE_METHODS

	template<typename TRAVERSE_NODE_FN>
	void traverse_preorder(node* n, TRAVERSE_NODE_FN fn) {
		if (!n) return;
		fn(*n);
		for (int i = 0; i < 8; ++i)
			traverse_preorder(n->children[i], fn);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_postorder(node* n, TRAVERSE_NODE_FN fn) {
		if (!n) return;
		for (int i = 0; i < 8; ++i)
			traverse_postorder(n->children[i], fn);
		fn(*n);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_if(node* n, TRAVERSE_NODE_FN fn) {
		if (!n) return;
		if (fn(*n))
			for (int i = 0; i < 8; ++i)
				traverse_if(n->children[i], fn);
	}

	// READONLY
	template<typename TRAVERSE_NODE_FN>
	void traverse_preorder(const node* n, TRAVERSE_NODE_FN fn)const {
		if (!n) return;
		fn(*n);
		for (int i = 0; i < 8; ++i)
			traverse_preorder(n->children[i], fn);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_postorder(const node* n, TRAVERSE_NODE_FN fn)const {
		if (!n) return;
		for (int i = 0; i < 8; ++i)
			traverse_postorder(n->children[i], fn);
		fn(*n);
	}
	template<typename TRAVERSE_NODE_FN>
	void traverse_if(const node* n, TRAVERSE_NODE_FN fn) const {
		if (!n) return;
		if (fn(*n))
			for (int i = 0; i < 8; ++i)
				traverse_if(n->children[i], fn);
	}
#endif

	node* add_object(node* n, const Object& obj);
	node* remove_object(node* parent, node* n, const Object& obj);
public:
	void build(const std::vector<const Object*>& objects);
	node* add_object(const Object& obj);
	bool remove_object(const Object& obj);

	void destroy();
	inline const node& root() const { return m_root; }
	inline int get_level_count() const { return m_level_count; }
	inline int get_max_level_count() const { return m_max_level_count; }
	inline int& get_max_level_count() { return m_max_level_count; }
	inline const vec3& get_center() const { return m_root.pos; }
	inline vec3& get_center() { return m_root.pos; }
	inline float get_size() const { return m_root.size; }
	inline float& get_size() { return m_root.size; }
	inline float get_size(int level) const { return m_root.size / (float)glm::pow<float>(2, level); }
	vec3 get_center(loc_code code) const;
	vec3 get_offset(u8 local_coord) const;
	int get_level(loc_code code) const;
	AABB get_aabb(loc_code code) const;
	node* get_node(loc_code code);
	std::string get_code_string(loc_code code)const;

#ifndef TRAVERSE_METHODS
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_preorder(TRAVERSE_NODE_FN fn) { traverse_preorder(&m_root, fn); }
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_postorder(TRAVERSE_NODE_FN fn) { traverse_postorder(&m_root, fn); }

	template<typename TRAVERSE_NODE_FN>
	inline void traverse_if(TRAVERSE_NODE_FN fn) { traverse_if(&m_root, fn); }

	// readonly
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_preorder(TRAVERSE_NODE_FN fn) const { traverse_preorder(&m_root, fn); }
	template<typename TRAVERSE_NODE_FN>
	inline void traverse_postorder(TRAVERSE_NODE_FN fn) const { traverse_postorder(&m_root, fn); }

	template<typename TRAVERSE_NODE_FN>
	inline void traverse_if(TRAVERSE_NODE_FN fn) const { traverse_if(&m_root, fn); }

#endif
};


#endif