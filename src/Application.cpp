#include <src/Application.h>

Application::Application()
{
	window = new ModuleWindow();
	input = new ModuleInput();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	events = new ModuleEventSystem();
	fs = new ModuleFS();

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

	// UI Even further last!
	AddModule(this->engine_ui);

	AddModule(fs);
	AddModule(events);
}

Application::~Application()
{	
	std::list<Module*>::iterator item = --list_modules.end();
	while (true) {
		if (*item == nullptr || (*item)->IsStaticModule()) { 
			if (item == list_modules.begin()) 
				break; 
			--item; continue; }
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
	//std::list<Module*>::iterator item = list_modules.begin();

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
void Application::SendEvents(std::vector<std::shared_ptr<Event>>& evt_vec) {
	for (Module* item : list_modules) {
		item->ReceiveEvents(evt_vec);
	}

	evt_vec.clear();
}

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
	do {
		if ((*item)->IsStaticModule()) {
			--item; 
			continue;
		}
		delete* item;
		*item = nullptr;

		--item;
	} while (item != list_modules.begin());
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}