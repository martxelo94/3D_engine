/**
* @file demo.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define demo
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef DEMO_H
#define DEMO_H


struct Object;

struct Demo 
{
	virtual void update() = 0;
	virtual void render() const = 0;
};

#endif