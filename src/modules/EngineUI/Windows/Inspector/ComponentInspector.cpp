#include "ComponentInspector.h"
#include <src/Application.h>



void ComponentInspector::Update() {
	bool changes_happened = false;
	ImGui::Begin(name.c_str(), &active);

	if (check_entity == UINT64_MAX || App->ecs->GetEntity(check_entity) == nullptr) {
		check_entity = UINT64_MAX;
		ImGui::End();
		return;
	}

	changes_happened |= ImGui::Checkbox("##ActiveEntityCheckbox", &entity.active);

	ImGui::SameLine();
	changes_happened |= ImGui::InputText("##EntityNameInspector", entity.name, sizeof(entity.name));
	
	static char button_id[64];
	for (auto c : components) {
		sprintf(button_id, "##Button%llu", c->id.id);
		ImGui::Checkbox(button_id, &c->active);
		ImGui::SameLine();		
		c->DrawInspector();
	}

	ImGui::End();

	if(changes_happened)
		EV_SEND_POINTER(ECS_REQUEST_MODIFY_ENTITY, &entity);
}

void ComponentInspector::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
	for (std::shared_ptr<Event> ev : evt_vec) {
		switch (ev->type) {
		case HIERARCHY_SELECTED_ENTITY: {
			check_entity = ev->uint64;
			entity = *App->ecs->GetEntity(check_entity);
			GetComponentsFromEntity();
			entity_id_str = "##" + std::to_string(check_entity);
			continue;
		}
		case ECS_REQUEST_DELETE_ENTITY: {
			check_entity = UINT64_MAX;
			components.clear();
			entity_id_str.clear();
			continue;
		}
		// TODO: Event that calls for specific component removal
		// IF inside the components, call again the Entity and GetComponents

		}
	}
}

void ComponentInspector::GetComponentsFromEntity() {
	components.clear();
	if (check_entity != UINT64_MAX) {
		for (auto& c : entity.components)
			components.push_back(App->ecs->GetComponentGeneric(c));
	}
}