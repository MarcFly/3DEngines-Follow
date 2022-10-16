#include "ModuleECS.h"
#include "ComponentsIncludeAll.h"
#include <src/helpers/Timer/sque_timer.h>

ModuleECS::ModuleECS() : Module("ECS") {
	InitTimer();
	CalibrateTimer();
	systems.push_back(new S_MeshRenderer());
}

ModuleECS::~ModuleECS() {
	for (System* system : systems)
		delete system;
	systems.clear();
}


bool ModuleECS::Init()
{
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->Init();

	return true;
}

bool ModuleECS::Start()
{
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->Start();

	return true;
}

update_status ModuleECS::PreUpdate(float dt)
{
	SQUE_Timer t;
	t.Start();
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->PreUpdate(dt);
	//PLOG("%f\n", t.ReadMicroSec());
	return UPDATE_CONTINUE;
}

update_status ModuleECS::Update(float dt)
{
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->Update(dt);
	return UPDATE_CONTINUE;
}

update_status ModuleECS::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleECS::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
	for (std::shared_ptr<Event> ev : evt_vec) {
		switch (ev->type) {
		case ECS_REQUEST_NEW_ENTITY:
			AddEntity(ev->uint64);
			continue;
		case ECS_REQUEST_DELETE_ENTITY:
			DeleteEntity(ev->uint64);
			continue;
		case ECS_REQUEST_MODIFY_ENTITY:
			Entity* cpy = (Entity*)ev->generic_pointer;
			Entity* get = GetEntity(cpy->id);
			if (get == nullptr) { EV_SEND_UINT64(ECS_REQUEST_DELETE_ENTITY, cpy->id); }
			else { *GetEntity(cpy->id) = *cpy; }
			continue;
		}
	}
}

bool ModuleECS::CleanUp()
{
	return true;
}
