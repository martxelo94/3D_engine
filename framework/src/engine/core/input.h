/**
* @file input.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define input
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef INPUT_H
#define INPUT_H

// MOST USED MACROS

// MOUSE:
#define MOUSE_LEFT		GLFW_MOUSE_BUTTON_LEFT
#define MOUSE_RIGHT		GLFW_MOUSE_BUTTON_RIGHT
#define MOUSE_MIDDLE	GLFW_MOUSE_BUTTON_MIDDLE

#define KEYS_LOW_BOUND	GLFW_KEY_SPACE
#define KEYS_HIGH_BOUND	GLFW_KEY_LAST
#define KEY(ascii_code) ascii_code - KEYS_LOW_BOUND

//#define KEY(X) GLFW_KEY_##X

enum class KeyState : bool { up = false, triggered = true };


template<unsigned LOW_BOUND, unsigned HIGH_BOUND>
struct InputDevice
{
protected:
	std::bitset<HIGH_BOUND - LOW_BOUND> devicePrev;
	std::bitset<HIGH_BOUND - LOW_BOUND> deviceCurr;
	/**
*
* @brief set state at virtual keycode
* @param vr_keycode
* @param val
*/
	inline void set(unsigned vr_keycode, KeyState val)
	{
		deviceCurr.set(vr_keycode - LOW_BOUND, (bool)val);
	}
public:
	/**
*
* @brief reset prev and current key states
*/
	void init()
	{
		devicePrev.reset();
		deviceCurr.reset();
	}
	void free() {}
	/**
*
* @brief update prev to current key states
*/
	void update()
	{
		devicePrev = deviceCurr;
		assert(devicePrev == deviceCurr);
	}
	/**
*
* @param keycode
* @return
*/
	bool triggered(unsigned keycode) const
	{
		assert(keycode >= LOW_BOUND);
		assert(keycode <= HIGH_BOUND);
		keycode -= LOW_BOUND;
		if (deviceCurr[keycode] == true && devicePrev[keycode] == false)
			return true;
		return false;
	}
	/**
*
* @param keycode
* @return
*/
	bool pressed(unsigned keycode) const
	{
		assert(keycode >= LOW_BOUND);
		assert(keycode <= HIGH_BOUND);
		keycode -= LOW_BOUND;
		if (deviceCurr[keycode] == true && devicePrev[keycode] == true)
			return true;
		return false;
	}
	/**
*
* @param keycode
* @return
*/
	bool released(unsigned keycode) const
	{
		assert(keycode >= LOW_BOUND);
		assert(keycode <= HIGH_BOUND);
		keycode -= LOW_BOUND;
		if (deviceCurr[keycode] == false && devicePrev[keycode] == true)
			return true;
		return false;
	}

	friend void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

struct MouseDevice : public InputDevice<GLFW_MOUSE_BUTTON_1, GLFW_MOUSE_BUTTON_LAST>
{
private:
	vec2 scrollOffset;		// scroll current offset
	vec2 prevScrollOffset;	// scroll previous offset
	vec2 scroll;		// Specifies scroll motion respect to previous frame
	vec2 pos;		// Specifies the mouse position in top-left window coordinates.
	vec2 prevPos;	// Mouse position the previous frame. Used to calculate "move".
	vec2 move;		// Specifies the mouse movement from the previous frame to the current frame.
	bool overWindow = false;	// Specifies whether the mouse is inside the window or not.
public:
	void init();
	void free() {}
	void update();

	inline const vec2& getPos() const { return pos; }
	inline const vec2& getMove() const { return move; }
	inline const vec2& getScroll() const { return scroll; }
	inline bool isOverWindow() const { return overWindow; }
	// return ray in NDC space
	Ray ray_cast() const;

	friend void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	// handle mouse position; top-left coordinate relative
	friend void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
	// handle mouse enter/leave window area
	friend void mouse_over_callback(GLFWwindow* window, int entered);
	// handle mouse scroll wheel
	friend void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

};
typedef InputDevice<KEYS_LOW_BOUND, KEYS_HIGH_BOUND> KeyboardDevice;
typedef InputDevice<0, 1> JoystickDevice;	// TODO
typedef MouseDevice MouseDevice;

extern JoystickDevice joystick;	// TODO: make singleton
extern KeyboardDevice keyboard;	// TODO: make singleton
extern MouseDevice mouse;		// TODO: make singleton

// CALLBACK functions
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
// handle mouse position; top-left coordinate relative
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// handle mouse enter/leave window area
void mouse_over_callback(GLFWwindow* window, int entered);
// handle mouse scroll wheel
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// handle move window event
void window_move_callback(GLFWwindow* window, int xoffset, int yoffset);

// SYSTEM TOOLS
std::string get_directory(const std::string &full_path);
std::string get_extension(const std::string &full_path);
std::string get_filename(const std::string &full_path);
std::string get_filename_extension(const std::string &full_path);
bool search_files(std::vector<std::string> *filenames, std::string *directory);

std::ostream& io_white(std::ostream &s);	// standard msg
std::ostream& io_red(std::ostream &s);		// for errors
std::ostream& io_yellow(std::ostream &s);	// for warnings
std::ostream& io_green(std::ostream &s);	// for success code


#endif	// INPUT_H