/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: camera.h
Purpose: define camera's properties and easy matrix acces
Language: c++
Platform: windows 10
Project: cs300_markel.p_0
Author: Markel Pisano Berrojalbiz
Creation date: 5/16/2019
----------------------------------------------------------------------------------------------------------*/
#ifndef CAMERA_H
#define CAMERA_H

struct Camera
{
	vec3 pos = {};
	vec3 target = {0, 0, -1};			// where to look at
	vec2 size = {WINDOW_X, WINDOW_Y};	// screen width and height
	float near_plane = 0.1f, far_plane = 5000.f;
	float fov = glm::radians(59.f);

	inline mat4 get_view(vec3 up = vec3{0, 1, 0}) const { return glm::lookAt(pos, target, up); }
	inline mat4 get_proj() const { return glm::perspective(fov, size.x / size.y, near_plane, far_plane); }
	inline mat4 ortho_proj() const { vec2 hs = size / 2.f; return glm::orthoRH(-hs.x, hs.x, -hs.y, hs.y, near_plane, far_plane); }
	inline vec3 ndc_to_world(vec3 ndc_coords) const {
		vec4 world_pos = glm::inverse(get_proj() * get_view()) * vec4{ ndc_coords, 1 };
		world_pos /= world_pos.w;
		return world_pos;
	}
};

#endif // CAMERA_H
