/**
* @file window.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/08/31	(eus)
* @brief Implement 
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>

/**
*
* @brief transform from window pixel coordinates to NDC space
* @return
*/
mat3 Window::window_to_ndc() const
{
	// NDC coordinates x{-1,1},y{-1,1}
	int w = size.x, h = size.y;
	//glfwGetWindowSize(window, &w, &h);
	// remember that glm matrices are column mayor, transpose what you know!
	return mat3(
		2.f / w, 0, 0,
		0, -2.f / h, 0,
		-1, 1, 1);
}

/**
*
* @brief transform from NDC space to window coordinates
* @return
*/
mat3 Window::ndc_to_window() const
{
	int w, h;
	glfwGetWindowSize(glfwWindow, &w, &h);

	return mat3(
		(float)w / 2.f, 0, 0,
		0, (float)h / 2.f, 0,
		(float)w / 2.f, (float)h / 2.f, 1
	);
}

/**
*
* @brief toggle fullscreen/windowed
* @return
*/
bool Window::toggle_fullscreen()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (is_fullscreen) {
		// switch to windowed
		//ivec2 screen_centered_pos = {monitor.x / 2 - window.size.x / 2, monitor.y / 2 - window.size.y / 2};
		glfwSetWindowMonitor(glfwWindow, NULL, position.x, position.y, size.x, size.y, mode->refreshRate);
		//glfwMaximizeWindow(window);
		glViewport(0, 0, size.x, size.y);
	}
	else
	{
		// switch to fullscreen
		glfwSetWindowMonitor(glfwWindow, monitor, position.x, position.y, size.x, size.y, mode->refreshRate);
		glViewport(0, 0, size.x, size.y);
	}

	return is_fullscreen = !is_fullscreen;
}
/**
*
* @param size_x
* @param size_y
* @return
*/
void Window::resize(int size_x, int size_y)
{
	size = { size_x, size_y };
	glfwSetWindowSize(glfwWindow, size_x, size_y);
	glViewport(0, 0, size_x, size_y);
}
