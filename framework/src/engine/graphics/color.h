/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 Markel Pisano's Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of Markel Pisano Berrojalbiz is prohibited.
File Name: color.h
Purpose: define color's properties and easy shader binding
Language: c++
Platform: windows 10
Project: cs300_markel.p_0
Author: Markel Pisano Berrojalbiz
Creation date: 5/16/2019
----------------------------------------------------------------------------------------------------------*/

#ifndef COLOR_H
#define COLOR_H

#define DEFAULT_COLOR 0xffffffff	// white

struct Color {
#if NOT_NAMED_UNIONS
	union {
		u32 c;
		struct {
			u8 r, g, b, a;
		};
		u8 v[4];
	};
	Color()
		: c(0xFFFFFFFF)	// white default
	{}
#else
	Color() = default;
	u8 r = 255, g = 255, b = 255, a = 255;
#endif

	Color(u8 r, u8 g, u8 b, u8 a = 255)
		: r(r), g(g), b(b), a(a)
	{}
	Color(u32 color)
		: Color(u8((color & 0xFF000000) >> 24), u8((color & 0x00FF0000) >> 16), u8((color & 0x0000FF00) >> 8), u8(color & 0x000000FF))
	{}
	Color(const vec4 &color)
		: r((u8)(color.r * 255)), g((u8)(color.g * 255)), b((u8)(color.b * 255)), a((u8)(color.a * 255))
	{}
	inline explicit operator vec4() const {
		return vec4{ r, g, b, a } / 255.f;
	}
	//uniform set (convert from [0, 255] to [0, 1])
	inline void set_uniform_RGBA(int uniform_location) const {
		glUniform4f(uniform_location, (float)r / 255, (float)g / 255, (float)b / 255, (float)a / 255);
	}
	inline void set_uniform_RGB(int uniform_location) const {
		glUniform3f(uniform_location, (float)r / 255, (float)g / 255, (float)b / 255);
	}
	// dont multiply alpha channel
	Color operator*(const float& rhs) const { return Color{ (u8)(r * rhs), (u8)(g * rhs), (u8)(b * rhs), a }; }
	Color& operator*=(const float& rhs) { return *this = Color{ (u8)(r * rhs), (u8)(g * rhs), (u8)(b * rhs), a }; }
	// dont divide alpha channel
	Color operator/(const float& rhs) const { return Color{ (u8)(r / rhs), (u8)(g / rhs), (u8)(b / rhs), a }; }
	Color& operator/=(const float& rhs) { return *this = Color{ (u8)(r / rhs), (u8)(g / rhs), (u8)(b / rhs), a }; }

	explicit operator u32& () { 
		return *reinterpret_cast<u32*>(&r);
	}
	explicit operator const u32& () const {
		return *reinterpret_cast<const u32*>(&r);
	}
	explicit operator u32() const {
		return u32(u32(r) << 24 | u32(g) << 16 | u32(b) << 8 | u32(a));
	}
};

#endif