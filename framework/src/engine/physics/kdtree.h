/**
* @file kdtree.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/03/25	(eus)
* @brief Define kdtree data structure
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#ifndef KDTREE_H
#define KDTREE_H


class kdtree
{
  public:
    struct node {
      private:
		  // data
		  union {
			  float split_point;
			  u32 primitive_index_offset;
		  }uni;
		  struct FourBytes {

			union {
				u32 subnode_index : 30;
				u32 primitive_count : 30;
			}uni;
			u32 axis : 2;
		  }_fourBytes;

      public:
		  void set_leaf(int first_primitive_index,
			  int primitive_count) {
			  uni.primitive_index_offset = first_primitive_index;
			  _fourBytes.uni.primitive_count = primitive_count;
			  _fourBytes.axis = 3;	// this is what actually tells if the node is a leaf
		  }

		  void set_internal(int   axis,
			  float split_point,
			  int   subnode_index) {
			  uni.split_point = split_point;
			  _fourBytes.uni.subnode_index = subnode_index;
			  _fourBytes.axis = axis;
			  assert(axis != 3);
		  }

		  inline bool  is_leaf() const noexcept { return _fourBytes.axis == 3; }			// NO CHANGE INTERFACE
		  inline bool  is_internal() const noexcept { return _fourBytes.axis != 3; }		// NO CHANGE INTERFACE
		  inline int   primitive_count() const noexcept { return _fourBytes.uni.primitive_count; }	// NO CHANGE INTERFACE
		  inline int   primitive_start() const noexcept { return uni.primitive_index_offset; }	// NO CHANGE INTERFACE
		  inline int   next_child() const noexcept { return _fourBytes.uni.subnode_index; }		// NO CHANGE INTERFACE
		  inline float split() const noexcept { return uni.split_point; }				// NO CHANGE INTERFACE
		  inline int   axis() const noexcept { return _fourBytes.axis; }				// NO CHANGE INTERFACE
    };

    struct config {
		float cost_intersection = 80;
		float cost_traversal = 1;
		int max_depth = 16;	// single triangle per node if not max_depth
    };

  private:
	  // data
	  std::vector<node> _nodes;
	  std::vector<AABB> _aabbs;
	  std::vector<size_t> _indices;
	  config _config;

	  std::vector<AABB>          get_aabbs(int depth, int current_depth, int node_idx) const;
  public:
    void build(const std::vector<Triangle>& triangles,
               const config&                config);

    void split(size_t                       node_index,
               const AABB&                  parent_aabb,
               int                          depth,
               const std::vector<Triangle>& all_triangles,
               const std::vector<size_t>&   candidate_triangles);

    std::vector<AABB>          get_aabbs(int depth) const;	// NO DISCARD, NO CHANGE INTERFACE
	inline const std::vector<AABB>&   aabbs() const noexcept { return _aabbs; }	// NO DISCARD, NO CHANGE INTERFACE
	inline const std::vector<size_t>& indices() const noexcept { return _indices; }		// NO DISCARD, NO CHANGE INTERFACE
	inline const std::vector<node>&   nodes() const noexcept { return _nodes; }		// NO DISCARD, NO CHANGE INTERFACE
};


#endif