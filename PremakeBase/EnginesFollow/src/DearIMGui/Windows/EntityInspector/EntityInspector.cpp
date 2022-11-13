#include "EnginePCH.h"
#include "EntityInspector.h"
#include "ECS/ECS_Events.h"
#include "../EntityHierarchy/EntityHierarchy.h"

DEF_DYN_MEMBER_EV_FUNS(Engine::DeleteEntity_Event, EntityInspector, DeleteEntity_EventFun) {
	if (ev->entity_id == check_entity) check_entity = UINT64_MAX;
}}

DEF_DYN_MEMBER_EV_FUNS(HierarchySelect_Event, EntityInspector, SelectEntity_EventFun) {
	check_entity = ev->entity_id;
}}

EntityInspector::EntityInspector() : IMGui_Item("Entity Inspector") {
	Engine::Events::SubscribeDyn<Engine::DeleteEntity_Event>(DeleteEntity_EventFunRedirect, this);
	Engine::Events::SubscribeDyn<HierarchySelect_Event>(SelectEntity_EventFunRedirect, this);
}

void EntityInspector::RMMVirtualUpdate() {

}

void EntityInspector::Update() {
	bool changes_happened = false;

	ImGui::Begin(debug_name.c_str(), &active);
	if (check_entity == UINT64_MAX) {
		ImGui::End();
		return;
	}

	if (entity.id == UINT64_MAX || check_entity != entity.id) {
		entity.id = check_entity;
		Engine::Events::SendHotNew(new Engine::RequestSingleEntity_Event(entity));
		check_entity = entity.id;
		ImGui::End();
		return;
	}

	changes_happened |= ImGui::Checkbox("##ActiveEntityCheckbox", &entity.active);
	ImGui::SameLine();
	changes_happened |= ImGui::InputText("##EntityNameInspector", entity.name, sizeof(entity.name));

	// Get the components every single frame, as memory can get corrupted with vectors...
	// TODO: safer memory data type...
	Engine::Events::SendHotNew(new Engine::RequestEntityComponents_Event(check_entity, components));
	static char button_id[64];
	for (auto c : components) {
		ImGui::BeginChild(c->cid.id+1);
		snprintf(button_id, sizeof(button_id), "##Button%llu", c->cid.id);
		ImGui::Checkbox(button_id, &c->active);
		ImGui::SameLine();
		c->DrawInspector();

		ImGui::EndChild();
	}

	RMMUpdate(debug_name.c_str());

	ImGui::End();

	if (changes_happened)
		Engine::Events::SendNew(new Engine::ModifyEntity_Event(entity));

	


}