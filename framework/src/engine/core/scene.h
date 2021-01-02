/**
* @file scene.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare Scene class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#ifndef SCENE_H
#define SCENE_H

class Scene
{
	Pool<GameObject, 1000> gameObjectPool;
public:
	void save();
	void save_as();
	void load();

	void init();
	void free();

	void imgui();
	void update();
	void render();
};

extern Scene scene;	// SINGLETON

#endif