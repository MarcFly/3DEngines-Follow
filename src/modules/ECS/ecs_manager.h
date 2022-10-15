#pragma once
#include <src/modules/Module.h>
#include <src/helpers/Globals.h>
#include "ComponentSystem.h"

// Entities Hold Components
// Components Are Data
// Systems Update the Data and uses them in their systems

// Additionally Entities might interact with components
// Is that bad?
// Is the base entity supposed to do nothing

typedef System<void*, void*> SystemP;

class ModuleECS : public Module {
public:
	ModuleECS() : Module("ECS", true) {};
	~ModuleECS() {};

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	
	bool CleanUp();

	void RegisterSystem(SystemP* sys_p);

	// Data
	std::vector<SystemP*> systems;
};

extern ModuleECS _ecs;

