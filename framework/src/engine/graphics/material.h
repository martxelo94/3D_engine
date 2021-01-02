/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: material.h
Purpose: implement Material type
Language: c++
Platform: windows 10
Project: cs300_markel.p_1
Author: Markel Pisano Berrojalbiz
Creation date: 6/1/2019
----------------------------------------------------------------------------------------------------------*/

#ifndef MATERIAL_H
#define MATERIAL_H

/*
	ON SHADER:

	struct Material {
		sampler2D texture;
		vec3 diffuse;
		vec3 specular;
		float shininess;
		float shine_intensity;
	};
	uniform Material material;
*/

struct Material
{
	GLuint * t_diffuse = &graphics.tex[Texture::t_white];	// pointer to texture
	GLuint * t_normals = nullptr;
	GLuint * t_specular = nullptr;
	Color diffuse, specular;
	float shininess = 32.f;
	float shine_intensity = 1.f;

	void edit(const char * node_name);
	void set_uniforms(Shader sh_program) const;
};

#ifdef AI_MATERIAL_H_INC
void set_uniforms(Shader sh_program, const aiMaterial *mat);
#endif

#endif