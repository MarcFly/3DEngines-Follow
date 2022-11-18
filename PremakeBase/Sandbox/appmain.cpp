#include <Engine.h>


void ChangeMainWindow(Engine::Window& win) {
	sprintf(win.title, "New Title!");

	win.ApplyChanges();
}

#include <MathGeoLib.h>

#include <ECS/DefaultComponents/CS_Transform.h>

int e_main(std::shared_ptr<Engine::Application>& engine) {	

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

	engine->modules.PopOverlay((Engine::Layer*)&imguilayer);
	engine->modules.PopLayer((Engine::Layer*)&ecslayer);
	engine->modules.PopLayer((Engine::Layer*)&renderer);

	APP_INFO("Quitting...");

	return 0;
}