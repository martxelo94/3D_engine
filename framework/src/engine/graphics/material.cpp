/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: material.cpp
Purpose: implement Material type
Language: c++
Platform: windows 10
Project: cs300_markel.p_1
Author: Markel Pisano Berrojalbiz
Creation date: 6/1/2019
----------------------------------------------------------------------------------------------------------*/

#include <pch.h>

void Material::edit(const char * node_name)
{
	if (ImGui::TreeNode(node_name)) {
		vec3 tmp(float(diffuse.r) / 255, float(diffuse.g) / 255, float(diffuse.b) / 255);
		ImGui::ColorEdit3("Diffuse", &tmp[0]);
		diffuse = Color{ vec4{ tmp, 1.f } };
		tmp = vec3(float(specular.r) / 255, float(specular.g) / 255, float(specular.b) / 255);
		ImGui::ColorEdit3("Specular", &tmp[0]);
		specular = Color{ vec4{ tmp, 1.f } };

		ImGui::DragFloat("Shininess", &shininess, 0.1f, 0.0, 1000.f);
		ImGui::DragFloat("Shine Intensity", &shine_intensity, 0.1f, 0.0, 1000.f);
		ImGui::TreePop();
	}

}

void Material::set_uniforms(Shader sh_program) const {
	assert(t_diffuse);
	int uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.t_diffuse");
	glUniform1i(uniform_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *t_diffuse);

	if (t_normals) {
		uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.t_normals");
		glUniform1i(uniform_loc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *t_normals);
}
	if (t_specular) {
		uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.t_specular");
		glUniform1i(uniform_loc, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, *t_specular);
	}


	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.diffuse");
	diffuse.set_uniform_RGB(uniform_loc);

	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.specular");
	specular.set_uniform_RGB(uniform_loc);

	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.shininess");
	glUniform1f(uniform_loc, shininess);

	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.shine_intensity");
	glUniform1f(uniform_loc, shine_intensity);
	}


#ifdef AI_TYPES_H_INC
std::string get_texture_name(const aiString &path)
{
	std::string tmp(path.C_Str());
	size_t slash_pos = tmp.find_last_of('/');
	tmp = tmp.substr(slash_pos + 1);
	return tmp;
}
#endif
#ifdef AI_MATERIAL_H_INC
void set_uniforms(Shader sh_program, const aiMaterial *mat)
{
	int uniform_loc;
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.t_diffuse");
		glUniform1i(uniform_loc, 0);
		glActiveTexture(GL_TEXTURE0);
		aiString path;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		const GLuint texture = graphics.texture_map[get_texture_name(path)];
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {

		uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.t_normals");
		glUniform1i(uniform_loc, 1);
		glActiveTexture(GL_TEXTURE1);
		aiString path;
		mat->GetTexture(aiTextureType_HEIGHT, 0, &path);
		const GLuint normal_map = graphics.texture_map[get_texture_name(path)];
		glBindTexture(GL_TEXTURE_2D, normal_map);

	}
	if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.t_specular");
		glUniform1i(uniform_loc, 1);
		glActiveTexture(GL_TEXTURE2);
		aiString path;
		mat->GetTexture(aiTextureType_SPECULAR, 0, &path);
		const GLuint normal_map = graphics.texture_map[get_texture_name(path)];
		glBindTexture(GL_TEXTURE_2D, normal_map);
	}

	Color color;
	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.diffuse");
	aiColor4D c;
	if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &c)) {
		color = Color{ vec4{ c.r, c.g, c.b, c.a } };
	}
	color.set_uniform_RGB(uniform_loc);

	// AMBIENT
	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.ambient");
	if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &c)) {
		color = Color{ vec4{ c.r, c.g, c.b, c.a } };
	}
	else
		color = Color{};
	color.set_uniform_RGB(uniform_loc);
	// SPECULAR
	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.specular");
	if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &c)) {
		color = Color{ vec4{ c.r, c.g, c.b, c.a } };
	}
	else
		color = Color{};
	color.set_uniform_RGB(uniform_loc);
	// EMISSIVE
	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.emissive");
	if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &c)) {
		color = Color{ vec4{ c.r, c.g, c.b, c.a } };
	}
	else
		color = Color{};
	color.set_uniform_RGB(uniform_loc);

	uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.shininess");
	float shininess;
	u32 max;
	aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max);
	glUniform1f(uniform_loc, shininess);

	//uniform_loc = glGetUniformLocation(graphics.shader_program[sh_program], "material.shine_intensity");
	//glUniform1f(uniform_loc, shine_intensity);
}
#endif