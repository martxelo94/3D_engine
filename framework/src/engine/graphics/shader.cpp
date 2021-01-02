/**
* @file shader.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/30	(eus)
* @brief Implement shader
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>

// Utils
namespace
{
	/**
*
* @param program_handle
*/
	void check_gl_program_link_errors(GLuint program_handle, const std::vector<std::string> &filenames)
	{
		GLint status = GL_FALSE;
		glGetProgramiv(program_handle, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			int infoLogLength = 0, maxLength = 0;
			glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &maxLength);
			std::string infoLog(maxLength, 0);
			glGetProgramInfoLog(program_handle, maxLength, &infoLogLength, &infoLog[0]);
			//pop error window
			std::cerr << "ShaderProgram LINK ERROR" << std::endl;
			for (int i = 0; i < filenames.size(); ++i)
				std::cout << "\t" << filenames[i] << std::endl;
			std::cout << "Program link error: " << infoLog << std::endl;
			//MessageBox(NULL, infoLog.c_str(), "Program compile error", MB_ICONERROR | MB_OK);
		}
	}

	/**
*
* @param shader_type
* @param source
* @return
*/
	GLuint compile_shader(GLuint shader_type, const std::string &source, const char *filename)
	{
		GLuint shader_handle = glCreateShader(shader_type);
		const char* c_source = source.c_str();
		glShaderSource(shader_handle, 1, &c_source, NULL);
		glCompileShader(shader_handle);

		// Check errors
		GLint status = GL_FALSE;
		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			int infoLogLength = 0, maxLength = 0;
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &maxLength);
			std::string infoLog(maxLength, 0);
			glGetShaderInfoLog(shader_handle, maxLength, &infoLogLength, &infoLog[0]);
			std::cerr << "Shader compile error: " << filename << std::endl << infoLog << std::endl;
			std::cerr << "\"" << source << "\"" << std::endl;

			//pop error window
			//MessageBox(NULL, infoLog.c_str(), "Shader compile error", MB_ICONERROR | MB_OK);
			return 0;
		}
		return shader_handle;
	}
	/**
*
* @param filename
* @return
*/
	std::string get_extension(const char* filename)
	{
		std::string f(filename);
		size_t dot_position = f.find_last_of('.');
		return f.substr(dot_position);
	}
	/**
*
* @brief return file contents in string format
* @param filename
* @return
*/
	std::string read_to_string(const char* filename)
	{
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			file.open(filename);
			std::string res((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			return res;
		}
		catch (std::ifstream::failure e) {
			std::cerr << "Exception opening/reading/closing file \"" << filename << "\"\n";
			return "";
		}
	}

	/**
*
* @param filepath
* @return
*/
	GLuint compile_shader(const char* filepath)
	{
		// knows the shader type by the file extension
		std::string extension = get_extension(filepath);
		GLuint shader_type = (GLuint)-1;
		if (extension == ".vert")
			shader_type = GL_VERTEX_SHADER;
		else if (extension == ".frag")
			shader_type = GL_FRAGMENT_SHADER;
		else if (extension == ".geo")
			shader_type = GL_GEOMETRY_SHADER;
		else if (extension == ".tcs")
			shader_type = GL_TESS_CONTROL_SHADER;
		else if (extension == ".tes")
			shader_type = GL_TESS_EVALUATION_SHADER;
		else {
			std::cerr << "\"" << filepath << "\" has not a valid shader extension.\n";
			return 0;	//no shader handle
		}
		//open file
		std::string shader_source = read_to_string(filepath);
		if (shader_source.empty()) {
			std::cerr << "ERROR: \"" << filepath << "\" source is empty.\n";
			return 0;
		}
		return compile_shader(shader_type, shader_source, filepath);
	}
}

bool compile_program_vert_frag(GLuint &shader_handler, const char* vert_file, const char* frag_file)
{
	int errors = 0;
	GLuint vertex_shader = compile_shader(vert_file);
	GLuint fragment_shader = compile_shader(frag_file);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed compiling shaders");

	//compile sh_line shader program
	shader_handler = glCreateProgram();
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed creating shader program");
	glAttachShader(shader_handler, vertex_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching vertex shader");
	glAttachShader(shader_handler, fragment_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching fragment shader");
	glLinkProgram(shader_handler);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed linking shader program");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	check_gl_program_link_errors(shader_handler, {vert_file, frag_file});
	errors += pop_gl_errors("ERROR at gl error checkin WTF!");
	return errors == 0;
}
bool compile_program_vert_geo_frag(GLuint &shader_handler, const char* vert_file, const char* geo_file, const char* frag_file)
{
	int errors = 0;
	GLuint vertex_shader = compile_shader(vert_file);
	GLuint geometry_shader = compile_shader(geo_file);
	GLuint fragment_shader = compile_shader(frag_file);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed compiling shaders");

	//compile sh_line shader program
	shader_handler = glCreateProgram();
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed creating shader program");
	glAttachShader(shader_handler, vertex_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching vertex shader");
	glAttachShader(shader_handler, geometry_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching geometry shader");
	glAttachShader(shader_handler, fragment_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching fragment shader");
	glLinkProgram(shader_handler);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed linking shader program");

	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);

	check_gl_program_link_errors(shader_handler, {vert_file, geo_file, frag_file});

	return errors == 0;
}
bool compile_program_vert_tcl_tes_frag(GLuint &shader_handler, const char* vert_file, const char* tcl_file, const char* tes_file, const char* frag_file)
{
	int errors = 0;
	GLuint vertex_shader = compile_shader(vert_file);
	GLuint tcl_shader = compile_shader(tcl_file);
	GLuint tes_shader = compile_shader(tes_file);
	GLuint fragment_shader = compile_shader(frag_file);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed compiling shaders");

	//compile sh_line shader program
	shader_handler = glCreateProgram();
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed creating shader program");
	glAttachShader(shader_handler, vertex_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching vertex shader");
	glAttachShader(shader_handler, tcl_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching tess-control shader");
	glAttachShader(shader_handler, tes_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching tess-evaluation shader");
	glAttachShader(shader_handler, fragment_shader);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed attaching fragment shader");
	glLinkProgram(shader_handler);
	errors += pop_gl_errors(__FUNCTION__" ERROR: Failed linking shader program");

	glDeleteShader(vertex_shader);
	glDeleteShader(tcl_shader);
	glDeleteShader(tes_shader);
	glDeleteShader(fragment_shader);

	check_gl_program_link_errors(shader_handler, {vert_file, tcl_file, tes_file, frag_file});

	return errors == 0;
}
