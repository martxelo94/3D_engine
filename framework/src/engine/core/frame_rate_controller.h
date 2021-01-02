/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: time.h
Purpose: define frame control utils
Language: c++
Platform: windows 10
Project: cs300_markel.p_0
Author: Markel Pisano Berrojalbiz
Creation date: 5/16/2019
----------------------------------------------------------------------------------------------------------*/
#ifndef TIME_H
#define TIME_H

#define DEFAULT_FPS	60

//get global time in seconds
double get_time();

struct FrameRateController
{
	//public set
	u32 fps = DEFAULT_FPS;					// desired frames per second
	float time_scale = 1.f;					// time scale of the app
	bool is_controlled_frame_rate = true;	// cap frame rate

private:
	float m_delta_time = 0.f;			// frame_time * time_scale
	double m_frame_rate = 0.0;			// application's frame rate
	double m_frame_time = 0.0;			// times took the last frame
	double m_frame_start_time = 0.0;	// time at the start of the frame
	double m_time_count = 0.0;			// time since launch
	u64 m_frame_count = 0;				// frames since launch

public:
	void reset();			// reset every time data (except settable variables)
	void start_frame();
	void end_frame();

	// gettors 
	const float & delta_time = m_delta_time;
	const double & frame_rate = m_frame_rate;
	const double & frame_time = m_frame_time;
	const double & time_count = m_time_count;
	const u64 & frame_count = m_frame_count;	// (is this good practice?)

};

// Frame Rate Controller
extern FrameRateController frc;		// TODO: make singleton
// shortcut for lazy programmers
#define dt frc.delta_time

// F floating point type
// TIME_OUT does reset at end?
template<typename F, bool TIME_OUT>
struct TimeCounter {
	F current = F();	// time since reset
	F end = F();		// time delta sice start, end here

	inline F get_end() const { return end; }
	inline void set(F _end) { end = _end; }
	inline void reset() { current = 0.f; }
	bool operator()() {
		current += (F)dt;
		if (current >= end) {
			 if constexpr (TIME_OUT == false)
				reset();
			return true;
		}
		return false;
	}
};
using TimeInterval = TimeCounter<float, false>;
using TimeOut = TimeCounter<float, true>;

// simple MACRO timer used with instanced float

// TIMER_START: Set start time
#define TIMER_S(counter) counter = get_time()
// TIMER_END: Get time since start
#define TIMER_E(counter) counter = get_time() - counter
// TIMER_RESET: Set time to 0
#define TIMER_R(counter) counter = 0.f

#endif	// TIME_H
