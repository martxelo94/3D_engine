/**
* @file graphics.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define graphics
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H




enum Primitive : int {no_primitive = -1, primitive_quad, primitive_cube, primitive_sphere, primitive_count };


struct Graphics
{
	//load data and initialize systems
	void init();
	//free data and systems (openGL, SDL)
	void free();


	//shader controll
	void delete_shaders();
	void compile_shaders();

	// debug
	void debug_line(const vec3& start, const vec3& end);
	void draw_debug_lines(const mat4 & m, Color color, float line_width = 1.f);

	//window stuff (dont set)
	Window window;

	//shader stuff
	GLuint shader_program[Shader::shader_count] = { 0 };

	//textures
	GLuint tex[Texture::texture_count] = { 0 };

	// primitives
	MeshBuffers primitives[primitive_count];

private:
	std::vector<vec3> m_debug_lines;

};
extern Graphics graphics;	// TODO: make singleton

#ifndef UTILS
// outputs error message, returns number of errors
unsigned pop_gl_errors(const char* msg_format, ...);
// get monitor size
ivec2 get_monitor_size();
#endif


#ifndef IMMEDIATE_DRAWING
// draw primitives
void draw_obb(const AABB& obb, const mat4& mvp, Color color);
void draw_aabb(const AABB& ab, const mat4& vp, Color color);
void draw_sphere(const Sphere& sph, const mat4& vp, Color color);
void draw_triangle(const Triangle& tri, const mat4& vp, Color color);

#endif

#endif