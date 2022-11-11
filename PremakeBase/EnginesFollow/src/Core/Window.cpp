#include "EnginePCH.h"

#include "Logger.h"
#include "Events/DefaultEvents.h"

#include "Window.h"
#include <GLFW/glfw3.h>

using namespace Engine;


WindowManager windowmanager;

//===================================================================
// CALLBACKS
//===================================================================

void GLFW_ErrorCallback(int error_code, const char* description)
{
    Engine_ERROR("GLFW_ERROR-{}: {}", error_code, description);
}

inline uint64_t GetWindow_FromGLFWwindow(GLFWwindow* window)
{
	for (auto& k : windowmanager.windows)
		if (k.second.internal_window == window)
			return k.first;

	// Will crash
	return windowmanager.windows.end()->first;
}

static void GLFW_CloseCallback(GLFWwindow* window)
{
	uint16_t& flags = ((Window*)glfwGetWindowUserPointer(window))->working_flags; // windowmanager.windows[GetWindow_FromGLFWwindow(window)].window->working_flags;
	SET_FLAG(flags, Window::WorkingFlags::CLOSE);
}



static void GLFW_FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	Window& win = *(Window*)glfwGetWindowUserPointer(window); //*windowmanager.windows[GetWindow_FromGLFWwindow(window)].window;
	win.w = width;
	win.h = height;
	win.ApplyChanges();
}

static void GLFW_MouseEnterLeaveCallback(GLFWwindow* window, int entered)
{
	Window& win = *(Window*)glfwGetWindowUserPointer(window); //  *windowmanager.windows[GetWindow_FromGLFWwindow(window)].window;
	if (entered > 0) {
		SET_FLAG(win.working_flags, Window::WorkingFlags::MOUSE_IN);
		// Send Event mouse in
	}
	else {
		CLEAR_FLAG(win.working_flags, Window::WorkingFlags::MOUSE_IN);
		// Send Event mouse out
	}
}

static void GLFW_DropFileCallback(GLFWwindow* window, int count, const char** paths)
{
	Window& win = *(Window*)glfwGetWindowUserPointer(window); // *windowmanager.windows[GetWindow_FromGLFWwindow(window)].window;
	FilesDropped* ev = new FilesDropped(paths, count);
	Events::Send(ev);
}


//===================================================================
// WINDOW SYSTEM INITIALIZATION
//===================================================================

EVENT_TYPE_STATIC_DATA(Window::WindowResize);

void Window::Init() {
	glfwSetErrorCallback(GLFW_ErrorCallback);

	
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE);
	if (!glfwInit()) exit(0);
	Engine_INFO("Initialized GLFW");
}

void Window::SetVSYNC(int frames_wait) {
	glfwSwapInterval(frames_wait);
}

void Window::Close() {
	for (auto& k : windowmanager.windows) {
		k.second.window->Destroy();
		windowmanager.windows.erase(k.first);
	}
}

//===================================================================
// WINDOW FUNCTIONS
//===================================================================

static int win_num = 0;

#include "Input/Input.h"

void Window::Create(bool defaults) {
	if (defaults) {
		context_hints.emplace_back(hintpair(GLFW_CONTEXT_VERSION_MAJOR, 4));
		context_hints.emplace_back(hintpair(GLFW_CONTEXT_VERSION_MINOR, 6));

		buffer_hints.emplace_back(hintpair(GLFW_RED_BITS, 8));
		buffer_hints.emplace_back(hintpair(GLFW_GREEN_BITS, 8));
		buffer_hints.emplace_back(hintpair(GLFW_BLUE_BITS, 8));
		buffer_hints.emplace_back(hintpair(GLFW_ALPHA_BITS, 8));
		// buffer_hints.emplace_back(hintpair(SQUE_BUFFER_SIZE, 32));
		buffer_hints.emplace_back(hintpair(GLFW_STENCIL_BITS, 0));
		buffer_hints.emplace_back(hintpair(GLFW_DEPTH_BITS, 24));
		//buffer_hints.emplace_back(hintpair(SQUE_RENDERABLE_TYPE, SQUE_MIN_RENDERABLE));

		w = 1600; h = 900;
		sprintf_s(title, "Default Window Name %d", win_num++);
	}

	id = simplehash(title);

	// Use vector as for other libraries, like egl, require passing by value...
	for (hintpair& hint : context_hints)
		glfwWindowHint(hint.first, hint.second);
	for (hintpair& hint : buffer_hints)
		glfwWindowHint(hint.first, hint.second);
	for (hintpair& hint : window_hints)
		glfwWindowHint(hint.first, hint.second);

	InternalWindow new_win(this); 
	new_win.id = id;
	
	new_win.internal_window = glfwCreateWindow(w, h, title, NULL, NULL);
	
	glfwSetWindowCloseCallback(new_win.internal_window, GLFW_CloseCallback);
	glfwSetFramebufferSizeCallback(new_win.internal_window, GLFW_FramebufferResizeCallback);
	glfwSetCursorEnterCallback(new_win.internal_window, GLFW_MouseEnterLeaveCallback);
	glfwSetDropCallback(new_win.internal_window, GLFW_DropFileCallback);
	glfwSetWindowUserPointer(new_win.internal_window, new_win.window);

	windowmanager.windows.insert(std::pair<uint64_t, InternalWindow>(id, new_win));

	Input::AttachWindow(id);
}

void Window::Destroy() {
	SET_FLAG(working_flags, WorkingFlags::CLOSE);
	auto& k = windowmanager.windows.find(id);
	if (k != windowmanager.windows.end()) {
		glfwDestroyWindow(k->second.internal_window);
		windowmanager.windows.erase(k->first);
	}
}

InternalWindow* Window::GetInternalWindow() {
	return &windowmanager.windows[id];
}

void Resize(const InternalWindow& win) {
	Window::WindowResize* ev = new Window::WindowResize(win.id, win.window->w, win.window->h);
	glfwSetWindowSize(win.internal_window, ev->w, ev->h);
	Events::SendHot(ev);
}

void SetTitle(const InternalWindow& win) {
	glfwSetWindowTitle(win.internal_window, win.window->title);
	// TODO: Window title event
	//Events::Send()
}

void Window::ApplyChanges() {
	const InternalWindow& win = windowmanager.windows[id];
	Resize(win);
	SetTitle(win);
}

void Window::MakeContextMain() {
	InternalWindow& win = windowmanager.windows[id];
	glfwMakeContextCurrent(win.internal_window);
}

void Window::SwapBuffers() {
	InternalWindow& win = windowmanager.windows[id];
	MakeContextMain();
	glfwSwapBuffers(win.internal_window);
	
	// TODO: call render to do that
	
}