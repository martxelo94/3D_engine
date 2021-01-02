/**
* @file component.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare Component class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/
#ifndef COMPONENT_H
#define COMPONENT_H

class GameObject;

class Component : public Object
{
protected:
	GameObject* gameObject;

public:
	void init() {}
	void free() {}

	RTTI_DCL
};

#endif