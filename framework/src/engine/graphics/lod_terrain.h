/*

	TERRAIN

	Geometry Clipmaps

*/

#ifndef LOD_TERRAIN_H
#define LOD_TERRAIN_H

// center, side, corner, ring // (remember the order)
enum class LOD_terrain_meshes : int {center = 0, side, corner, ring, mesh_count};

void create_grid_mesh(MeshBuffers* mb, MeshData* md = nullptr, tri_idx segments = 1);
void create_grid_with_margins_mesh(MeshBuffers* mb, MeshData* md, tri_idx segments, float margin_proportion, std::array<bool, 4> directions	/* +X, -X, +Y, -Y */);
void create_margins_ring_mesh(MeshBuffers* mb, MeshData* md, tri_idx segments, float margin_proportion);
void create_terrain_meshes(MeshBuffers(&buffers)[(int)LOD_terrain_meshes::mesh_count], u32 segments, float margin_proportion);

void draw_terrain_LOD(Shader sh, const MeshBuffers(&buffers)[(int)LOD_terrain_meshes::mesh_count], Camera cam, u8 levels, float snap_precision, bool frustrum_culling);

#endif