#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
	// Scenes

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{	
	std::list<Module*>::iterator item = --list_modules.end();
	while (true) {
		if (*item == nullptr) { if (item == list_modules.begin()) break; --item; continue; }
		delete* item;
		*item = nullptr;

		if (item == list_modules.begin())
			break;

		--item;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	for (Module* item : list_modules) {
		item->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	
	for (Module* item : list_modules) {
		item->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	//Do here object move?

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	for (Module* item : list_modules) {
		if (ret != UPDATE_CONTINUE) break;
		ret = item->PreUpdate(dt);
	}

	for (Module* item : list_modules) {
		if (ret != UPDATE_CONTINUE) break;
		ret = item->Update(dt);
	}

	for (Module* item : list_modules) {
		if (ret != UPDATE_CONTINUE) break;
		ret = item->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	std::list<Module*>::iterator item = --list_modules.end();
	while (true) {
		delete* item;
		*item = nullptr;

		if (item == list_modules.begin())
			break;

		--item;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}