/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: time_system.cpp
Purpose: count application's time
Language: c++
Platform: windows 10
Project: cs300_markel.p_0
Author: Markel Pisano Berrojalbiz
Creation date: 5/16/2019
----------------------------------------------------------------------------------------------------------*/

#include <pch.h>

FrameRateController frc;		// Frame Rate Controller

	/**
*
* @brief reset Frame Rate Controller system
*/
void FrameRateController::reset()
{
	m_delta_time = 0.f;
	m_frame_time = m_frame_start_time = m_time_count = 0.0;
	m_frame_rate = fps;
	m_frame_count = 0;
}
/**
*
* @brief start frame timer
*/
void FrameRateController::start_frame()
{
	m_frame_start_time = get_time();
}
/**
*
* @brief end frame timer (can block app until selected frame time)
*/
void FrameRateController::end_frame()
{
	const double controlled_frame_time = 1.0 / fps;
	m_frame_time = get_time() - m_frame_start_time;
	if (is_controlled_frame_rate)
	{
		// lock the application until frame reaches desired time
		while (frame_time < controlled_frame_time)
			m_frame_time = get_time() - m_frame_start_time;
	}
	m_frame_count++;
	m_frame_rate = 1.0 / frame_time;
	m_time_count += frame_time;
	m_delta_time = (float)frame_time * time_scale;
}
/**
*
* @brief get current time in seconds
* @return
*/
double get_time()
{
	return glfwGetTime();
}
