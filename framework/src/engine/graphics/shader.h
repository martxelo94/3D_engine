/**
* @file shader.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/30	(eus)
* @brief Declare shader
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef SHADER_H
#define SHADER_H

// used in Graphics as shader_program array indices
enum Shader : int {
	no_shader = -1,
	/*3D, 2D basic*/	sh_raw = 0, sh_color,
	/*DEBUG*/			sh_depth_color, sh_red_color, sh_alpha_color,
	/*Forward shading*/	sh_forward_light, sh_skybox,
	/*Deferred shading*/sh_geometry_pass, sh_lighting_pass, sh_ssao, sh_hbao, sh_decal, sh_pn_triangles, sh_phong_tess,
	/*Post-procesing*/	sh_sobel, sh_antialiasing, sh_hdr, sh_linear_blur, sh_gaussian_blur, sh_bilateral_blur, sh_bloom,
	/*others(Deferred)*/sh_terrain_perlin, sh_terrain_map, sh_terrain_cubemap,

	shader_count
};

bool compile_program_vert_frag(GLuint &shader_handler, const char* vert_file, const char* frag_file);
bool compile_program_vert_geo_frag(GLuint &shader_handler, const char* vert_file, const char* geo_file, const char* frag_file);
bool compile_program_vert_tcl_tes_frag(GLuint &shader_handler, const char* vert_file, const char* tcl_file, const char* tes_file, const char* frag_file);

#endif