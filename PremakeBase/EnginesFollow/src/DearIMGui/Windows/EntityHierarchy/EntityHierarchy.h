#pragma once
#include "Globals.h"

#include "../../DearIMGuiLayer.h"
#include "ECS/ECS.h"


struct RequestEntities_Event : public Engine::Event {
	EVENT_TYPE_CLASS(RequestEntities_Event);
};

struct ProvideEntities_Event : public Engine::Event {
	EVENT_TYPE_CLASS(ProvideEntities_Event);
	ProvideEntities_Event(const Engine::ECS* _ecs) : ecs(_ecs) {}

	const Engine::ECS* ecs = nullptr;
};

struct HierarchySelect_Event : public Engine::Event {
	EVENT_TYPE_CLASS(HierarchySelect_Event);
	HierarchySelect_Event(const uint64_t _entity_id) : entity_id(_entity_id) {}
	uint64_t entity_id;
};

struct EntityHierarchyWindow : public Engine::IMGui_Item, public Engine::RMPopupMenu {
	EntityHierarchyWindow() : IMGui_Item("Enity Hierarchy") {
		Engine::Events::SubscribeDyn<ProvideEntities_Event>(UpdateEntities_EventFunRedirect, this);
	};
	~EntityHierarchyWindow() {};

	void Update();

	void RMMVirtualUpdate();

	std::vector<uint64_t> selected;
	const Engine::ECS* read_ecs = nullptr;
	bool IsSelected(uint64_t eid) { for (auto v : selected) if (eid == v) return true; return false; }
	void UpdateEntry(const uint64_t entry);

	DECL_DYN_EV_FUNS(UpdateEntities_EventFun);
};


