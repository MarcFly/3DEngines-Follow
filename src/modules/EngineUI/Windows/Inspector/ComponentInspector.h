#pragma once

#include "../../ui_item.h"
#include "../../ModuleEngineUI.h"
#include <src/modules/ECS/ModuleECS.h>

struct ComponentInspector : UI_Item {
	std::string entity_id_str;
	Entity* entity = nullptr;

	ComponentInspector() : UI_Item("Inspector") {};

	RMPopupMenu rm_menu;
	void UpdateRMMenu();

	void Update();
	
};
