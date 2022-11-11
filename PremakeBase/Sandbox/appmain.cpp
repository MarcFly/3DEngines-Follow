#include <Engine.h>


void ChangeMainWindow(Engine::Window& win) {
	sprintf(win.title, "New Title!");

	win.ApplyChanges();
}

#include <MathGeoLib.h>

struct base {
	template<typename T>
	static void inherited_static_fun(std::vector<T>&) {}
};

struct derivated : public base {
};


#include <ECS/DefaultComponents/CS_Transform.h>

int e_main(Engine::Application* engine) {	

	ChangeMainWindow(engine->main_window);
	
	engine->main_window.MakeContextMain();
	Engine::RenderAPI::InitForContext();

	Engine::ImGuiLayer imguilayer(engine->main_window.GetInternalWindow());
	engine->modules.PushOverlay(&imguilayer);

	imguilayer.RegisterSubmenu("Load", "File");

	Engine::ECS ecslayer;
	engine->modules.PushLayer(&ecslayer);
	Engine::FS::RegisterFiletaker(&ecslayer);

	Engine::Renderer renderer;
	engine->modules.PushLayer(&renderer);

	APP_INFO("Example");

	while (!CHECK_FLAG(engine->main_window.working_flags, Engine::Window::WorkingFlags::CLOSE))
	{
		Engine::Input::Poll();
		engine->Run();
		
		engine->main_window.SwapBuffers();
		Engine::RenderAPI::ClearFB(float4(.1, .1, .1, 1.));
	}

	engine->modules.PopOverlay(&imguilayer);
	engine->modules.PopLayer(&ecslayer);
	engine->modules.PopLayer(&renderer);

	APP_INFO("Quitting...");

	return 0;
}