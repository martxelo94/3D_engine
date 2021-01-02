/**
* @file renderer.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Implement Renderer class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#include <pch.h>

RTTI_IMPL(Renderer, Component)

/**
*
* @brief updates obb
* @param md
*/
void Renderer::set_mesh_data(const MeshData* md) {
	// must recompute bv?
	if (md == mesh_data) return;
	mesh_data = md;
	if (md)
		obb = compute_bv_aabb_from_points(md->positions.data(), md->positions.size());
}
/**
*
* @param mb
*/
void Renderer::set_mesh_buffers(const MeshBuffers* mb) { mesh_buffers = mb; }
/**
*
* @param c
*/
void Renderer::set_color(Color c) { color = c; }
