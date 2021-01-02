/**
* @file mesh.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement mesh
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>

/**
*
* @param rhs
* @return
*/
MeshBuffers::MeshBuffers(MeshBuffers && rhs)
{
	assert(vao == 0);
	//pass data
	vao = rhs.vao;
	std::memcpy(vbo, rhs.vbo, sizeof(GLuint) * VBO_SIZE);
	ibo = rhs.ibo;
	index_count = rhs.index_count;
	//erase data
	rhs.vao = rhs.ibo = rhs.index_count = 0;
	std::memset(rhs.vbo, 0, sizeof(GLuint) * VBO_SIZE);
}
/**
*
* @param rhs
* @return
*/
MeshBuffers& MeshBuffers::operator=(MeshBuffers &&rhs)
{
	//pass data
	glDeleteVertexArrays(1, &vao);
	vao = rhs.vao;
	glDeleteBuffers(VBO_SIZE, vbo);
	std::memcpy(vbo, rhs.vbo, sizeof(GLuint) * VBO_SIZE);
	glDeleteBuffers(1, &ibo);
	ibo = rhs.ibo;
	index_count = rhs.index_count;
	//erase data
	rhs.vao = rhs.ibo = rhs.index_count = 0;
	std::memset(rhs.vbo, 0, sizeof(GLuint) * VBO_SIZE);
	return *this;
}

MeshBuffers::~MeshBuffers() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(VBO_SIZE, vbo);
	glDeleteBuffers(1, &ibo);
}

/**
*
* @param mb
* @param positions
* @param vertex_count
* @param indices
* @param index_count
*/
void create_buffers(MeshBuffers* mb,
	vec3 const* positions,
	const tri_idx vertex_count,
	tri_idx const* indices, const tri_idx index_count)
{
	assert(mb);
	assert(vertex_count);
	mb->clear();

	glGenVertexArrays(1, &mb->vao);
	glBindVertexArray(mb->vao);
	//generate buffers
	glGenBuffers(VBO_SIZE, &mb->vbo[0]);
	//load positions buffer
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertex_count, positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//load index buffer
	glGenBuffers(1, &mb->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mb->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_idx) * index_count, indices, GL_STATIC_DRAW);
	mb->index_count = index_count;
	//Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	pop_gl_errors(__FUNCTION__"|V={p}");
}
/**
*
* @param mb
* @param md
*/
void create_buffers(MeshBuffers* mb,
	vec3 const* positions, vec3 const* normals, vec2 const* uvs, Color const* colors, vec3 const* tangents, vec3 const* bitangents, const tri_idx vertex_count,
	tri_idx const* indices, const tri_idx index_count)
{
	assert(mb);
	mb->clear();

	glGenVertexArrays(1, &mb->vao);
	glBindVertexArray(mb->vao);
	//generate buffers
	glGenBuffers(VBO_SIZE, &mb->vbo[0]);
	pop_gl_errors(__FUNCTION__" -> create buffers");
	//load positions buffer
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertex_count, positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	pop_gl_errors(__FUNCTION__" -> positions");
	//load normals buffer
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertex_count, normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
	pop_gl_errors(__FUNCTION__" -> normals");
	//load uvs buffer
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * vertex_count, uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	pop_gl_errors(__FUNCTION__" -> uvs");
	//load color buffer
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * vertex_count, colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	pop_gl_errors(__FUNCTION__" -> colors");
	//load tangents
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertex_count, tangents, GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	pop_gl_errors(__FUNCTION__" -> tangents");
	//load bitangent
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertex_count, bitangents, GL_STATIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
	pop_gl_errors(__FUNCTION__" -> bitangents");

	//load index buffer
	glGenBuffers(1, &mb->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mb->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_idx) * index_count, indices, GL_STATIC_DRAW);
	pop_gl_errors(__FUNCTION__" -> indices");
	mb->index_count = index_count;
	//Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	pop_gl_errors(__FUNCTION__"|V={p,n,tex,c,t,b}, T={i}");
}

void create_buffers(MeshBuffers* mb, const MeshData *md)
{
	glGenVertexArrays(1, &mb->vao);
	glBindVertexArray(mb->vao);
	//generate buffers
	glGenBuffers(VBO_SIZE, &mb->vbo[0]);
	//load positions buffer
	if (!md->positions.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * md->positions.size(), md->positions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	//load normals buffer
	if (!md->normals.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * md->normals.size(), md->normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
	}
	//load uvs buffer
	if (!md->uvs.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * md->uvs.size(), md->uvs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	//load color buffer
	if (!md->colors.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * md->colors.size(), md->colors.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	}
	//load tangents
	if (!md->tangents.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * md->tangents.size(), md->tangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	//load bitangent
	if (!md->bitangents.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[5]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * md->bitangents.size(), md->bitangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	//load index buffer
	if (!md->indices.empty())
	{
		glGenBuffers(1, &mb->ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mb->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_idx) * md->indices.size(), md->indices.data(), GL_STATIC_DRAW);
		mb->index_count = md->indices.size();
	}

	//Unbind buffers --- NO!!! (allow binding more buffers after this function call...)
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	pop_gl_errors(__FUNCTION__"|ModelData -> MeshData");
}

namespace  // aux namespace
{
	/**
*
* @param line
* @param position_idx
* @return
*/
	bool parse_face(const std::string& line, int * position_idx)
	{
		int matches = sscanf(line.c_str(), "%d %d %d",
			&position_idx[0],
			&position_idx[1],
			&position_idx[2]
		);
		if (matches == 3)
			return true;
		return false;
	}
	struct Vertex {
		vec3 position;

		bool operator <(const Vertex& rhs) const {
			return std::memcmp((const void*)this, (const void*)&rhs, sizeof(Vertex)) > 0;
		}
	};
	/**
*
* @param v
* @param vertexToOutIndex
* @param result
* @return
*/
	bool is_vertex_already_in_use(const Vertex& v, const std::map<Vertex, tri_idx> & vertexToOutIndex, tri_idx & result)
	{
		auto it = vertexToOutIndex.find(v);
		if (it == vertexToOutIndex.end())
			return false;	// does not exists, new vertex

		// vertex used, return index
		result = it->second;
		return true;
	}
	/**
*
* @param positions
* @return
*/
	MeshData index_vbo(const std::vector<vec3>& positions)
	{
		MeshData result;
		std::map<Vertex, tri_idx> vertexToOutIndex;

		//foreach input vertex
		for (tri_idx i = 0; i < positions.size(); i++) {
			Vertex v = { positions[i] };
			tri_idx index;
			bool found = is_vertex_already_in_use(v, vertexToOutIndex, index);
			if (found)	// vertex is already in the vbo, take its index
				result.indices.push_back(index);
			else {
				// new vertex, add to output data
				result.positions.push_back(positions[i]);

				tri_idx new_idx = (tri_idx)result.positions.size() - 1;
				result.indices.push_back(new_idx);
				vertexToOutIndex[v] = new_idx;
			}
		}
		return result;
	}

}
/**
*
* @param filepath
* @param mb
* @param md
* @return
*/
bool load_mesh_obj(const char* filepath, MeshBuffers* mb, MeshData* md)
{

	// obj data read
	std::vector<vec3> tmp_positions;
	// every combination of vertices to give vbo indexer
	std::vector<vec3> in_positions;

	//open file
	std::ifstream file(filepath);
	if (!file.is_open())
		return false;

	//read file
	while (!file.eof()) {
		std::string word;
		//read the firest word of the line
		file >> word;

		// attribute cases
		if (word == "v") {
			// position
			vec3 p;
			file >> word;
			p.x = (float)std::atof(word.c_str());
			file >> word;
			p.y = (float)std::atof(word.c_str());
			file >> word;
			p.z = (float)std::atof(word.c_str());

			tmp_positions.push_back(p);

		}
		else if (word == "f") {
			// face
			int p_idx[3];

			//get whole face line
			std::getline(file, word);

			if (!parse_face(word, p_idx)) {
				return false;
			}

			// decrement indices
			p_idx[0]--;
			p_idx[1]--;
			p_idx[2]--;

			// create new combination of position, normal, uv
			if (p_idx[0] >= 0) {
				in_positions.push_back(tmp_positions[p_idx[0]]);
				in_positions.push_back(tmp_positions[p_idx[1]]);
				in_positions.push_back(tmp_positions[p_idx[2]]);
			}
			else {
				in_positions.push_back(vec3{});
				in_positions.push_back(vec3{});
				in_positions.push_back(vec3{});
			}
		}
		else {
			// probably a commnet
			std::getline(file, word);	// skip all the line
		}
	}
	// create the actual final buffers
	MeshData result = index_vbo(in_positions);

	//outputs
	if (mb)
		create_buffers(mb, &result);
	if (md)
		*md = std::move(result);

	return true;
}
/**
*
* @brief
* @param filepath
* @return
*/
std::vector<Triangle> load_triangles_from_obj(const char* filepath)
{
	// return value
	std::vector<Triangle> triangles;

	// obj data read
	std::vector<vec3> tmp_positions;
	// every combination of vertices to give vbo indexer
	std::vector<vec3> in_positions;

	//open file
	std::ifstream file(filepath);
	if (!file.is_open())
		return triangles;

	//read file
	while (!file.eof()) {
		std::string word;
		//read the firest word of the line
		file >> word;

		// attribute cases
		if (word == "v") {
			// position
			vec3 p;
			file >> word;
			p.x = (float)std::atof(word.c_str());
			file >> word;
			p.y = (float)std::atof(word.c_str());
			file >> word;
			p.z = (float)std::atof(word.c_str());

			tmp_positions.push_back(p);

		}
		else if (word == "f") {
			// face
			int p_idx[3];

			//get whole face line
			std::getline(file, word);

			if (!parse_face(word, p_idx)) {
				return triangles;
			}

			// decrement indices
			p_idx[0]--;
			p_idx[1]--;
			p_idx[2]--;

			// create new combination of position, normal, uv
			if (p_idx[0] >= 0) {
				in_positions.push_back(tmp_positions[p_idx[0]]);
				in_positions.push_back(tmp_positions[p_idx[1]]);
				in_positions.push_back(tmp_positions[p_idx[2]]);
			}
			else {
				in_positions.push_back(vec3{});
				in_positions.push_back(vec3{});
				in_positions.push_back(vec3{});
			}
		}
		else {
			// probably a commnet
			std::getline(file, word);	// skip all the line
		}
	}

	// create triangles
	assert(in_positions.size() % 3 == 0);
	triangles.reserve(in_positions.size() / 3);
	for (int i = 0; i < in_positions.size(); i += 3) {
		Triangle tri{ in_positions[i], in_positions[i + 1],in_positions[i + 2] };
		triangles.push_back(tri);
	}
	return triangles;

}

/**
*
* @param ab
* @param mb
* @param md
*/
void create_aabb_mesh(const AABB& ab, MeshBuffers* mb, MeshData* md)
{
	assert(mb || md);

	const vec3& MAX = ab.max_point;
	const vec3& MIN = ab.min_point;
	const vec3 p[8] = { MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };

	const tri_idx idx[36] = {
		// +X
		0, 1, 2, 2, 3, 0,
		// -X
		5, 4, 7, 7, 6, 5,
		// +Y
		0, 3, 5, 5, 6, 0,
		// -Y
		4, 2, 1, 1, 7, 4,
		// +Z
		0, 6, 7, 7, 1, 0,
		// -Z
		3, 2, 4, 4, 5, 3
	};
	if (mb)
		create_buffers(mb, &p[0], sizeof(p) / sizeof(vec3), &idx[0], sizeof(idx) / sizeof(tri_idx));
	if (md) {
		md->positions.assign(p, p + sizeof(p) / sizeof(vec3));
		md->indices.assign(idx, idx + sizeof(idx) / sizeof(tri_idx));
	}
}
/**
*
* @param sph
* @param mb
* @param md
*/
void create_sphere_mesh(const Sphere& sph, MeshBuffers* mb, MeshData* md)
{
	assert(mb || md);	// one of those for sure, why call it otherwise?
	//calculate vtx count and angles
	const u32 rings = 20;
	const u32 slices = 20;
	const tri_idx vertexCount = rings * slices + 2 + rings;	//extra vertices like poles and overlaped ring begin/end
	const float ringStep = (float)M_PI / rings;	//half circle only
	float sliceStep = (2.0f * (float)M_PI / slices);
	float currRing = ringStep, currSlice = 0.0f;
	std::vector<vec3> positions;
	positions.reserve(vertexCount);
	for (u16 r = 0; r < rings; ++r)
	{
		//calculate height
		const float y = cos(currRing) * sph.radius;
		//calculate radius
		const float radius = sin(currRing) * sph.radius;
		for (u16 s = 0; s < slices; ++s)
		{
			//calculate xz
			const float x = radius * cos(currSlice);
			const float z = radius * sin(currSlice);
			//create the vertex position
			const vec3 pos{ x, y, z };
			//add vertex position
			positions.push_back(sph.center + pos);
			currSlice += sliceStep;
		}
		//extra vertex to avoid weird texture mapping
		positions.push_back(sph.center + vec3{  radius, y, 0.0f });

		currSlice = 0.0f;
		currRing += ringStep;
	}
	//add poles
	positions.push_back(sph.center + vec3{ 0,  sph.radius, 0 });		//top
	positions.push_back(sph.center + vec3{ 0, -sph.radius, 0 });	//bot

	//time of triangulating
	const tri_idx ringTriangleCount = slices * 2 * 3;
	const tri_idx triangleCount = (ringTriangleCount * (rings - 1)) + (slices * 3 * 2);
	std::vector<tri_idx> indices;
	indices.reserve(triangleCount);
	//middle triangles
	for (u16 r = 0; r < rings - 1; ++r)
	{
		for (u16 s = 0; s < slices; ++s)
		{
			const u16 curr = (r * (slices + 1)) + s;
			//quad
			indices.push_back(curr);
			indices.push_back(curr + 1);
			indices.push_back(curr + slices + 1);

			indices.push_back(curr + 1);
			indices.push_back(curr + slices + 1 + 1);
			indices.push_back(curr + slices + 1);
		}
	}
	//poles
	for (u16 s = 0; s < slices; ++s)
	{
		//top
		indices.push_back(s);
		indices.push_back(vertexCount - 2);
		indices.push_back(s + 1);
		//bot
		indices.push_back((rings - 1)* slices + s);
		indices.push_back(vertexCount - 1);
		indices.push_back((rings - 1)* slices + s + 1);
	}

	//check indices are good
	for (size_t i = 0; i < indices.size(); ++i)
		assert((size_t)indices[i] < positions.size());

	// Data completed, load to GPU
	if (mb)
		create_buffers(mb, positions.data(), (tri_idx)positions.size(), indices.data(), (tri_idx)indices.size());
	if (md) {
		md->positions = std::move(positions);
		md->indices = std::move(indices);
	}

}
/**
*
* @param frustum
* @param mb
* @param md
*/
void create_frustum_mesh(const Frustum &frustum, MeshBuffers* mb, MeshData* md)
{
	assert(mb || md);

	std::vector<vec3> p = frustum.get_points();
	assert(p.size() == 8);

	const tri_idx idx[36] = {
		// +X
		0, 1, 5, 5, 4, 0,
		// -X
		7, 6, 2, 2, 3, 7,
		// +Y
		7, 3, 0, 0, 4, 7,
		// -Y
		5, 1, 2, 2, 6, 5,
		// +Z
		3, 2, 1, 1, 0, 3,
		// -Z
		4, 5, 6, 6, 7, 4
	};
	if (mb)
		create_buffers(mb, p.data(), (tri_idx)p.size(), &idx[0], sizeof(idx) / sizeof(tri_idx));
	if (md) {
		md->positions.assign(p.data(), p.data() + p.size());
		md->indices.assign(idx, idx + sizeof(idx) / sizeof(tri_idx));
	}

}

void create_mesh_from_triangles(MeshBuffers* mb, MeshData* md, const std::vector<Triangle>& triangles)
{
	std::vector<vec3> positions;
	positions.reserve(triangles.size() * 3);
	std::vector<tri_idx> indices;
	indices.reserve(triangles.size() * 3);

	for (int i = 0; i < triangles.size(); ++i) {
		positions.push_back(triangles[i].p0);
		positions.push_back(triangles[i].p1);
		positions.push_back(triangles[i].p2);
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}
	if (md) {
		md->positions = positions;
		md->indices = indices;
	}
	if (mb) {
		create_buffers(mb, positions.data(), (tri_idx)positions.size(), indices.data(), (tri_idx)indices.size());
	}
}

#define DEFAULT_COLOR 0xffffffff
void create_quad_mesh(MeshBuffers* mb, MeshData* md)
{
	assert(mb || md);	// one of those for sure, why call it otherwise?
	const tri_idx vertexCount = 4;
	// defined ccw plane on XY axis
	vec3 positions[vertexCount] = {
		{.5f, .5f, 0.f},		// 0
		{-.5f, .5f, 0.f},		// 1
		{-.5f, -.5f, 0.f},	// 2
		{.5f, -.5f, 0.f}		// 3
	};
	vec3 normals[vertexCount] = {
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f}
	};
	vec2 uvs[vertexCount] = {
		{1.f, 1.f},
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f}
	};
	Color colors[vertexCount] = { DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR };
	tri_idx indices[6] = {
		0, 1, 2,
		0, 2, 3
	};
	// tangent & bitangent computation
	vec3 tangents[vertexCount] = { vec3{} };
	vec3 bitangents[vertexCount] = { vec3{} };

	compute_tangent_basis(tangents, bitangents, positions, normals, uvs, vertexCount, indices, 6);

	if (mb)
		create_buffers(mb, positions, normals, uvs, colors, tangents, bitangents, vertexCount, indices, sizeof(indices) / sizeof(tri_idx));
	if (md) {
		md->positions.assign(positions, positions + vertexCount);
		md->normals.assign(normals, normals + vertexCount);
		md->uvs.assign(uvs, uvs + vertexCount);
		md->colors.assign(colors, colors + vertexCount);
		md->tangents.assign(tangents, tangents + vertexCount);
		md->bitangents.assign(bitangents, bitangents + vertexCount);
		md->indices.assign(indices, indices + 6);
	}
}
//like the quad, but Y axis aligned
void create_plane_mesh(MeshBuffers* mb, MeshData* md)
{
	assert(mb || md);	// one of those for sure, why call it otherwise?
	const tri_idx vertexCount = 4;
	// defined ccw plane on XY axis
	vec3 positions[vertexCount] = {
		{.5f, 0.f, .5f,},		// 0
		{-.5f,0.f, .5f,},		// 1
		{-.5f,0.f,-.5f,},	// 2
		{.5f, 0.f,-.5f,}		// 3
	};
	vec3 normals[vertexCount] = {
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f}
	};
	vec2 uvs[vertexCount] = {
		{1.f, 1.f},
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f}
	};
	Color colors[vertexCount] = { DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR };

	tri_idx indices[6] = {
		0, 1, 2,
		0, 2, 3
	};
	// tangent & bitangent computation
	vec3 tangents[vertexCount] = { vec3{} };
	vec3 bitangents[vertexCount] = { vec3{} };

	compute_tangent_basis(tangents, bitangents, positions, normals, uvs, vertexCount, indices, 6);

	if (mb)
		create_buffers(mb, positions, normals, uvs, colors, tangents, bitangents, vertexCount, indices, sizeof(indices) / sizeof(tri_idx));
	if (md) {
		md->positions.assign(positions, positions + vertexCount);
		md->normals.assign(normals, normals + vertexCount);
		md->uvs.assign(uvs, uvs + vertexCount);
		md->colors.assign(colors, colors + vertexCount);
		md->tangents.assign(tangents, tangents + vertexCount);
		md->bitangents.assign(bitangents, bitangents + vertexCount);
		md->indices.assign(indices, indices + 6);
	}
}
void create_cube_mesh(MeshBuffers* mb, MeshData* md) {
	assert(mb || md);	// one of those for sure, why call it otherwise?
	const tri_idx vertexCount = 4 * 6;	// 4 vertices, 6 faces
										// defined ccw plane on XY axis
	vec3 positions[vertexCount] = {
		// front
		{.5f, .5f, .5f},		// 0
		{-.5f, .5f, .5f},		// 1
		{-.5f, -.5f, .5f},	// 2
		{.5f, -.5f, .5f},		// 3
		// back
		{-.5f, .5f, -.5f},	// 4
		{.5f, .5f,  -.5f},	// 5
		{.5f, -.5f, -.5f},	// 6
		{-.5f, -.5f,-.5f},	// 7
		// top
		{.5f, .5f, -.5f},		// 8
		{-.5f, .5f,-.5f},	// 9
		{-.5f, .5f, .5f},		// 10
		{.5f, .5f,  .5f},		// 11
		// bot
		{.5f,  -.5f,  .5f},	// 12
		{-.5f, -.5f,  .5f},	// 13
		{-.5f, -.5f, -.5f},	// 14
		{.5f,  -.5f, -.5f},	// 15
		//left
		{-.5f, .5f,  .5f},		// 16
		{-.5f, .5f, -.5f},	// 17
		{-.5f, -.5f,-.5f},	// 18
		{-.5f, -.5f, .5f},	// 19
		//right
		{.5f, .5f,  -.5f},		// 20
		{.5f, .5f,   .5f},		// 21
		{.5f, -.5f,  .5f},		// 22
		{.5f, -.5f, -.5f}		// 23
	};
	vec3 normals[vertexCount] = {
		//front
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		// back
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		// top
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		// bot
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},
		//left
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},
		//right
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f}

	};
	vec2 uvs[vertexCount] = {
		//front
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f },
		// back
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f },
		// top
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f },
		// bot
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f },
		//left
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f },
		//right
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f }
	};
	Color colors[vertexCount] = { DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR,
								DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR,
								DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR ,
								DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR ,
								DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR ,
								DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR };
	tri_idx indices[36] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};
	// tangent & bitangent computation
	vec3 tangents[vertexCount] = { vec3{} };
	vec3 bitangents[vertexCount] = { vec3{} };

	compute_tangent_basis(tangents, bitangents, positions, normals, uvs, vertexCount, indices, 36);

	if (mb)
		create_buffers(mb, positions, normals, uvs, colors, tangents, bitangents, vertexCount, indices, sizeof(indices) / sizeof(tri_idx));
	if (md) {
		md->positions.assign(positions, positions + vertexCount);
		md->normals.assign(normals, normals + vertexCount);
		md->uvs.assign(uvs, uvs + vertexCount);
		md->colors.assign(colors, colors + vertexCount);
		md->tangents.assign(tangents, tangents + vertexCount);
		md->bitangents.assign(bitangents, bitangents + vertexCount);
		md->indices.assign(indices, indices + 36);
	}
}
void create_cone_mesh(MeshBuffers* mb, MeshData* md, u16 segments)
{
	assert(mb || md);	// one of those for sure, why call it otherwise?
	const tri_idx vertexCount = segments * 3 + 1;
	//positions
	std::vector<vec3> positions{ (size_t)vertexCount };
	const float angleStep = (2.0f * (float)M_PI / segments);
	float angle = 0.0f;
	for (u16 i = 0; i < segments; ++i)
	{
		float x = cos(angle) / 2;
		float y = sin(angle) / 2;
		vec3 bot = vec3{ x, -0.5f, y };
		angle += angleStep;
		positions[i] = { 0.f, 0.5f, 0.f };	//side
		positions[i + segments] = bot;		//side
		positions[i + segments * 2] = bot;	//disk
	}
	//bot
	positions[vertexCount - 1] = vec3{ 0.0f, -0.5f, 0.0f };

	//normals
	std::vector<vec3> normals{ vertexCount };
	angle = 0.0f;
	for (u16 i = 0; i < segments; ++i)
	{
		float x = cos(angle) / 2;
		float y = sin(angle) / 2;
		vec3 dir = vec3{ x, -0.5f,  y };
		vec3 tmp = vec3{ 0.f, 0.5f, 0.f } -dir;
		tmp = glm::cross(tmp, dir);
		tmp = glm::normalize(tmp);
		dir = glm::normalize(dir);
		dir = glm::cross(dir, tmp);
		angle += angleStep;
		normals[i] = dir;					//side
		normals[i + segments] = dir;		//side
		normals[i + segments * 2] = { 0.f, -1.f, 0.f };	//disk
	}
	normals[vertexCount - 1] = { 0.f, -1.f, 0.f };
	//uvs
	angle = 0.0f;
	std::vector<vec2> uvs{ (size_t)vertexCount };
	for (u16 i = 0; i < segments; ++i)
	{
		float x = cos(angle) / 2 + 0.5f;
		float y = sin(angle) / 2 + 0.5f;
		uvs[i] = { angle / (2 * M_PI), 1.f };
		uvs[i + segments] = { angle / (2 * M_PI), 0.f };
		angle += angleStep;
		uvs[i + segments * 2] = { x, y };

	}
	uvs[vertexCount - 2] = uvs[vertexCount - 1] = vec2{ 0.5f, 0.5f };
	//colors (by default)
	std::vector<Color> colors(vertexCount, Color{ DEFAULT_COLOR });
	//indices
	std::vector<tri_idx> indices;
	indices.reserve(segments * 3 * 4);
	for (u16 i = 0; i < segments - 1; ++i)
	{
		//bot
		indices.push_back(i + segments * 2); indices.push_back(i + segments * 2 + 1);  indices.push_back(vertexCount - 1);
		//side quad
		indices.push_back(i); indices.push_back(i + 1); indices.push_back(i + segments);
		indices.push_back(i + segments); indices.push_back(i + 1); indices.push_back(i + segments + 1);
	} //last segments
	//bot
	indices.push_back(segments * 2);  indices.push_back(vertexCount - 1); indices.push_back(segments * 3 - 1);
	//side
	indices.push_back(0); indices.push_back(segments); indices.push_back(segments - 1);
	indices.push_back(segments); indices.push_back(segments * 2 - 1); indices.push_back(segments - 1);

	// tangent & bitangent computation
	std::vector<vec3> tangents = { vertexCount, vec3{} };
	std::vector<vec3> bitangents = { vertexCount, vec3{} };

	compute_tangent_basis(tangents.data(), bitangents.data(), positions.data(), normals.data(), uvs.data(), vertexCount, indices.data(), indices.size());

	// Data completed, load to GPU
	if (mb)
		create_buffers(mb, positions.data(), normals.data(), uvs.data(), colors.data(), tangents.data(), bitangents.data(), positions.size(), indices.data(), indices.size());
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
void create_cylinder_mesh(MeshBuffers* mb, MeshData* md, u16 segments)
{
	assert(mb || md);	// one of those for sure, why call it otherwise?
	const tri_idx vertexCount = segments * 2 * 2 + 2;
	//positions
	std::vector<vec3> positions{ (size_t)vertexCount };
	const float angleStep = (2.0f * (float)M_PI / segments);
	float angle = 0.0f;
	for (u16 i = 0; i < segments; ++i)
	{
		float x = cos(angle) / 2;
		float y = sin(angle) / 2;
		vec3 top = vec3{ x, 0.5f,  y };
		vec3 bot = vec3{ x, -0.5f, y };
		angle += angleStep;
		positions[i] = top;					//side
		positions[i + segments] = bot;		//side
		positions[i + segments * 2] = top;	//disk
		positions[i + segments * 3] = bot;	//disk
	}
	//top	
	positions[vertexCount - 2] = vec3{ 0.0f, 0.5f, 0.0f };
	//bot
	positions[vertexCount - 1] = vec3{ 0.0f, -0.5f, 0.0f };

	//normals
	std::vector<vec3> normals{ vertexCount };
	angle = 0.0f;
	for (u16 i = 0; i < segments; ++i)
	{
		float x = cos(angle);
		float y = sin(angle);
		vec3 dir = vec3{ x, 0.f,  y };
		angle += angleStep;
		normals[i] = dir;					//side
		normals[i + segments] = dir;		//side
		normals[i + segments * 2] = { 0.f, 1.f, 0.f };	//disk
		normals[i + segments * 3] = { 0.f, -1.f, 0.f };	//disk
	}
	normals[vertexCount - 2] = { 0.f, 1.f, 0.f };
	normals[vertexCount - 1] = { 0.f, -1.f, 0.f };
	//uvs
	angle = 0.0f;
	std::vector<vec2> uvs{ (size_t)vertexCount };
	for (u16 i = 0; i < segments; ++i)
	{
		float x = cos(angle) / 2 + 0.5f;
		float y = sin(angle) / 2 + 0.5f;
		uvs[i] = { angle / (2 * M_PI), 1.f };
		uvs[i + segments] = { angle / (2 * M_PI), 0.f };
		angle += angleStep;
		uvs[i + segments * 2] = { 1.0f - x, y };
		uvs[i + segments * 3] = { x, y };

	}
	uvs[vertexCount - 2] = uvs[vertexCount - 1] = vec2{ 0.5f, 0.5f };
	//colors (by default)
	std::vector<Color> colors(vertexCount, Color{ DEFAULT_COLOR });
	//indices
	std::vector<tri_idx> indices;
	indices.reserve(segments * 3 * 4);
	for (u16 i = 0; i < segments - 1; ++i)
	{
		//top
		indices.push_back(i + segments * 2);  indices.push_back(vertexCount - 2); indices.push_back(i + segments * 2 + 1);
		//bot
		indices.push_back(i + segments * 3); indices.push_back(i + segments * 3 + 1); indices.push_back(vertexCount - 1);
		//side quad
		indices.push_back(i); indices.push_back(i + 1); indices.push_back(i + segments);
		indices.push_back(i + segments); indices.push_back(i + 1); indices.push_back(i + segments + 1);
	}
	//last segments
	//top
	indices.push_back(segments * 2); indices.push_back(segments * 3 - 1); indices.push_back(vertexCount - 2);
	//bot
	indices.push_back(segments * 3);  indices.push_back(vertexCount - 1); indices.push_back(segments * 4 - 1);
	//side
	indices.push_back(0); indices.push_back(segments); indices.push_back(segments - 1);
	indices.push_back(segments); indices.push_back(segments * 2 - 1); indices.push_back(segments - 1);

	// tangent & bitangent computation
	std::vector<vec3> tangents = { vertexCount, vec3{} };
	std::vector<vec3> bitangents = { vertexCount, vec3{} };

	compute_tangent_basis(tangents.data(), bitangents.data(), positions.data(), normals.data(), uvs.data(), vertexCount, indices.data(), indices.size());


	// Data completed, load to GPU
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

void spherical_uvs(vec2 * uvs, vec3 const* positions, tri_idx vertex_count)
{
	for (tri_idx i = 0; i < vertex_count; i++) {
		const vec3 &p = positions[i];
		float angle = atan2(p.z, p.x);
		float x = 0.5f + (angle / float(M_PI * 2));
		float y = 0.5f - (asin(p.y) / float(M_PI * 2));
		assert(x >= 0 && x <= 1);
		assert(y >= 0 && y <= 1);
		uvs[i] = { x, y };
	}
}
void create_sphere_mesh(MeshBuffers* mb, MeshData* md, u16 slices, u16 rings)
{
#define SPHERE_RADIUS 1.f
	assert(mb || md);	// one of those for sure, why call it otherwise?
	//calculate vtx count and angles
	const tri_idx vertexCount = rings * slices + 2 + rings;	//extra vertices like poles and overlaped ring begin/end
	const float ringStep = (float)M_PI / rings;	//half circle only
	float sliceStep = (2.0f * (float)M_PI / slices);
	float currRing = ringStep, currSlice = 0.0f;
	std::vector<vec3> positions;
	positions.reserve(vertexCount);
	for (u16 r = 0; r < rings; ++r)
	{
		//calculate height
		const float y = cos(currRing) * SPHERE_RADIUS;
		//calculate radius
		const float radius = sin(currRing) * SPHERE_RADIUS;
		for (u16 s = 0; s < slices; ++s)
		{
			//calculate xz
			const float x = radius * cos(currSlice);
			const float z = radius * sin(currSlice);
			//create the vertex position
			const vec3 pos{ x, y, z };
			//add vertex position
			positions.push_back(pos);
			currSlice += sliceStep;
		}
		//extra vertex to avoid weird texture mapping
		positions.push_back(vec3{ radius, y, 0.0f });

		currSlice = 0.0f;
		currRing += ringStep;
	}
	//add poles
	positions.push_back(vec3{ 0,  SPHERE_RADIUS, 0 });		//top
	positions.push_back(vec3{ 0, -SPHERE_RADIUS, 0 });	//bot

	//normals are pointing out the sphere perpendicularly, so are the positions itself, but must normalize
	std::vector<vec3> normals;
	normals.reserve(positions.size());
	for (const vec3& p : positions)
		normals.push_back(glm::normalize(p));
	//uvs ... how to map this...
#if 1
	currRing = currSlice = 0.0f;
	std::vector<vec2> uvs;
	uvs.reserve(vertexCount);
	currSlice = 2.0f * (float)M_PI;
	sliceStep = 2.0f * (float)M_PI / (slices + 1);
	currRing = (float)M_PI - ringStep;
	for (u16 r = 0; r < rings; ++r)
	{
		for (u16 s = 0; s < slices + 1; ++s)
		{
			//create the vertex position
			vec2 uv{ (float)currSlice / (2.0f * (float)M_PI), currRing / (float)M_PI };
			//add vertex position
			uvs.push_back(uv);
			currSlice -= sliceStep;
		}
		currSlice = 2.0f * (float)M_PI;
		currRing -= ringStep;
	}
	//poles
	uvs.push_back({ 0.5f, 1.0f });	//top
	uvs.push_back({ 0.5f, 0.0f });	//bot
#else
	std::vector<vec2> uvs(positions.size(), vec2{});
	spherical_uvs(uvs.data(), positions.data(), uvs.size());
#endif
	//colors (by default)
	std::vector<Color> colors(vertexCount, Color{ DEFAULT_COLOR });


	//time of triangulating
	const tri_idx ringTriangleCount = slices * 2 * 3;
	const tri_idx triangleCount = (ringTriangleCount * (rings - 1)) + (slices * 3 * 2);
	std::vector<tri_idx> indices;
	indices.reserve(triangleCount);
	//middle triangles
	for (u16 r = 0; r < rings - 1; ++r)
	{
		for (u16 s = 0; s < slices; ++s)
		{
			const u16 curr = (r * (slices + 1)) + s;
			//quad
			indices.push_back(curr);
			indices.push_back(curr + 1);
			indices.push_back(curr + slices + 1);

			indices.push_back(curr + 1);
			indices.push_back(curr + slices + 1 + 1);
			indices.push_back(curr + slices + 1);
		}
	}
	//poles
	for (u16 s = 0; s < slices; ++s)
	{
		//top
		indices.push_back(s);
		indices.push_back(vertexCount - 2);
		indices.push_back(s + 1);
		//bot
		indices.push_back((rings - 1)* slices + s);
		indices.push_back(vertexCount - 1);
		indices.push_back((rings - 1)* slices + s + 1);
	}

	//check indices are good
	for (size_t i = 0; i < indices.size(); ++i)
		assert((size_t)indices[i] < positions.size());

	// tangent & bitangent computation
	std::vector<vec3> tangents = { vertexCount, vec3{} };
	std::vector<vec3> bitangents = { vertexCount, vec3{} };

	compute_tangent_basis(tangents.data(), bitangents.data(), positions.data(), normals.data(), uvs.data(), vertexCount, indices.data(), indices.size());

	// Data completed, load to GPU
	if (mb)
		create_buffers(mb, positions.data(), normals.data(), uvs.data(), colors.data(), tangents.data(), bitangents.data(), positions.size(), indices.data(), indices.size());
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
void create_icosahedron_mesh(MeshBuffers* mb, MeshData* md, u8 tesselation_level)
{
	const float e = 1.f + (float)sqrt(5) / 4;
	const tri_idx vertexCount = 12;

	vec3 positions[vertexCount] = {
		{-1, e, 0}, {1, e, 0}, {-1, -e, 0}, {1, -e, 0},	// XY plane
		{0, -1, e}, {0, 1, e}, {0, -1, -e}, {0, 1, -e},	// YZ plane
		{e, 0, -1}, {e, 0, 1}, {-e, 0, -1}, {-e, 0, 1}	// XZ plane
	};
	//normalize
	for (int i = 0; i < vertexCount; i++)
		positions[i] = glm::normalize(positions[i]);
	vec3 normals[vertexCount] = {};
	std::memcpy(normals, positions, sizeof(vec3) * vertexCount);

	//uvs : calculate spherical coordinates
	vec2 uvs[vertexCount] = { {0.f, 0.f} };
	spherical_uvs(uvs, positions, vertexCount);

	//colors (by default)
	const Color colors[vertexCount] = {
		DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR,
		DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR,
		DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR
	};
	//indices
	const tri_idx index_count = 20 * 3;
	tri_idx indices[index_count] = {
		0, 10, 11,		// 0
		0, 11, 5,		// 1
		0, 5, 1,		// 2
		0, 1, 7,		// 3
		0, 7, 10,		// 4
		3, 8, 9,		// 5
		3, 6, 8,		// 6
		3, 2, 6,		// 7
		3, 4, 2,		// 8
		3, 9, 4,		// 9
		2, 11, 10,		// 10
		2, 4, 11,		// 11
		4, 5, 11,		// 12
		4, 9, 5,		// 13
		9, 1, 5,		// 14
		9, 8, 1,		// 15
		8, 7, 1,		// 16
		8, 6, 7,		// 17
		7, 6, 10,		// 18
		6, 2, 10		// 19
	};

	// tangent & bitangent computation
	vec3 tangents[vertexCount] = { vec3{} };
	vec3 bitangents[vertexCount] = { vec3{} };

	compute_tangent_basis(tangents, bitangents, positions, normals, uvs, vertexCount, indices, index_count);

	// Data completed, load to GPU
	if (md) {
		md->positions.assign(positions, positions + vertexCount);
		md->normals.assign(normals, normals + vertexCount);
		md->uvs.assign(uvs, uvs + vertexCount);
		md->colors.assign(colors, colors + vertexCount);
		md->tangents.assign(tangents, tangents + vertexCount);
		md->bitangents.assign(bitangents, bitangents + vertexCount);
		md->indices.assign(indices, indices + index_count);

		if (tesselation_level > 0)
		{
			*md = tesselate(*md, tesselation_level, true);
			spherical_uvs(&md->uvs[0], &md->positions[0], md->positions.size());
		}

		if (mb)
			create_buffers(mb,
				md->positions.data(), md->normals.data(), md->uvs.data(), md->colors.data(), md->tangents.data(), md->bitangents.data(), md->positions.size(),
				md->indices.data(), md->indices.size());
	}
	else if (mb) {
		create_buffers(mb, positions, normals, uvs, colors, tangents, bitangents, vertexCount, indices, index_count);
	}

}

void compute_tangent_basis(vec3 * tangents, vec3 * bitangents,
	const vec3 *positions, const vec3 *normals, const vec2 *uvs, const tri_idx vertex_count,
	const tri_idx *indices, const tri_idx index_count)
{
	for (tri_idx i = 0; i < index_count; i += 3) {
		tri_idx i0 = indices[i];
		tri_idx i1 = indices[i + 1];
		tri_idx i2 = indices[i + 2];

		vec3 p0 = positions[i0];
		vec3 p1 = positions[i1];
		vec3 p2 = positions[i2];

		vec3 dPos0 = p1 - p0;
		vec3 dPos1 = p2 - p0;

		vec2 t0 = uvs[i0];
		vec2 t1 = uvs[i1];
		vec2 t2 = uvs[i2];

		vec2 dTex0 = t1 - t0;
		vec2 dTex1 = t2 - t0;

		float det = glm::determinant(mat2(dTex0, dTex1));
		if (glm::epsilonEqual(det, 0.f, FLT_EPSILON))
		{
			continue;
		}
		float invDet = 1.f / det;

		vec3 T = invDet * (dTex1.y * dPos0 - dTex0.y * dPos1);
		vec3 B = invDet * (dTex0.x * dPos1 - dTex1.x * dPos0);

		tangents[i0] += T;
		tangents[i1] += T;
		tangents[i2] += T;

		bitangents[i0] += B;
		bitangents[i1] += B;
		bitangents[i2] += B;
	}

	// Gram-Schmidt
	for (tri_idx i = 0; i < vertex_count; i++) {
		vec3 T = tangents[i];
		vec3 N = normals[i];
		T = T - (glm::dot(T, N) * N);

		vec3 B = bitangents[i];
		B = B - glm::dot(N, B) * N - glm::dot(T, B) * T;

#if SANITY_CHECK_TANGENTS
		// sanity check for 0 length
		float mag = glm::dot(T, T);
		if (glm::epsilonNotEqual(mag, 0.f, FLT_EPSILON))
#endif
			T = glm::normalize(T);
#if SANITY_CHECK_TANGENTS
		mag = glm::dot(B, B);
		if (glm::epsilonNotEqual(mag, 0.f, FLT_EPSILON))
#endif
			B = glm::normalize(B);
		tangents[i] = T;
		bitangents[i] = B;
	}

}

MeshData tesselate(const MeshData& md, u8 level, bool normalized_output)
{
	if (!level)
		return md;

	MeshData res;
	res.positions = md.positions;
	res.normals = md.normals;
	res.uvs = md.uvs;
	res.colors = md.colors;
	res.tangents = md.tangents;
	res.bitangents = md.bitangents;
	res.indices = md.indices;

	auto key = [](tri_idx a, tri_idx b) -> u64 {
		return (static_cast<u64>(a) << 32) | b;
	};
	for (u8 lvl = 0; lvl < level; lvl++) {
		std::unordered_map<u64, tri_idx> edges_interpolated;

		auto interp = [&res, &normalized_output, &edges_interpolated, &key](tri_idx a, tri_idx b) -> tri_idx {
			u64 hash_1 = key(a, b);
			u64 hash_2 = key(b, a);
			if (edges_interpolated.find(hash_1) != edges_interpolated.end() && edges_interpolated.find(hash_2) != edges_interpolated.end())
				return edges_interpolated.at(hash_1);
			vec3 p = (res.positions[a] + res.positions[b]) / 2.f;
			vec3 n = (res.normals[a] + res.normals[b]) / 2.f;
			vec2 t = (res.uvs[a] + res.uvs[b]) / 2.f;
			Color c0 = res.colors[a], c1 = res.colors[b];
			Color c(((u32)c0.r + (u32)c1.r) / 2, ((u32)c0.g + (u32)c1.g) / 2, ((u32)c0.b + (u32)c1.b) / 2, ((u32)c0.a + (u32)c1.a) / 2);
			vec3 tan = (res.tangents[a] + res.tangents[b]) / 2.f;
			vec3 bitan = (res.bitangents[a] + res.bitangents[b]) / 2.f;
			if (normalized_output)
				p = glm::normalize(p);
			res.positions.push_back(p);
			res.normals.push_back(n);
			res.uvs.push_back(t);
			res.colors.push_back(c);
			res.tangents.push_back(tan);
			res.bitangents.push_back(bitan);
			tri_idx index = res.positions.size() - 1;
			edges_interpolated[hash_1] = edges_interpolated[hash_2] = index;
			return index;
		};

		std::vector<tri_idx> new_indices;
		new_indices.reserve(res.indices.size());
		for (tri_idx i = 0; i < res.indices.size(); i += 3) {
			tri_idx i0 = res.indices[i];
			tri_idx i1 = res.indices[i + 1];
			tri_idx i2 = res.indices[i + 2];

			tri_idx a = interp(i0, i1);
			tri_idx b = interp(i1, i2);
			tri_idx c = interp(i2, i0);

			new_indices.push_back(i0); new_indices.push_back(a); new_indices.push_back(c);
			new_indices.push_back(i1); new_indices.push_back(b); new_indices.push_back(a);
			new_indices.push_back(i2); new_indices.push_back(c); new_indices.push_back(b);
			new_indices.push_back(a); new_indices.push_back(b); new_indices.push_back(c);

			//assert(a != i0 && a != i1 && a != i2);
			//assert(b != i0 && b != i1 && b != i2);
			//assert(c != i0 && c != i1 && c != i2);
		}
		res.indices = std::move(new_indices);
	}
	return res;
}
