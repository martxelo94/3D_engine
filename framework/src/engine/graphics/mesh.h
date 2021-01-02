/**
* @file mesh.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define mesh
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef MESH_H
#define MESH_H

#define SANITY_CHECK_TANGENTS 1
/*
	Choose triangle index format (switch lines)
#define MESH_IDX_TYPE GL_UNSIGNED_SHORT
*/
#define MESH_IDX_TYPE GL_UNSIGNED_INT
#if MESH_IDX_TYPE == GL_UNSIGNED_INT
using tri_idx = u32;
#elif MESH_IDX_TYPE == GL_UNSIGNED_SHORT
using tri_idx = u16;
#elif MESH_IDX_TYPE == GL_UNSIGNED_BYTE
using tri_idx = u8;
#endif

/*
	Flag by bit position
	Vertex Attributes:	  position (1)
						, normal (2)
						, uvs/texture_coords (3)
						, color (4)
						, tangent (5)
						, bitangent (6)
						, bone weight (7)

	Vertex Formats:		Any flag combination
*/
#define VTX_FLAG_POSITION	(1 << 0)
#define VTX_FLAG_NORMAL		(1 << 1)
#define VTX_FLAG_UV			(1 << 2)
#define VTX_FLAG_COLOR		(1 << 3)
#define VTX_FLAG_TANGENT	(1 << 4)
#define VTX_FLAG_BITANGENT	(1 << 5)
#define VTX_FLAG_BONEINDEX	(1 << 6)
#define VTX_FLAG_BONEWEIGHT	(1 << 7)

#define MAX_BONE_INFLUENCES 4
using vtx_bone_indices = std::array<tri_idx, MAX_BONE_INFLUENCES>;
using vtx_bone_weights = std::array<float, MAX_BONE_INFLUENCES>;

#define VBO_SIZE 6		// p n uv c t b

#define DEFAULT_SEGMENTS 12

struct VertexAttribute
{
	size_t byte_size;
	size_t components;
};

struct MeshData
{
	//define arrays for each attribute
	std::vector<vec3> positions, normals, tangents, bitangents;
	std::vector<vec2> uvs;
	std::vector<Color> colors;
	std::vector<vtx_bone_indices> bone_indices;
	std::vector<vtx_bone_weights> bone_weights;
	std::vector<tri_idx> indices;

	MeshData() = default;

	inline void clear() {
		positions.clear();
		normals.clear();
		tangents.clear();
		bitangents.clear();
		uvs.clear();
		colors.clear();
		bone_indices.clear();
		bone_weights.clear();
		indices.clear();
	}
};

struct MeshBuffers
{
	GLuint vao = 0;
	GLuint vbo[VBO_SIZE] = { 0 };
	GLuint ibo = 0;
	tri_idx index_count = 0;

	MeshBuffers() {}
	// Only move ctors
	MeshBuffers(MeshBuffers && rhs);
	MeshBuffers& operator=(MeshBuffers &&rhs);
	MeshBuffers(const MeshBuffers &) = delete;
	~MeshBuffers();

	// free GPU memory
	inline void clear() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(VBO_SIZE, vbo);
		glDeleteBuffers(1, &ibo);
		index_count = 0;
	}
	inline void bind() const { glBindVertexArray(vao); }
	inline void draw(GLuint mode = GL_TRIANGLES) const { glDrawElements(mode, index_count, MESH_IDX_TYPE, 0); }
};

void create_buffers(MeshBuffers* mb,
	vec3 const* positions, const tri_idx vertex_count,
	tri_idx const* indices, const tri_idx index_count);
void create_buffers(MeshBuffers* mb,
	vec3 const* positions, vec3 const* normals, 
	vec2 const* uvs, Color const* colors, 
	vec3 const* tangents, vec3 const* bitangents, const tri_idx vertex_count,
	tri_idx const* indices, const tri_idx index_count);

void create_buffers(MeshBuffers* mb, const MeshData *md);

bool load_mesh_obj(const char* filepath, MeshBuffers* mb, MeshData* md = nullptr);
std::vector<Triangle> load_triangles_from_obj(const char* filepath);
void create_aabb_mesh(const AABB& ab, MeshBuffers* mb, MeshData* md = nullptr);
void create_sphere_mesh(const Sphere& sph, MeshBuffers* mb, MeshData* md = nullptr);
void create_frustum_mesh(const Frustum &frustum, MeshBuffers* mb, MeshData* md = nullptr);
void create_mesh_from_triangles(MeshBuffers* mb, MeshData* md, const std::vector<Triangle>& triangles);

void create_quad_mesh(MeshBuffers* mb, MeshData* md = nullptr);
void create_plane_mesh(MeshBuffers* mb, MeshData* md = nullptr);
void create_cube_mesh(MeshBuffers* mb, MeshData* md = nullptr);
void create_cone_mesh(MeshBuffers* mb, MeshData* md = nullptr, u16 segments = DEFAULT_SEGMENTS);
void create_cylinder_mesh(MeshBuffers* mb, MeshData* md = nullptr, u16 segments = DEFAULT_SEGMENTS);
void create_sphere_mesh(MeshBuffers* mb, MeshData* md = nullptr, u16 slices = DEFAULT_SEGMENTS, u16 rings = DEFAULT_SEGMENTS / 2);
void create_icosahedron_mesh(MeshBuffers* mb, MeshData* md, u8 tesselation_level = 0);

void compute_tangent_basis(vec3 * tangents, vec3 * bitangents,
	const vec3 *positions, const vec3 *normals, const vec2 *uvs, const tri_idx vertex_count,
	const tri_idx *indices, const tri_idx index_count);
void spherical_uvs(vec2 * uvs, vec3 const* positions, tri_idx vertex_count);
MeshData tesselate(const MeshData& md, u8 level, bool normalized_output = false);

#endif