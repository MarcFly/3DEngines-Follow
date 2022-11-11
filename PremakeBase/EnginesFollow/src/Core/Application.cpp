#include <EnginePCH.h>
#include "Application.h"

#include "Logger.h"
#include "Events/Events.h"
#include "Window.h"
#include "Input/Input.h"
#include "RenderAPI/Render.h"
#include "FileSystem/FS.h"

#include <pcg_basic.h>
pcg32_random_t rngptr_a, rngptr_b;

using namespace Engine;

Application::Application(bool quick_init) {
	pcg32_srandom_r(&rngptr_a, time(NULL), (intptr_t)&rngptr_a);
	pcg32_srandom_r(&rngptr_b, time(NULL), (intptr_t)&rngptr_b);

	if (quick_init) {
		Logger::Init();
		Events::Init();
		Window::Init();
		Input::Init();
		FS::Init();

		main_window.Create();
		main_window.MakeContextMain();
		RenderAPI::InitForContext();
		Engine::Application::RegisterDebugCallbacks();
	}
}

void Application::RegisterDebugCallbacks() {
	Events::Subscribe<MousePosEvent>([](Event* ev) {
		MousePosEvent* mpev = (MousePosEvent*)ev;
		Engine_INFO("MousePosEvent: {} {}", mpev->x, mpev->y); });
	Events::Subscribe<KeyEvent>([](Event* ev) {
		KeyEvent* kev = (KeyEvent*)ev;
		Engine_INFO("KeyEvent: {} {}", (char)kev->code, kev->state); });
	Events::Subscribe<ScrollEvent>([](Event* ev) {
		ScrollEvent* screv = (ScrollEvent*)ev;
		Engine_INFO("MousePosEvent: {} {}", screv->x, screv->y); });
}

Application::~Application() {
	main_window.Destroy();
}

void Application::Run() {
	
	for (Layer* l : modules.layers)
		l->PreUpdate();
	for (Layer* o : modules.overlays)
		o->PreUpdate();

	Events::DispatchAll();

	for (Layer* l : modules.layers)
		l->Update();
	for (Layer* o : modules.overlays)
		o->Update();

	Events::DispatchAll();

	for (Layer* l : modules.layers)
		l->PostUpdate();
	for (Layer* o : modules.overlays)
		o->PostUpdate();

	Events::DispatchAll();
}