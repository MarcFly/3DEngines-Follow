#pragma once

#include "../Globals.h"
#include <GLFW/glfw3.h>
#include "Events/Events.h"

namespace Engine {
	typedef std::pair<int32_t, int32_t> hintpair;

	struct Window;
	struct EF_API InternalWindow {
		InternalWindow(Window* _window = nullptr) : window(_window) {};
		uint64_t id;

		GLFWwindow* internal_window;

		Window* window; // 
	};

	struct EF_API Window {
		~Window() { Destroy(); }

		enum WorkingFlags {
			CLOSE = BITSET1,
			FULLSCREEN = BITSET2,
			MAXIMIZED = BITSET3,
			RESIZABLE = BITSET4,
			BORDERLESS = BITSET5,
			MOUSE_IN = BITSET6,
			VSYNC = BITSET7,
		};

		static void Init();
		static void Close();
		static void SetVSYNC(int frames_wait = 1);
		
		// Use data that you have setup with false
		void Create(bool defaults = true);
		void ApplyChanges();
		void MakeContextMain();
		void SwapBuffers();
		void Destroy();
		InternalWindow* GetInternalWindow();

		uint64_t id;
		char title[128] = "";
		float w = 0, h = 0;
		uint16_t working_flags = 0;

		std::vector<hintpair> context_hints;
		std::vector<hintpair> buffer_hints;
		std::vector<hintpair> window_hints;

		void* internal_window = nullptr;

		//========================================================================
		// EVENTS
		//========================================================================

		struct WindowResize : Event {
			EVENT_TYPE_CLASS(WindowResize);
			WindowResize(uint64_t _id, float _w, float _h) : id(_id), w(_w), h(_h) {}
			uint64_t id;
			float w, h;
		};
	};

	

	struct WindowManager {
		std::unordered_map<uint64_t, InternalWindow> windows;
	};
}