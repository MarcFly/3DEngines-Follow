#pragma once

#include "../../ui_item.h"
#include "../../ModuleEngineUI.h"
#include <src/modules/ECS/ModuleECS.h>

struct ComponentInspector : UI_Item {
	uint64_t check_entity = UINT64_MAX;
	std::string entity_id_str;
	Entity entity;
	std::vector<Component*> components;

	ComponentInspector() : UI_Item("Inpsector") {};

	void GetComponentsFromEntity();

	void Update();
	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);
	
};
