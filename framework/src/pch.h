/**
* @file pch.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Include headers
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef PCH_H
#define PCH_H

// Windows
#include <Windows.h>

// std
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <bitset>
#include <stack>
#include <memory>	// smart pointers
#include <numeric>	// iota

// glm
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/random.hpp>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// stb
//#include <stb/stb_image.h>


// ImGui
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define NOT_NAMED_UNIONS 0

// core
#include "engine/core/types.h"
#include "engine/core/pool.h"
#include "engine/core/shapes.h"
#include "engine/core/input.h"
#include "engine/core/frame_rate_controller.h"
#include "engine/core/rtti.h"
#include "engine/core/object.h"
#include "engine/core/component.h"
#include "engine/core/game_object.h"
#include "engine/core/scene.h"
// graphics
#include "engine/graphics/color.h"
#include "engine/graphics/window.h"
#include "engine/graphics/shader.h"
#include "engine/graphics/texture.h"
#include "engine/graphics/buffer.h"
#include "engine/graphics/mesh.h"
#include "engine/graphics/camera.h"
#include "engine/graphics/graphics.h"
#include "engine/graphics/light.h"
#include "engine/graphics/material.h"
#include "engine/graphics/noise.h"
#include "engine/graphics/lod_terrain.h"
#include "engine/graphics/renderer.h"
// physics
#include "engine/physics/geometry.h"
#include "engine/physics/bounding_volume.h"
#include "engine/physics/bounding_volume_hierarchy.h"
#include "engine/physics/octree.h"
#include "engine/physics/kdtree.h"
#include "engine/physics/gjk_simplex.h"
#include "engine/physics/gjk3d.h"
#include "engine/physics/collider.h"
#include "engine/physics/rigidbody.h"

// demos
#include "demo/demo.h"
#include "demo/demo_bvh.h"
#include "demo/demo_gjk.h"
#include "demo/demo_kdtree.h"
#include "demo/demo_octree.h"
#include "demo/demo_lod_terrain.h"
#include "demo/demo_ambient_occlusion.h"
#include "demo/demo_decals.h"
#include "demo/demo_tesselation.h"
#include "demo/demo_deferred_shading.h"


#endif
