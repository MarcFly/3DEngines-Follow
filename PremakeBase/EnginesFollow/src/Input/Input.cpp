#include "EnginePCH.h"

#include "Input.h"
#include "InputEnums.h"

using namespace Engine;

Key Input::keys[INPUT::KB::KB_MAX];
Pointer Input::pointers[10];
EVENT_TYPE_STATIC_DATA(KeyEvent);
EVENT_TYPE_STATIC_DATA(MousePosEvent);
EVENT_TYPE_STATIC_DATA(ScrollEvent);


#include <GLFW/glfw3.h>

//==================================================================
// GLFW CALLBACKS
//==================================================================

static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Key& curr_key = Input::keys[key];
    curr_key.prev_state = curr_key.state;
    curr_key.state = action;

    // Basically like a sendhot, but specific to the key
    KeyEvent* kevent = new KeyEvent(key, action, curr_key.prev_state);
    for(KeyFun* key_fun : Input::keys[key].callbacks)
        key_fun(kevent);

    Events::SendHot(kevent);

}

std::vector<char> char_buffer;
static void GLFW_CharCallback(GLFWwindow* window, uint32_t codepoint)
{
    char_buffer.push_back(codepoint);
}

static void GLFW_MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Pointer& p = Input::pointers[0];
    p.x = (float)xpos;
    p.y = (float)ypos;
    MousePosEvent* posevent = new MousePosEvent(0, p.x, p.y);
    for (MousePosFun* mpos_fun : p.pos_callbacks)
        mpos_fun(posevent);
    Events::SendHot(posevent);
}
static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Key& mbutton = Input::pointers[0].buttons[button];
    mbutton.prev_state = mbutton.state;
    mbutton.state = action;

    // Add MouseButtonEvent != KeyEvent, important as it might messup some 0-16 non specified keys
    KeyEvent* kevent = new KeyEvent(button, action, mbutton.prev_state);
    for (KeyFun* key_fun : mbutton.callbacks)
        key_fun(kevent);

    for (VoidFun* key_fun : mbutton.state_specific_cb[action])
        key_fun();

    Events::SendHot(kevent);
}

static void GLFW_MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Pointer& p = Input::pointers[0];
    p.scrollx = xoffset;
    p.scrolly = yoffset;

    ScrollEvent* scrollev = new ScrollEvent(xoffset, yoffset);
    Events::SendHot(scrollev);
}

//==================================================================
// GENERAL CALLBACKS
//==================================================================

void Input::AddKeyCallback(int code, KeyFun* callback) {
    keys[code].callbacks.push_back(callback);
}
void Input::AddKeyPressCallback(int code, VoidFun* callback) {
    keys[code].state_specific_cb[INPUT::ACTIONS::PRESS].push_back(callback);
}
void Input::AddKeyReleaseCallback(int code, VoidFun* callback) {
    keys[code].state_specific_cb[INPUT::ACTIONS::RELEASE].push_back(callback);
}
void Input::AddKeyRepeatCallback(int code, VoidFun* callback) {
    keys[code].state_specific_cb[INPUT::ACTIONS::REPEAT].push_back(callback);
}
void Input::AddMouseButtonCallback(int mouse_button, KeyFun* callback, int id) {
    pointers[id].buttons[mouse_button].callbacks.push_back(callback);
}
void Input::AddMouseButtonPressCallback(int mouse_button, VoidFun* callback, int id) {
    pointers[id].buttons[mouse_button].state_specific_cb[INPUT::ACTIONS::PRESS].push_back(callback);
}
void Input::AddMouseButtonReleaseCallback(int mouse_button, VoidFun* callback, int id) {
    pointers[id].buttons[mouse_button].state_specific_cb[INPUT::ACTIONS::RELEASE].push_back(callback);
}
void Input::AddMouseButtonRepeatCallback(int mouse_button, VoidFun* callback, int id) {
    pointers[id].buttons[mouse_button].state_specific_cb[INPUT::ACTIONS::REPEAT].push_back(callback);
}

//==================================================================
// INPUT INITIALIZATION
//==================================================================

#include "Core/Window.h"
extern WindowManager windowmanager;

void Input::Init() {

}

void Input::AttachWindow(uint64_t window_id) {
    GLFWwindow* glfwin = windowmanager.windows[window_id].internal_window;

    glfwSetInputMode(glfwin, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetKeyCallback(glfwin, GLFW_KeyCallback);

    glfwSetCursorPosCallback(glfwin, GLFW_MousePosCallback);
    glfwSetMouseButtonCallback(glfwin, GLFW_MouseButtonCallback);
    glfwSetScrollCallback(glfwin, GLFW_MouseScrollCallback);
    glfwSetCharCallback(glfwin, GLFW_CharCallback);    
}

void Input::Close() {

}

void Input::Poll() {
    glfwPollEvents();
}

//==================================================================
// GETTERS
//==================================================================

void Input::GetPointerPos(float& x, float& y, int pointer_id) {
    Pointer& p = pointers[pointer_id];
    x = p.x;
    y = p.y;
}
void Input::GetPointerButton(int& state, INPUT::MOUSE button, int pointer_id) {
    state = pointers[pointer_id].buttons[button].state;
}
void Input::GetPointerScroll(float& x, float& y, int pointer_id) {
    Pointer& p = pointers[pointer_id];
    x = p.scrollx;
    y = p.scrolly;
}
void Input::GetKey(int& state, int code) {
    state = keys[code].state;
}

void Input::GetAvgPointerPos(float& x, float& y) {
    x = y = 0;
    float active_pointers = 0;
    for (const Pointer& p : pointers) {
        if (p.buttons[0].state > 0) {
            x += p.x;
            y += p.y;
            ++active_pointers;
        }
    }

    x /= active_pointers;
    y /= active_pointers;
}