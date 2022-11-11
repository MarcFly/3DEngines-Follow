#pragma once

#include "Globals.h"
#include <imgui.h>
#include "../../DearIMGuiLayer.h"
#include "ECS/ECS.h"

struct EntityInspector : public Engine::IMGui_Item, public Engine::RMPopupMenu {
	uint64_t check_entity = UINT64_MAX;
	std::string entity_id_str;
	Engine::Entity entity;
	std::vector<Engine::Component*> components;

	EntityInspector();
	void RMMVirtualUpdate();

	DECL_DYN_EV_FUNS(DeleteEntity_EventFun);
	DECL_DYN_EV_FUNS(SelectEntity_EventFun);

	void GetComponentsFromEntity();

	void Update();
};