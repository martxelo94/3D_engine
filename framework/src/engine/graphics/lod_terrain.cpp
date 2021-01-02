#include <pch.h>

// return TRUE if plane totally outside frustrum, FALSE if totally or partially inside
bool plane_frustrum_culling(const mat4& MVP) {
	const float OFFSET = 0.1f;
	// top-right, bot-right, bot-left, top-left
	const vec2 corners[4] = {
		{0.5 + OFFSET, 0.5 + OFFSET},
		{0.5 + OFFSET, -0.5 - OFFSET},
		{-0.5 - OFFSET,-0.5 - OFFSET},
		{-0.5 - OFFSET, 0.5 + OFFSET}
	};
	enum FrustrumSide : u8 { inside = 0, right = 1, bot = 2, left = 4, top = 8, out = 16 };
	std::array<u8, 4> side = { inside, inside, inside, inside };
	auto assign_sides = [&corners, &side, &MVP](int idx) -> bool {
		vec4 p_ndc = MVP * vec4{ corners[idx].x, 0, corners[idx].y, 1 };
		// check if crosses near plane
		if (p_ndc.w <= 0) {
			//side[idx] = out;
			return false;
		}
		//assert(p_ndc.w != 0.f);
		p_ndc /= p_ndc.w;
		if (p_ndc.x > 1)
			side[idx] = right;
		else if (p_ndc.x < -1)
			side[idx] = left;
		if (p_ndc.y > 1)
			side[idx] |= top;
		else if (p_ndc.y < -1)
			side[idx] |= bot;
		return side[idx] == (u8)inside;
	};
	// top-right
	if (assign_sides(0))
		return false;
	// bot-right
	if (assign_sides(1))
		return false;
	// bot-left
	if (assign_sides(2))
		return false;
	// top-left
	if (assign_sides(3))
		return false;

	// check that plane inside frustrum, although points outside
	for (int i = 0; i < 4 - 1; ++i) {
		u8 flag0 = side[i];
		for (int j = i + 1; j < 4; ++j) {
			u8 flag1 = side[j];
			u8 flag_dif = flag0 & flag1;
			if (flag_dif == inside)
				return false;
		}
	}

	return true;
}

void draw_terrain_LOD(Shader sh, const MeshBuffers(&buffers)[(int)LOD_terrain_meshes::mesh_count], Camera cam, u8 levels, float snap_precision, bool frustrum_culling)
{
	const float SCALE = 1.f;
	float scale = SCALE;
	vec3 offset = vec3{ 0.5f, 0.f, 0.5f };
	if (snap_precision > 0.f) {
		float snap = 1.f / snap_precision;
		cam.pos = glm::floor(cam.pos * snap) / snap;
	}
	mat4 m_cam_offset = glm::translate(vec3{ cam.pos.x, 0.f, cam.pos.z });	// POPING
	//mat4 m_cam_offset = glm::translate(vec3{ camera_pos.x, 0.f, camera_pos.z });							// FLICKERING
	mat4 m_sc = m_cam_offset * glm::scale(vec3{ scale, scale, scale });
	mat4 m_rot;
	mat4 m_tr;

	// prepare matrices!
	std::vector<mat4> core_matrices(4, mat4());
	std::vector<mat4> side_matrices(8 * (levels + 1), mat4());
	std::vector<mat4> corner_matrices(4 * (levels + 1), mat4());
	std::vector<mat4> ring_matrices(levels, mat4());
	// core
	core_matrices[0] = m_sc * glm::translate(vec3{ -1, 0, 0 } +offset);
	core_matrices[1] = m_sc * glm::translate(vec3{ 0, 0, 0 }+offset);
	core_matrices[2] = m_sc * glm::translate(vec3{ 0, 0, -1 }+offset);
	core_matrices[3] = m_sc * glm::translate(vec3{ -1, 0, -1 }+offset);

	// levels 1-X
	for (int i = 0; i < levels; ++i) {
		//sides
		side_matrices[i * 8 + 0] = m_sc * glm::translate(vec3{ 1, 0, 0 }+offset);
		side_matrices[i * 8 + 1] = m_sc * glm::translate(vec3{ 1, 0, -1 }+offset);
		m_rot = glm::rotate(float(M_PI / 2), vec3{ 0,1,0 });
		side_matrices[i * 8 + 2] = m_sc * glm::translate(vec3{ 0, 0, -2 }+offset) * m_rot;
		side_matrices[i * 8 + 3] = m_sc * glm::translate(vec3{ -1, 0, -2 }+offset)* m_rot;
		m_rot = glm::rotate(float(M_PI), vec3{ 0,1,0 });
		side_matrices[i * 8 + 4] = m_sc * glm::translate(vec3{ -2, 0, -1 }+offset) * m_rot;
		side_matrices[i * 8 + 5] = m_sc * glm::translate(vec3{ -2, 0, 0 }+offset) * m_rot;
		m_rot = glm::rotate(float(-M_PI / 2), vec3{ 0,1,0 });
		side_matrices[i * 8 + 6] = m_sc * glm::translate(vec3{ -1, 0, 1 } +offset) * m_rot;
		side_matrices[i * 8 + 7] = m_sc * glm::translate(vec3{ 0, 0, 1 }+offset)   * m_rot;

		//corners
		corner_matrices[i * 4 + 0] = m_sc * glm::translate(vec3{ 1, 0, 1 }+offset);
		corner_matrices[i * 4 + 1] = m_sc * glm::translate(vec3{ 1, 0, -2 }+offset) * glm::rotate(float(M_PI / 2), vec3{ 0,1,0 });;
		corner_matrices[i * 4 + 2] = m_sc * glm::translate(vec3{ -2, 0, -2 }+offset) * glm::rotate(float(M_PI), vec3{ 0,1,0 });
		corner_matrices[i * 4 + 3] = m_sc * glm::translate(vec3{ -2, 0, 1 } +offset) *  glm::rotate(-float(M_PI / 2), vec3{ 0,1,0 });

		// ring
		ring_matrices[i] = m_sc;

		// update scale
		m_sc = m_cam_offset * glm::scale(vec3{ scale *= 2 });
	}

	// fustrum culling!
#define MAX_LOD_LEVELS (20 + 1)
	std::bitset<4> core_grids;
	std::bitset<MAX_LOD_LEVELS * 8> side_grids;
	std::bitset<MAX_LOD_LEVELS * 4> corner_grids;
	if (frustrum_culling) {
		core_grids.set();
		side_grids.set();
		corner_grids.set();
		const mat4 VP = cam.get_proj() * cam.get_view();

		for (int i = 0; i < core_matrices.size(); ++i)
			core_grids[i] = plane_frustrum_culling(VP * core_matrices[i]);
		for (int i = 0; i < side_matrices.size(); ++i)
			side_grids[i] = plane_frustrum_culling(VP * side_matrices[i]);
		for (int i = 0; i < corner_matrices.size(); ++i)
			corner_grids[i] = plane_frustrum_culling(VP * corner_matrices[i]);
	}


	// lvl 0
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh], "mipmap_level"), 0);
	glBindVertexArray(buffers[0].vao);
	if (core_grids[0] == false) {
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(core_matrices[0])[0][0]);
		glDrawElements(GL_TRIANGLES, buffers[0].index_count, GL_UNSIGNED_INT, 0);
	}
	if (core_grids[1] == false) {
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(core_matrices[1])[0][0]);
		glDrawElements(GL_TRIANGLES, buffers[0].index_count, GL_UNSIGNED_INT, 0);
	}
	if (core_grids[2] == false) {
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(core_matrices[2])[0][0]);
		glDrawElements(GL_TRIANGLES, buffers[0].index_count, GL_UNSIGNED_INT, 0);
	}
	if (core_grids[3] == false) {
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(core_matrices[3])[0][0]);
		glDrawElements(GL_TRIANGLES, buffers[0].index_count, GL_UNSIGNED_INT, 0);
	}

	// sides
	glBindVertexArray(buffers[1].vao);
	for (int i = 0; i < levels; ++i) {
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh], "mipmap_level"), i);
		//right
		if (side_grids[i * 8 + 0] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 0])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}
		if (side_grids[i * 8 + 1] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 1])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}
		//bot
		if (side_grids[i * 8 + 2] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 2])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}
		if (side_grids[i * 8 + 3] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 3])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}
		//left
		if (side_grids[i * 8 + 4] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 4])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}
		if (side_grids[i * 8 + 5] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 5])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}
		//top
		if (side_grids[i * 8 + 6] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 6])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}
		if (side_grids[i * 8 + 7] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(side_matrices[i * 8 + 7])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[1].index_count, GL_UNSIGNED_INT, 0);
		}

	}

	// corners
	glBindVertexArray(buffers[2].vao);
	for (int i = 0; i < levels; ++i) {
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh], "mipmap_level"), i);
		//top-right
		if (corner_grids[i * 4 + 0] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(corner_matrices[i * 4 + 0])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[2].index_count, GL_UNSIGNED_INT, 0);
		}
		//bot-right
		if (corner_grids[i * 4 + 1] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(corner_matrices[i * 4 + 1])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[2].index_count, GL_UNSIGNED_INT, 0);
		}
		//bot-left
		if (corner_grids[i * 4 + 2] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(corner_matrices[i * 4 + 2])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[2].index_count, GL_UNSIGNED_INT, 0);
		}
		//top-left
		if (corner_grids[i * 4 + 3] == false) {
			glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(corner_matrices[i * 4 + 3])[0][0]);
			glDrawElements(GL_TRIANGLES, buffers[2].index_count, GL_UNSIGNED_INT, 0);
		}
	}

	// rings
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh], "ring_case"), 1);
	glBindVertexArray(buffers[3].vao);
	for (int i = 0; i < levels; ++i) {
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh], "mipmap_level"), i);
		glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh], "WORLD"), 1, false, &(ring_matrices[i])[0][0]);
		glDrawElements(GL_TRIANGLES, buffers[3].index_count, GL_UNSIGNED_INT, 0);
	}
	glUniform1i(glGetUniformLocation(graphics.shader_program[sh], "ring_case"), 0);

}

void create_grid_mesh(MeshBuffers* mb, MeshData* md, tri_idx segments)
{
	assert(segments > 0 && "Segments must be greater than 0!");
	assert(mb || md);	// one of those for sure, why call it otherwise?
	const tri_idx vertexCount = (segments + 1) * (segments + 1);
	// defined ccw plane on XY axis
	std::vector<vec3> positions;
	positions.reserve(vertexCount);
	std::vector<vec2> uvs;
	uvs.reserve(vertexCount);
	std::vector<tri_idx> indices;
	indices.reserve(segments * segments * 6);

	float step = 1.f / segments;
	// construct grid
	for (tri_idx y = 0; y < segments + 1; y++) {
		float pos_y = y * step;
		for (tri_idx x = 0; x < segments + 1; x++) {
			float pos_x = x * step;
			uvs.push_back(vec2{ pos_x, pos_y });
			positions.push_back(vec3{ pos_x - 0.5f, 0.f,pos_y - 0.5f });
		}
	}
	// set triangles
	for (tri_idx y = 0; y < segments; y++) {
		tri_idx row = y * (segments + 1);
		tri_idx next_row = (y + 1) * (segments + 1);
		for (tri_idx x = 0; x < segments; x++) {

			indices.push_back(row + x);
			indices.push_back(next_row + x);
			indices.push_back(row + x + 1);

			indices.push_back(row + x + 1);
			indices.push_back(next_row + x);
			indices.push_back(next_row + x + 1);
		}
	}

	std::vector<vec3> normals(vertexCount, vec3{ 0.f, 1.f, 0.f });
	std::vector<Color> colors(vertexCount, Color{ DEFAULT_COLOR });


	// tangent & bitangent computation
	std::vector<vec3> tangents(vertexCount, vec3{});
	std::vector<vec3> bitangents(vertexCount, vec3{});

	compute_tangent_basis(tangents.data(), bitangents.data(), positions.data(), normals.data(), uvs.data(), vertexCount, indices.data(), indices.size());

	if (mb)
		create_buffers(mb, positions.data(), normals.data(), uvs.data(), colors.data(), tangents.data(), bitangents.data(), vertexCount, indices.data(), indices.size());
	if (md) {
		md->positions = std::move(positions);
		md->normals = std::move(normals);
		md->uvs = std::move(uvs);
		md->colors = std::move(colors);
		md->tangents = std::move(tangents);
		md->bitangents = std::move(bitangents);
		md->indices = std::move(indices);
	}
}

void create_terrain_meshes(MeshBuffers(&buffers)[(int)LOD_terrain_meshes::mesh_count], u32 segments, float margin_proportion)
{

	// create center mesh (0)
	create_grid_mesh(&buffers[0], nullptr, segments);
	// create side mesh (1)
	create_grid_with_margins_mesh(&buffers[1], nullptr, segments, margin_proportion, { 1, 0, 0, 0 });
	// create corner mesh (2)
	create_grid_with_margins_mesh(&buffers[2], nullptr, segments, margin_proportion, { 1, 0, 1, 0 });
	// create ring mesh (3)
	create_margins_ring_mesh(&buffers[3], nullptr, segments, margin_proportion);
}

// create a grid with specified marginin any 4 directions
void create_grid_with_margins_mesh(MeshBuffers* mb, MeshData* md, tri_idx segments, float margin_proportion, std::array<bool, 4> directions	/* +X, -X, +Y, -Y */)
{
	assert(segments > 0 && "Segments must be greater than 0!");
	assert(mb || md);	// one of those for sure, why call it otherwise?
	const tri_idx vertexCount = (segments + 1) * (segments + 1);
	// defined ccw plane on XY axis
	std::vector<vec3> positions;
	positions.reserve(vertexCount);
	std::vector<vec2> uvs;
	uvs.reserve(vertexCount);
	std::vector<tri_idx> indices;
	indices.reserve(segments * segments * 6);
	const float margin = margin_proportion / segments;
	float step = 1.f / segments;
	// construct grid
	for (tri_idx y = 0; y < segments + 1; y++) {
		float pos_y = y * step;
		for (tri_idx x = 0; x < segments + 1; x++) {
			float pos_x = x * step;
			uvs.push_back(vec2{ pos_x, pos_y });
			positions.push_back(vec3{ pos_x - 0.5f, 0.f,pos_y - 0.5f });
		}
	}
	// set margins
	if (directions[0]) {
		// +X
		for (int y = 0; y < segments + 1; y++) {
			tri_idx i = y * (segments + 1) + segments;
			positions[i].x -= margin;
			uvs[i].x -= margin;
		}

	}
	if (directions[1]) {
		// -X
		for (int y = 0; y < segments + 1; y++) {
			tri_idx i = y * (segments + 1);
			positions[i].x += margin;
			uvs[i].x += margin;
		}
	}
	if (directions[2]) {
		// +Y
		for (int x = 0; x < segments + 1; x++) {
			tri_idx i = segments * (segments + 1) + x;
			positions[i].z -= margin;
			uvs[i].y -= margin;
		}
	}
	if (directions[3]) {
		// -Y
		for (int x = 0; x < segments + 1; x++) {
			tri_idx i = x;
			positions[i].z += margin;
			uvs[i].y += margin;
		}
	}

	// set triangles
	for (tri_idx y = 0; y < segments; y++) {
		tri_idx row = y * (segments + 1);
		tri_idx next_row = (y + 1) * (segments + 1);
		for (tri_idx x = 0; x < segments; x++) {

			indices.push_back(row + x);
			indices.push_back(next_row + x);
			indices.push_back(row + x + 1);

			indices.push_back(row + x + 1);
			indices.push_back(next_row + x);
			indices.push_back(next_row + x + 1);
		}
	}

	std::vector<vec3> normals(vertexCount, vec3{ 0.f, 1.f, 0.f });
	std::vector<Color> colors(vertexCount, Color{ DEFAULT_COLOR });


	// tangent & bitangent computation
	std::vector<vec3> tangents(vertexCount, vec3{});
	std::vector<vec3> bitangents(vertexCount, vec3{});

	compute_tangent_basis(tangents.data(), bitangents.data(), positions.data(), normals.data(), uvs.data(), vertexCount, indices.data(), indices.size());

	if (mb)
		create_buffers(mb, positions.data(), normals.data(), uvs.data(), colors.data(), tangents.data(), bitangents.data(), vertexCount, indices.data(), indices.size());
	if (md) {
		md->positions = std::move(positions);
		md->normals = std::move(normals);
		md->uvs = std::move(uvs);
		md->colors = std::move(colors);
		md->tangents = std::move(tangents);
		md->bitangents = std::move(bitangents);
		md->indices = std::move(indices);
	}

}


/*
	create a ring around LOD grids to remove snaping artifacts
*/
void create_margins_ring_mesh(MeshBuffers* mb, MeshData* md, tri_idx segments, float margin_proportion)
{
	assert(segments > 0 && "Segments must be greater than 0!");
	assert(mb || md);	// one of those for sure, why call it otherwise?

	const int SIZE = 4;	// dont change
	const int HALF_SIZE = SIZE / 2;
	const u32 in_segments = SIZE * segments;
	const u32 out_segments = HALF_SIZE * segments;
	const u32 vertexInsideCount = SIZE * segments + 1;
	const u32 vertexOutsideCount = (SIZE * segments / 2) + 1;
	const u32 vertexCount = (vertexInsideCount + vertexOutsideCount) * 4 - 8;

	std::vector<vec3> positions;
	positions.reserve(vertexCount);
	std::vector<vec3> normals(vertexCount, vec3{ 0, 1, 0 });
	std::vector<vec2> uvs;
	uvs.reserve(vertexCount);
	std::vector<Color> colors(vertexCount, Color{ DEFAULT_COLOR });
	std::vector<tri_idx> indices;
	const u32 indexCount = segments * SIZE * 4 / 2 * 3 * 3;
	indices.reserve(indexCount);

	const float margin = margin_proportion / segments;
	const float in_step = 1.f / segments;
	const float out_step = in_step * 2;

	vec3 p;

	// Left-Top to Right-Top

	// inner vertices
	float dist = HALF_SIZE - margin;
	p = vec3{ -HALF_SIZE + margin , 0.f, dist };	//corner LEFT-TOP
	positions.push_back(p);
	uvs.push_back({ margin, 1.f - margin });
	for (int x = 1; x < vertexInsideCount - 1; ++x) {
		p = { -HALF_SIZE + x * in_step, 0.f, dist };
		positions.push_back(p);
		uvs.push_back({ float(x) / (vertexInsideCount - 1), 1.f - margin });
	}
	p = { dist, 0.f, dist };						// corner RIGHT-TOP
	positions.push_back(p);
	uvs.push_back({ 1.f - margin, 1.f - margin });

	// outer vertices
	for (int x = 0; x < vertexOutsideCount; ++x) {
		p = { -HALF_SIZE + x * out_step, 0.f, HALF_SIZE };
		positions.push_back(p);
		uvs.push_back({ float(x) / (vertexOutsideCount - 1), 1.f });
	}
	// Right-Top to Right-Bot

	// inner vertices
	for (int y = 1; y < vertexInsideCount - 1; ++y) {
		p = { dist , 0.f, HALF_SIZE - y * in_step };
		positions.push_back(p);
		uvs.push_back({ 1.f - margin, 1.f - float(y) / (vertexInsideCount - 1) });
	}
	p = { dist, 0.f, -HALF_SIZE + margin };		// corner RIGHT-BOT
	positions.push_back(p);
	uvs.push_back({ 1.f - margin, 0.f });

	// outer vertices
	for (int y = 1; y < vertexOutsideCount; ++y) {
		p = { HALF_SIZE, 0.f, HALF_SIZE - y * out_step };
		positions.push_back(p);
		uvs.push_back({ 1.f, 1.f - float(y) / (vertexOutsideCount - 1) });
	}


	// Right-Bot to Left-Bot
	dist = -HALF_SIZE + margin;

	// inner vertices
	for (int x = 1; x < vertexInsideCount - 1; ++x) {
		p = { HALF_SIZE - x * in_step, 0.f, dist };
		positions.push_back(p);
		uvs.push_back({ 1.f - float(x) / (vertexInsideCount - 1), margin });
	}
	p = { dist, 0.f, dist };		// corner LEFT-BOT
	positions.push_back(p);
	uvs.push_back({ margin, margin });

	// outer vertices
	for (int x = 1; x < vertexOutsideCount; ++x) {
		p = { HALF_SIZE - x * out_step, 0.f, -HALF_SIZE };
		positions.push_back(p);
		uvs.push_back({ float(x) / (vertexOutsideCount - 1), 0.f });
	}

	// Left-Bot to Left-Top

	// inner vertices
	for (int y = 1; y < vertexInsideCount - 1; ++y) {
		p = { dist , 0.f, -HALF_SIZE + y * in_step };
		positions.push_back(p);
		uvs.push_back({ margin, float(y) / (vertexInsideCount - 1) });
	}
	// outer vertices
	for (int y = 1; y < vertexOutsideCount - 1; ++y) {
		p = { -HALF_SIZE, 0.f, -HALF_SIZE + y * out_step };
		positions.push_back(p);
		uvs.push_back({ 1.f, float(y) / (vertexOutsideCount - 1) });
	}

	assert(positions.size() == vertexCount);

	// indices
	tri_idx in_top_right = vertexInsideCount - 1;
	tri_idx out_top_right = vertexInsideCount + vertexOutsideCount - 1;
	tri_idx in_bot_right = out_top_right + vertexInsideCount - 1;
	tri_idx out_bot_right = in_bot_right + vertexOutsideCount - 1;
	tri_idx in_bot_left = out_bot_right + vertexInsideCount - 1;
	tri_idx out_bot_left = in_bot_left + vertexOutsideCount - 1;

	tri_idx in_idx = 0;
	tri_idx out_idx = vertexInsideCount;

	for (int i = 0; i < out_segments; ++i) {
		int i_2 = i * 2;
		indices.push_back(i_2 + in_idx);							// in
		indices.push_back(i + out_idx);		// out
		indices.push_back(i_2 + 1 + in_idx);						// in

		indices.push_back(i + out_idx);		// out
		indices.push_back(i + out_idx + 1);	// out
		indices.push_back(i_2 + 1 + in_idx);						// in

		indices.push_back(i_2 + 1 + in_idx);						// in
		indices.push_back(i + out_idx + 1);	// out
		indices.push_back(i_2 + 2 + in_idx);						// in
	}
	// right
	in_idx = out_idx + vertexOutsideCount;
	out_idx = in_idx + vertexInsideCount - 1;

	// first segment
	indices.push_back(in_top_right);
	indices.push_back(out_top_right);
	indices.push_back(in_idx);

	indices.push_back(out_top_right);
	indices.push_back(out_idx);
	indices.push_back(in_idx);

	indices.push_back(in_idx);
	indices.push_back(out_idx);
	indices.push_back(in_idx + 1);


	// remember the reused corner vertex offset (-1)
	for (int i = 1; i < out_segments; ++i) {
		int i_2 = i * 2;
		indices.push_back(i_2 + in_idx - 1);							// in
		indices.push_back(i + out_idx - 1);		// out
		indices.push_back(i_2 + in_idx);						// in

		indices.push_back(i + out_idx - 1);		// out
		indices.push_back(i + out_idx);	// out
		indices.push_back(i_2 + in_idx);						// in

		indices.push_back(i_2 + in_idx);						// in
		indices.push_back(i + out_idx);	// out
		indices.push_back(i_2 + 1 + in_idx);						// in
	}
	// bot

	in_idx = out_idx + vertexOutsideCount - 1;
	out_idx = in_idx + vertexInsideCount - 1;


	// first segment
	indices.push_back(in_bot_right);
	indices.push_back(out_bot_right);
	indices.push_back(in_idx);

	indices.push_back(out_bot_right);
	indices.push_back(out_idx);
	indices.push_back(in_idx);

	indices.push_back(in_idx);
	indices.push_back(out_idx);
	indices.push_back(in_idx + 1);

	for (int i = 1; i < out_segments; ++i) {
		int i_2 = i * 2;
		indices.push_back(i_2 + in_idx - 1);							// in
		indices.push_back(i + out_idx - 1);		// out
		indices.push_back(i_2 + in_idx);						// in

		indices.push_back(i + out_idx - 1);		// out
		indices.push_back(i + out_idx);	// out
		indices.push_back(i_2 + in_idx);						// in

		indices.push_back(i_2 + in_idx);						// in
		indices.push_back(i + out_idx);	// out
		indices.push_back(i_2 + 1 + in_idx);						// in
	}

	// left

	in_idx = out_idx + vertexOutsideCount - 1;
	out_idx = in_idx + vertexInsideCount - 2;


	// first segment
	indices.push_back(in_bot_left);
	indices.push_back(out_bot_left);
	indices.push_back(in_idx);

	indices.push_back(out_bot_left);
	indices.push_back(out_idx);
	indices.push_back(in_idx);

	indices.push_back(in_idx);
	indices.push_back(out_idx);
	indices.push_back(in_idx + 1);
	for (int i = 1; i < out_segments - 1; ++i) {
		int i_2 = i * 2;
		indices.push_back(i_2 + in_idx - 1);							// in
		indices.push_back(i + out_idx - 1);		// out
		indices.push_back(i_2 + in_idx);						// in

		indices.push_back(i + out_idx - 1);		// out
		indices.push_back(i + out_idx);	// out
		indices.push_back(i_2 + in_idx);						// in

		indices.push_back(i_2 + in_idx);						// in
		indices.push_back(i + out_idx);	// out
		indices.push_back(i_2 + 1 + in_idx);						// in
	}

	indices.push_back(out_bot_left + vertexInsideCount - 3);			  // in
	indices.push_back(vertexCount - 1);								// out
	indices.push_back(out_bot_left + vertexInsideCount - 2);			  // in

	indices.push_back(vertexCount - 1);								// out
	indices.push_back(vertexInsideCount);							 // out
	indices.push_back(out_bot_left + vertexInsideCount - 2);			// in

	indices.push_back(out_bot_left + vertexInsideCount - 2);			 // in
	indices.push_back(vertexInsideCount);							// out
	indices.push_back(0);											 // in


	assert(indices.size() == indexCount);

	// tangent & bitangent computation
	std::vector<vec3> tangents(vertexCount, vec3{ 1, 0, 0 });
	std::vector<vec3> bitangents(vertexCount, vec3{ 0, 1, 0 });

	//compute_tangent_basis(tangents.data(), bitangents.data(), positions.data(), normals.data(), uvs.data(), vertexCount, indices.data(), indices.size());

	if (mb) {
		create_buffers(mb, positions.data(), normals.data(), uvs.data(), colors.data(), tangents.data(), bitangents.data(), positions.size(), indices.data(), indices.size());
	}
	if (md) {
		md->positions = std::move(positions);
		md->normals = std::move(normals);
		md->uvs = std::move(uvs);
		md->colors = std::move(colors);
		md->tangents = std::move(tangents);
		md->bitangents = std::move(bitangents);
		md->indices = std::move(indices);
	}

}
