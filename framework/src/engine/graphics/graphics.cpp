/**
* @file graphics.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement graphics
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include <pch.h>

Graphics graphics;

/**
*
* @brief initialize graphics system
*/
void Graphics::init()
{
	int error_code = -1;
	// initialize GLFW
	error_code = glfwInit();
	if (!error_code) {
		// failed initializing GLFW
		std::cerr << "glfwInit failed with code \"" << error_code << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	// set OpenGL context version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

	//initialize window & gl_context
	//window.size = { WINDOW_X, WINDOW_Y };
	window.size = get_monitor_size();
	window.glfwWindow = glfwCreateWindow(window.size.x, window.size.y, "MARTXELO's FRAMEWORK", NULL, NULL);
	glfwGetWindowPos(window.glfwWindow, &window.position.x, &window.position.y);
	
	if (!window.glfwWindow) {
		// failed creating window
		glfwTerminate();
		std::cerr << io_red << "glfwCreateWindow failed creating a WINDOW" << io_white << std::endl;
		exit(EXIT_FAILURE);
	}
	// create context
	glfwMakeContextCurrent(window.glfwWindow);
	// initialize GLAD lib
	error_code = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!error_code) {
		glfwTerminate();
		std::cerr << io_red << "gladLoadGLLoader failed with code \"" << error_code << "\"" << io_white << std::endl;
		exit(EXIT_FAILURE);
	}
	error_code = gladLoadGL();
	if (!error_code) {
		glfwTerminate();
		std::cerr << io_red << "gladLoadGL failed with code \"" << error_code << "\"" << io_white << std::endl;
		exit(EXIT_FAILURE);
	}
	// set buffer swap interval
	glfwSwapInterval(1);

	// set viewport
	glViewport(0, 0, window.size.x, window.size.y);

	//openGL alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);	// Does not work with G-BUFFER!!!
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
#if 0	// Debug info
	GLint range[2];
	glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, range);
	printf("SMOOTH RANGE min=%d max=%d\n", range[0], range[1]);
	glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range);
	printf("ALIASED RANGE min=%d max=%d\n", range[0], range[1]);
#endif

	pop_gl_errors(__FUNCTION__" -> OpenGL Environment Initialization");

	//SHADER INITIALIZATION
	compile_shaders();

	// PRIMITIVE CREATION
	create_quad_mesh(&primitives[primitive_quad], nullptr);
	create_aabb_mesh(AABB{ vec3{-1.f}, vec3{1.f} }, &primitives[primitive_cube]);
	create_sphere_mesh(Sphere{ vec3{}, 1.f }, &primitives[primitive_sphere]);

	// IMGUI initialization
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		const ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow, true);
		ImGui_ImplOpenGL3_Init("#version 420");
		//ImGui::StyleColorsClassic();
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
	}

	//check for GL errors
	pop_gl_errors(__FUNCTION__);
}
/**
*
* @brief shutdown graphics system
* @return
*/
void Graphics::free()
{
	//free ImGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//delete all textures
	for (int i = 0; i < Texture::texture_count; ++i)
		glDeleteTextures(1, &tex[i]);

	//delete shaders
	delete_shaders();

	//free window & gl_context
	glfwDestroyWindow(window.glfwWindow);
	glfwTerminate();
}



// SHADERS

	/**
*
* @brief delete all shader programs from memory
*/
void Graphics::delete_shaders()
{
	for (int i = 0; i < Shader::shader_count; i++)
		glDeleteProgram(shader_program[i]);
}
/**
*
* @brief hardcoded initialization of every shader program
*/
void Graphics::compile_shaders()
{
	GLuint vertex_shader, fragment_shader;
	// RAW
	if(!compile_program_vert_frag(shader_program[Shader::sh_raw], "../resources/shaders/raw.vert", "../resources/shaders/raw.frag"))
		std::cerr << io_red << "sh_raw COMPILE ERROR!!!" << io_white << std::endl;
	// COLOR
	if(!compile_program_vert_frag(shader_program[Shader::sh_color], "../resources/shaders/color.vert", "../resources/shaders/color.frag"))
		std::cerr << io_red << "sh_color COMPILE ERROR!!!" << io_white << std::endl;
	// DEPTH COLOR
	if(!compile_program_vert_frag(shader_program[Shader::sh_depth_color], "../resources/shaders/basic.vert", "../resources/shaders/depth_color.frag"))
		std::cerr << io_red << "sh_depth_color COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_depth_color]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_depth_color], "shadowMap"), 0);
		pop_gl_errors("glUniform1i -> shadowMap");
	}
	// RED COLOR
	if(!compile_program_vert_frag(shader_program[Shader::sh_red_color], "../resources/shaders/basic.vert", "../resources/shaders/red_color.frag"))
		std::cerr << io_red << "sh_red_color COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_red_color]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_red_color], "shadowMap"), 0);
		pop_gl_errors("glUniform1i -> shadowMap");
	}
	// ALPHA COLOR
	if(!compile_program_vert_frag(shader_program[Shader::sh_alpha_color], "../resources/shaders/raw.vert", "../resources/shaders/alpha_color.frag"))
		std::cerr << io_red << "sh_alpha_color COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_alpha_color]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_alpha_color], "diffuse_texture"), 0);
		pop_gl_errors("glUniform1i -> diffuse_texture");
	}
	// FORWARD LIGHT
	if(!compile_program_vert_frag(shader_program[Shader::sh_forward_light], "../resources/shaders/forward_light.vert", "../resources/shaders/forward_light.frag"))
		std::cerr << io_red << "sh_forward_light COMPILE ERROR!!!" << io_white << std::endl;
	// SKYBOX
	if (!compile_program_vert_frag(shader_program[Shader::sh_skybox], "../resources/shaders/skybox.vert", "../resources/shaders/cubemap.frag"))
		std::cerr << io_red << "sh_skybox COMPILE ERROR!!!" << io_white << std::endl;
	// GEOMETRY PASS
	if(!compile_program_vert_frag(shader_program[Shader::sh_geometry_pass], "../resources/shaders/geometry_pass.vert", "../resources/shaders/geometry_pass.frag"))
		std::cerr << io_red << "sh_geometry_pass COMPILE ERROR!!!" << io_white << std::endl;
	// LIGHTING PASS
	if(!compile_program_vert_frag(shader_program[Shader::sh_lighting_pass], "../resources/shaders/lighting_pass.vert", "../resources/shaders/lighting_pass.frag"))
		std::cerr << io_red << "sh_lighting_pass COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_lighting_pass]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_lighting_pass], "gPosition"), 0);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_lighting_pass], "gNormalShininess"), 1);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_lighting_pass], "gAlbedoSpec"), 2);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_lighting_pass], "t_ambient_occlusion"), 3);
		pop_gl_errors("glUniform1i -> gPosition, gNormalShininess, gAlbedoSpec, t_ambient_occlusion");
	}
	// SSAO
	if(!compile_program_vert_frag(shader_program[Shader::sh_ssao], "../resources/shaders/ssao_pass.vert", "../resources/shaders/ssao_pass.frag"))
		std::cerr << io_red << "sh_ssao COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_ssao]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_ssao], "t_position"), 0);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_ssao], "t_normal"), 1);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_ssao], "t_noise"), 2);
		pop_gl_errors("glUniform1i -> t_position, t_normal, t_noise");
	}
	// HBAO
	if(!compile_program_vert_frag(shader_program[Shader::sh_hbao], "../resources/shaders/hbao_pass.vert", "../resources/shaders/hbao_pass.frag"))
		std::cerr << io_red << "sh_hbao COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_hbao]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_hbao], "t_position"), 0);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_hbao], "t_normal"), 1);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_hbao], "t_noise"), 2);
		pop_gl_errors("glUniform1i -> t_position, t_normal, t_noise");
	}
	// DECAL
	if(!compile_program_vert_frag(shader_program[Shader::sh_decal], "../resources/shaders/decal.vert", "../resources/shaders/decal.frag"))
		std::cerr << io_red << "sh_decal COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_decal]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_decal], "t_diffuse"), 0);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_decal], "t_normal"), 1);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_decal], "t_depth"), 2);
		pop_gl_errors("glUniform1i -> t_diffuse, t_normal, t_depth");
	}
	// PN TRIANGLES
	if(!compile_program_vert_tcl_tes_frag(shader_program[Shader::sh_pn_triangles], "../resources/shaders/vert_tess.vert", "../resources/shaders/pn_triangles.tcs", "../resources/shaders/pn_triangles.tes", "../resources/shaders/phong_tess.frag"))
		std::cerr << io_red << "sh_pn_triangles COMPILE ERROR!!!" << io_white << std::endl;

	// PHONG TESS
	if(!compile_program_vert_tcl_tes_frag(shader_program[Shader::sh_phong_tess], "../resources/shaders/vert_tess.vert", "../resources/shaders/phong_tess.tcs", "../resources/shaders/phong_tess.tes", "../resources/shaders/phong_tess.frag"))
		std::cerr << io_red << "sh_phong_tess COMPILE ERROR!!!" << io_white << std::endl;

	// SOBEL
	if(!compile_program_vert_frag(shader_program[Shader::sh_sobel], "../resources/shaders/raw.vert", "../resources/shaders/sobel.frag"))
		std::cerr << io_red << "sh_sobel COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_sobel]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_sobel], "depth_texture"), 0);
		pop_gl_errors("glUniform1i -> depth_texture");
	}
	// ANTIALIASING
	if(!compile_program_vert_frag(shader_program[Shader::sh_antialiasing], "../resources/shaders/raw.vert", "../resources/shaders/antialiasing.frag"))
		std::cerr << io_red << "sh_antialiasing COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_antialiasing]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_antialiasing], "color_texture"), 0);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_antialiasing], "edges_texture"), 1);
		pop_gl_errors("glUniform1i -> color_texture, edges_texture");
	}
	// HDR
	if(!compile_program_vert_frag(shader_program[Shader::sh_hdr], "../resources/shaders/raw.vert", "../resources/shaders/hdr.frag"))
		std::cerr << io_red << "sh_hdr COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_hdr]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_hdr], "hdr_texture"), 0);
		pop_gl_errors("glUniform1i -> hdr_texture");
	}
	// BLUR
	if(!compile_program_vert_frag(shader_program[Shader::sh_linear_blur], "../resources/shaders/raw.vert", "../resources/shaders/linear_blur.frag"))
		std::cerr << io_red << "sh_linear_blur COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_linear_blur]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_linear_blur], "color_texture"), 0);
		pop_gl_errors("glUniform1i -> color_texture");
	}
	// GAUSSIAN BLUR
	if(!compile_program_vert_frag(shader_program[Shader::sh_gaussian_blur], "../resources/shaders/raw.vert", "../resources/shaders/gaussian_blur.frag"))
		std::cerr << io_red << "sh_gaussian_blur COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_gaussian_blur]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_gaussian_blur], "color_texture"), 0);
		pop_gl_errors("glUniform1i -> color_texture");
	}
	// BILATERAL BLUR
	if(!compile_program_vert_frag(shader_program[Shader::sh_bilateral_blur], "../resources/shaders/raw.vert", "../resources/shaders/bilateral_blur.frag"))
		std::cerr << io_red << "sh_bilateral_blur COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_bilateral_blur]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_bilateral_blur], "color_texture"), 0);
		pop_gl_errors("glUniform1i -> color_texture");
	}
	// BLOOM
	if(!compile_program_vert_frag(shader_program[Shader::sh_bloom], "../resources/shaders/raw.vert", "../resources/shaders/bloom.frag"))
		std::cerr << io_red << "sh_bloom COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_bloom]);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_bloom], "color_texture"), 0);
		glUniform1i(glGetUniformLocation(graphics.shader_program[sh_bloom], "hdr_texture"), 1);
		pop_gl_errors("glUniform1i -> color_texture, hdr_texture");
	}
	// TERRAIN PERLIN
	if(!compile_program_vert_frag(shader_program[Shader::sh_terrain_perlin], "../resources/shaders/terrain_perlin.vert", "../resources/shaders/terrain_perlin.frag"))
		std::cerr << io_red << "sh_terrain_perlin COMPILE ERROR!!!" << io_white << std::endl;

	// TERRAIN MAP
	if(!compile_program_vert_frag(shader_program[Shader::sh_terrain_map], "../resources/shaders/terrain_map.vert", "../resources/shaders/terrain_map.frag"))
		std::cerr << io_red << "sh_terrain_map COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_terrain_map]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_terrain_map], "t_height"), 3);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_terrain_map], "t_noise"), 4);
		pop_gl_errors("glUniform1i -> t_height, t_noise");
	}
	// TERRAIN CUBEMAP
	if(!compile_program_vert_frag(shader_program[Shader::sh_terrain_cubemap], "../resources/shaders/terrain_cubemap.vert", "../resources/shaders/geometry_pass.frag"))
		std::cerr << io_red << "sh_terrain_cubemap COMPILE ERROR!!!" << io_white << std::endl;
	else {
		glUseProgram(shader_program[Shader::sh_terrain_cubemap]);
		glUniform1i(glGetUniformLocation(shader_program[Shader::sh_terrain_cubemap], "t_height_cubemap"), 3);
		pop_gl_errors("glUniform1i -> t_height_cubemap");
	}



}

// DEBUG

	/**
*
* @brief add a segment to debug lines list, ready to be poped and rendered
* @param start
* @param end
*/
void Graphics::debug_line(const vec3& start, const vec3& end)
{
	m_debug_lines.push_back(start);
	m_debug_lines.push_back(end);
}
/**
*
* @brief draw all stacked debug lines and pop them after
* @param m
* @param color
* @param line_width
*/
void Graphics::draw_debug_lines(const mat4 & m, Color color, float line_width)
{
	if (m_debug_lines.empty())
		return;
	// create buffer
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_debug_lines.size(), m_debug_lines.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw
	//glLineWidth(line_width);
	glUseProgram(graphics.shader_program[sh_color]);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &m[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glDrawArrays(GL_LINES, 0, (GLsizei)m_debug_lines.size());

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	m_debug_lines.clear();

	pop_gl_errors(__FUNCTION__);
}



#ifndef UTILS

/**

*
* @brief pop all accumulated openGL errors
* @param msg_format like printf
* @return
*/
unsigned pop_gl_errors(const char* msg_format, ...)
{
	va_list argptr;
	va_start(argptr, msg_format);
	unsigned error_count = 0;
	while (GLuint error = glGetError())
	{
		error_count++;
		//MessageBox(NULL, std::to_string(error).c_str(), "GL error code:", MB_ICONERROR | MB_OK);
		std::cout << "GL error code: " << std::showbase << std::hex << error << ". ";
	}
	if (error_count > 0) {
		vprintf(msg_format, argptr);
		std::cout << std::endl;
	}
	va_end(argptr);
	return error_count;
}
/**
*
* @brief get monitor size in pixels
* @return
*/
ivec2 get_monitor_size()
{
	// get monitor size
	int width, height, xpos, ypos;
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	//glfwGetMonitorContentScale(monitor, &scale_x, &scale_y);
	//glfwGetMonitorPhysicalSize(monitor, &monitor_w, &monitor_h);
	glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);
	return ivec2{ width, height };
}

#endif



#ifndef IMMEDIATE_DRAWING
// draw primitives
/**
*
* @param obb
* @param mvp
* @param color
*/
void draw_obb(const AABB& obb, const mat4& mvp, Color color)
{
	glUseProgram(graphics.shader_program[sh_color]);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &(mvp)[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glBindVertexArray(graphics.primitives[primitive_cube].vao);
	graphics.primitives[primitive_cube].draw();
}
/**
*
* @param ab
* @param vp
* @param color
*/
void draw_aabb(const AABB& ab, const mat4& vp, Color color)
{
	glUseProgram(graphics.shader_program[sh_color]);
	vec3 diff = ab.max_point - ab.min_point;
	diff /= 2.f;
	vec3 center = ab.min_point + diff;
	mat4 model = glm::translate(center) * glm::scale(diff);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &(vp * model)[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glBindVertexArray(graphics.primitives[primitive_cube].vao);
	graphics.primitives[primitive_cube].draw();
}
/**
*
* @param sph
* @param vp
* @param color
*/
void draw_sphere(const Sphere& sph, const mat4& vp, Color color)
{
	glUseProgram(graphics.shader_program[sh_color]);
	mat4 model = glm::translate(sph.center) * glm::scale(vec3{sph.radius});
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &(vp * model)[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glBindVertexArray(graphics.primitives[primitive_sphere].vao);
	graphics.primitives[primitive_sphere].draw();
}
/**
*
* @param tri
* @param vp
* @param color
*/
void draw_triangle(const Triangle& tri, const mat4& m, Color color)
{
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), &tri.p0, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw
	//glLineWidth(line_width);
	glUseProgram(graphics.shader_program[sh_color]);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &m[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	pop_gl_errors(__FUNCTION__);

}
#endif