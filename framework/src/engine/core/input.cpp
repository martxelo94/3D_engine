/**
* @file input.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/01	(eus)
* @brief Define bounding volume creators
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include <pch.h>

JoystickDevice joystick;
KeyboardDevice keyboard;
MouseDevice mouse;

/**
*
* @brief reset mouse info
*/
void MouseDevice::init()
{
	InputDevice::init();
	pos = prevPos = move = scrollOffset = prevScrollOffset = scroll = ivec2{};
	overWindow = false;
}
/**
*
* @brief update input buttons and mouse info
*/
void MouseDevice::update()
{
	InputDevice::update();
	// update mouse motion
	move = pos - prevPos;
	prevPos = pos;
	// update mouse scroll
	scroll = scrollOffset - prevScrollOffset;
	prevScrollOffset = scrollOffset;
}
/**
*
* @brief create a ray in NDC space from near to far, in the direction of the cursor
* @return
*/
Ray MouseDevice::ray_cast() const
{
// return ray in NDC space
	vec3 mouse_pos{pos, 1.f};
	mouse_pos = graphics.window.window_to_ndc() * mouse_pos;
	mouse_pos.z = -1.f;
	return Ray{ mouse_pos, vec3{0, 0, 2} };
}


// callback functions

	/**
*
* @param window
* @param key
* @param scancode
* @param action
* @param mods
*/
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// not used yet...
	(void)scancode;
	(void)mods;

	assert(key >= KEYS_LOW_BOUND);
	assert(key <= KEYS_HIGH_BOUND);

	// close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	
	if (action == GLFW_PRESS) {
		keyboard.set(key, KeyState::triggered);
	}
	else if (action == GLFW_RELEASE) {
		keyboard.set(key, KeyState::up);
	}

	// IMGUI
	//ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}
/**
*
* @param window
* @param button
* @param action
* @param mods
*/
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	// not used yet...
	(void)mods;

	assert(button >= GLFW_MOUSE_BUTTON_1);
	assert(button <= GLFW_MOUSE_BUTTON_LAST);

	if (action == GLFW_PRESS) {
		mouse.set(button, KeyState::triggered);
	}
	else if (action == GLFW_RELEASE) {
		mouse.set(button, KeyState::up);
	}

	// IMGUI
	//ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}
/**
*
* @param window
* @param xpos
* @param ypos
*/
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
// handle mouse position; top-left coordinate relative
	mouse.pos = { xpos, ypos };
	
	// handle ImGui mouse pos
	//ImGuiIO& io = ImGui::GetIO();
	//io.MousePos.x = mouse.pos.x;
	//io.MousePos.y = mouse.pos.y;
}
/**
*
* @param window
* @param entered 
*/
void mouse_over_callback(GLFWwindow* window, int entered)
{
// handle mouse enter/leave window area
	if (entered)
		mouse.overWindow = true;
	else
		mouse.overWindow = false;
}
/**
*
* @param window
* @param xoffset
* @param yoffset
*/
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
// handle mouse scroll wheel
	mouse.scrollOffset = {xoffset, yoffset};

	// IMGUI
	//ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

/**
*
* @param window
* @param xoffset
* @param yoffset
*/
void window_move_callback(GLFWwindow* window, int xoffset, int yoffset)
{
	graphics.window.position = {xoffset, yoffset};
}


std::string get_directory(const std::string &full_path)
{
	size_t dir_offset = full_path.find_last_of("\\");	// ONLY WINDOWS!!! CREATE MACRO TO HANDLE OTHER OS
	std::string directory = full_path.substr(0, dir_offset + 1);
	return directory;
}

std::string get_extension(const std::string &full_path)
{
	size_t ext_offset = full_path.find_last_of(".");
	std::string extension= full_path.substr(ext_offset);
	return extension;
}

std::string get_filename(const std::string &full_path)
{
	size_t dir_offset = full_path.find_last_of("\\");	// ONLY WINDOWS!!! CREATE MACRO TO HANDLE OTHER OS
	size_t ext_offset = full_path.find_last_of(".");
	std::string filename = full_path.substr(dir_offset + 1, ext_offset - dir_offset - 1);
	return filename;
}
std::string get_filename_extension(const std::string &full_path)
{
	size_t dir_offset = full_path.find_last_of("\\");	// ONLY WINDOWS!!! CREATE MACRO TO HANDLE OTHER OS
	std::string filename_ext = full_path.substr(dir_offset + 1);
	return filename_ext;
}
// SYSTEM TOOLS
std::vector<std::string> get_filenames(const char* directory_path)
{
	std::vector<std::string> result;
	WIN32_FIND_DATA file;
	HANDLE handle = FindFirstFile(directory_path, &file);
	if (handle) {
		do {
			//std::wcout << file.cFileName << std::endl;
			result.push_back(file.cFileName);
		} while (FindNextFile(handle, &file));
		FindClose(handle);
	}
	return result;
}

bool search_files(std::vector<std::string> *filenames, std::string *directory)
{
	char buffer[256];
	// setup file dialog box
	OPENFILENAME opf;
	opf.hwndOwner = 0;
	opf.lpstrFilter = NULL;
	opf.lpstrCustomFilter = 0;
	opf.nMaxCustFilter = 0L;
	opf.nFilterIndex = 1L;
	opf.lpstrFile = buffer;
	opf.lpstrFile[0] = '\0';
	opf.nMaxFile = 255;
	opf.lpstrFileTitle = 0;
	opf.nMaxFileTitle = 50;
	opf.lpstrInitialDir = NULL;
	opf.lpstrTitle = "Search files";
	opf.nFileOffset = 0;
	//opf.nFilemExtension    = 2;
	opf.lpstrDefExt = NULL;
	opf.lpfnHook = NULL;
	opf.lCustData = 0;
	opf.Flags = (DWORD)OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR;
	opf.lStructSize = sizeof(OPENFILENAMEW);

	if (GetOpenFileName(&opf))
	{
		LPSTR fname = opf.lpstrFile;	//absolut path
		if (directory)
			*directory = get_directory(fname);

		fname += strlen(fname) + 1;	// get first filename
		do
		{
			std::cout << fname << std::endl;
			if (filenames)
				filenames->push_back(fname);
			fname += strlen(fname) + 1;	// next filename
		} while (*fname != 0);

		return true;
	}
	else
		std::cerr << "Searching files FAILED!" << std::endl;
	
	return false;
}

std::ostream& io_white(std::ostream &s) {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);	// RGB flags
	return s;
}
std::ostream& io_red(std::ostream &s) {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED);	// R flags
	return s;
}
std::ostream& io_yellow(std::ostream &s) {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN);	// RG flags
	return s;
}
std::ostream& io_green(std::ostream &s) {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN);	// G flags
	return s;
}
