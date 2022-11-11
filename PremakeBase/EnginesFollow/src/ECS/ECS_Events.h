#pragma once

#include "Events/Events.h"
#include "FileSystem/DefaultFileTypes.h"
#include "Entity.h"

namespace Engine {

	struct NewEntity_Event : public Event {
		EVENT_TYPE_CLASS(NewEntity_Event);
		NewEntity_Event(uint64_t _parent_id = UINT64_MAX, uint64_t _entity_copy = UINT64_MAX) : entity_copy(_entity_copy), parent_id(_parent_id) {}
		uint64_t entity_copy;
		uint64_t parent_id;
	};

	struct DeleteEntity_Event : public Event {
		EVENT_TYPE_CLASS(DeleteEntity_Event);
		DeleteEntity_Event(uint64_t _entity_id = UINT64_MAX) : entity_id(_entity_id) {}
		uint64_t entity_id;
	};

	struct ModifyEntity_Event : public Event {
		EVENT_TYPE_CLASS(ModifyEntity_Event);
		ModifyEntity_Event(const Entity& entity) : cpy(entity) {}
		Entity cpy;
	};

	struct AddComponent_Event : public Event {
		EVENT_TYPE_CLASS(AddComponent_Event);
		AddComponent_Event(const uint64_t _parent_id, const uint64_t _component_type, const uint64_t _copy_component_id = UINT64_MAX) :
			parent_id(_parent_id), component_type(_component_type), copy_component_id(_copy_component_id) {}
		uint64_t parent_id, component_type, copy_component_id;
	};

	struct DeleteComponent_Event : public Event {
		EVENT_TYPE_CLASS(DeleteComponent_Event);
		DeleteComponent_Event(const CID _cid) : cid(_cid) {}
		const CID cid;
	};

	struct LOADJSONPrefab_Event : public Event {
		EVENT_TYPE_CLASS(LOADJSONPrefab_Event);
		LOADJSONPrefab_Event(JSONVWrap& json_value) : json(json_value) {}
		
		JSONVWrap& json;
	};

	struct RequestSingleEntity_Event : public Event {
		EVENT_TYPE_CLASS(RequestSingleEntity_Event);
		RequestSingleEntity_Event(Entity& entity_ref) : entity(entity_ref) {}
		Entity& entity;
	};

	struct RequestEntityComponents_Event : public Event {
		EVENT_TYPE_CLASS(RequestEntityComponents_Event);
		RequestEntityComponents_Event(const uint64_t _entity_id, std::vector<Component*>& _fill_vec) : entity_id(_entity_id), components(_fill_vec) {}
		const uint64_t entity_id;
		std::vector<Component*>& components;
	};
};