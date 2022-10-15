#include "ecs_manager.h"

ModuleECS _ecs;

#include "CS_MeshRenderer/cs_meshrenderer.h"

bool ModuleECS::Init()
{
	RegisterSystem((SystemP*)&mesh_renderer);

	return false;
}

update_status ModuleECS::PreUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	for (auto item : systems)
		ret = item->PreUpdate();
	return ret;
}

update_status ModuleECS::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleECS::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleECS::CleanUp()
{
	return false;
}

void ModuleECS::RegisterSystem(SystemP* sys_p)
{ 
	systems.push_back(sys_p);
}
