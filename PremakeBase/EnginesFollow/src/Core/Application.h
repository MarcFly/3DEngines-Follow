#pragma once

#include "../Globals.h"
#include "Window.h"
#include "Layer.h"

namespace Engine {
	struct EF_API Application {
		Application(bool quick_init = true);
		~Application();
		void Run();

		static void RegisterDebugCallbacks();

		Window main_window;
		LayerStack modules;
	};

}