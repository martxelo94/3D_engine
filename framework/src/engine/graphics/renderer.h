/**
* @file renderer.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare Renderer class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/
#ifndef RENDERER_H
#define RENDERER_H

class Renderer : public Component
{
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

	RTTI_DCL
};


#endif