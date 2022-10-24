#pragma once

#include <src/helpers/Globals.h>
#include <src/helpers/Timer/sque_timer.h>
#include <src/modules/Module.h>
#include <src/modules/ModuleWindow.h>
#include <src/modules/ModuleInput.h>
#include <src/modules/Render/ModuleRenderer3D.h>
#include <src/modules/ModuleCamera3D.h>
#include <src/modules/EngineUI/ModuleEngineUI.h>
#include <src/modules/EventSystem/ModuleEventSystem.h>
#include <src/modules/FileSystem/ModuleFS.h>
#include <src/modules/ECS/ModuleECS.h>

#include<list>
#include<vector>

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEngineUI* engine_ui;
	ModuleEventSystem* events;
	ModuleFS* fs;
	ModuleECS* ecs; // = &_ecs;

private:
	TSCNS_Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	void SendEvents(std::vector<std::shared_ptr<Event>>& evt_vec);
	update_status Update();
	bool CleanUp();

	void Save(JSON_Object* root_node);
	void Load(JSON_Object* root_node);
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;