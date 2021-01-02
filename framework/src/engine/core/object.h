/**
* @file object.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/22	(eus)
* @brief Declare Object class
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#ifndef OBJECT_H
#define OBJECT_H

class Object
{
private:
	// ID
	std::string name;
	bool enabled = true;

public:
	void set_enabled(bool val) { enabled = val; }
	bool get_enabled() const { return enabled; }

	void set_name(const std::string& _name) { name = _name; }
	const std::string& get_name() const { return name; }

	RTTI_DCL
};

#endif	// OBJECT_H