/**
* @file scene.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Implement Scene class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#include <pch.h>

Scene scene;

void Scene::save() {

}
void Scene::save_as() {

}
void Scene::load() {

}

void Scene::init() {

}
void Scene::free() {
	gameObjectPool.free();
}
void Scene::imgui() {
	auto fun = [](const GameObject& go) {
		std::cout << go.get_name() << " ";
	};
	//ImGui::Begin("Hierarchy")
	gameObjectPool.for_each(fun);
	//ImGui::End();
}
void Scene::update() {

}
void Scene::render() {

}