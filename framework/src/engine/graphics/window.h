/**
* @file window.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/31	(eus)
* @brief Define 
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#ifndef WINDOW_HEADER
#define WINDOW_HEADER

#define WINDOW_X 800
#define WINDOW_Y int(WINDOW_X * (9.f / 16.f))

struct Window
{
	GLFWwindow* glfwWindow = nullptr;
	ivec2 size = vec2{ WINDOW_X, WINDOW_Y };
	ivec2 position;
	bool is_fullscreen = false;

	mat3 window_to_ndc() const;
	mat3 ndc_to_window() const;
	bool toggle_fullscreen();
	void resize(int size_x, int size_y);
};

#endif