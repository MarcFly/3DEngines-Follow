#include "EnginePCH.h"
#include "ECS.h"
#include "ECS_Events.h"

using namespace Engine;

#include "DefaultComponents/CS_Transform.h"
#include "DefaultComponents/CS_MeshRenderer.h"

ECS::ECS() {
	RegisterSystem<S_Transform>();
	RegisterSystem<S_MeshRenderer>();
}

ECS::~ECS() {

}

//===================================================================
// ENTITY
//===================================================================

uint64_t Entity::default_name = 0;

//===================================================================
// ECS DATA CREATION AND MANAGEMENT
//===================================================================

#include "DefaultComponents/CS_Transform.h"

Entity* ECS::AddEntity(const uint64_t _parent_id) {
	entities.push_back(Entity());
	Entity& entity = entities.back();
	entity.parent_id = _parent_id;
	std::string temp = std::to_string(Entity::default_name++);
	memcpy(entity.name, temp.c_str(), temp.length());

	if (entity.parent_id == UINT64_MAX)
		base_entities.push_back(entity.id);
	else{
		Entity* p_entity = GetEntity(entity.parent_id);
		p_entity->children.push_back(entity.id);
	}

	return &entity;
}

Entity* ECS::AddCopyEntity(const Entity& entity_copy, const uint64_t parent_id) {
	entities.push_back(entity_copy);
	Entity& entity = entities.back();
	entity.id = PCGRand64();
	entity.parent_id = parent_id;

	for (int i = 0; i < entity.components.size(); ++i) {
		entity.components[i].parent_id = entity.id;
		entity.components[i] = AddCopyComponentGeneric(entity.components[i], entity.id);
	}

	if (entity.parent_id == UINT64_MAX)
		base_entities.push_back(entity.id);
	else {
		Entity* p_entity = GetEntity(entity.parent_id);
		p_entity->children.push_back(entity.id);
	}

	// TODO: Send Event per childre to add copy entity with updated parenting

	return &entity;
}

Entity* ECS::GetEntity(const uint64_t entity_id) {
	int size = entities.size();
	for (int i = 0; i < size; ++i)
		if (entity_id == entities[i].id)
			return &entities[i];
	return nullptr;
}

const Entity* ECS::GetEntityConst(const uint64_t entity_id) const {
	int size = entities.size();
	for (int i = 0; i < size; ++i)
		if (entity_id == entities[i].id)
			return &entities[i];
	return nullptr;
}

void ECS::DeleteEntity(const uint64_t entity_id) {
	for (int i = 0; i < entities.size(); ++i) {
		Entity& curr_entity = entities[i];
		if (curr_entity.id == entity_id) {
			for (int j = 0; j < curr_entity.children.size(); ++j) {
				// TODO: Send Event to delete entity
			}

			for (int j = 0; j < curr_entity.components.size(); ++j) {
				DeleteComponent(curr_entity.components[j]);
			}
				
			if (curr_entity.parent_id == UINT64_MAX) {
				// TODO: Send Event to refresh base entities
			}
			else {
				Entity* parent = GetEntity(curr_entity.parent_id);
				if (parent != nullptr) {
					std::vector<uint64_t> swapvec;
					swapvec.reserve(parent->children.size());
					for (int j = 0; j < parent->children.size(); ++j) {
						if (parent->children[j] == curr_entity.id) continue;
						swapvec.push_back(parent->children[j]);
					}
					parent->children.swap(swapvec);
				}
			}

			curr_entity = entities.back();
			entities.pop_back();
			return;
		}
	}
}

CID ECS::AddComponentGeneric(const uint64_t type, const uint64_t entity_id) {
	return GetSystemOfType(type)->AddGeneric(entity_id);
}

CID ECS::AddCopyComponentGeneric(CID& cid, const uint64_t entity_id) {
	uint64_t cache_parent = cid.parent_id;
	const Component* c = GetComponentGeneric(cid);
	return AddCopyOfComponentGeneric(c, cache_parent);
}

CID ECS::AddCopyOfComponentGeneric(const Component* copy, const uint64_t entity_id) {
	return GetSystemOfType(copy->cid.ctype)->AddCopyOfGeneric(copy, entity_id);
}

Component* ECS::GetComponentGeneric(CID& cid) {
	return GetSystemOfType(cid.ctype)->GetGeneric(cid);
}

void ECS::DeleteComponent(const CID& cid) {
	GetSystemOfType(cid.ctype)->DeleteGeneric(cid);
}

//===================================================================
// ECS FLOW MANAGEMENT
//===================================================================

#include "DearIMGui/Windows/EntityHierarchy/EntityHierarchy.h"

void ECS::OnAttach() {
	Events::SubscribeDyn<NewEntity_Event>(AddEntity_EventFunRedirect, this);
	Events::SubscribeDyn<DeleteEntity_Event>(DeleteEntity_EventFunRedirect, this);
	Events::SubscribeDyn<ModifyEntity_Event>(ModifyEntity_EventFunRedirect, this);
	Events::SubscribeDyn<AddComponent_Event>(AddComponent_EventFunRedirect, this);
	Events::SubscribeDyn<DeleteComponent_Event>(DeleteComponent_EventFunRedirect, this);
	Events::SubscribeDyn<LOADJSONPrefab_Event>(LOADJSONPrefab_EventFunRedirect, this);

	Events::SubscribeDyn<RequestEntities_Event > (RequestEntities_EventFunRedirect, this);
	
	Events::SubscribeDyn<RequestSingleEntity_Event>(RequestSingleEntity_EventFunRedirect, this);
	Events::SubscribeDyn<RequestEntityComponents_Event>(RequestEntityComponents_EventFunRedirect, this);
}



void ECS::OnDetach() {
	// TODO: Unsubscribe Dyns...

	Events::SendHotNew(new ProvideEntities_Event(nullptr));

	CleanUp();
}

// TODO: DELTA TIME!

void ECS::PreUpdate() {
	for (auto& sys : systems)
		sys.second->PreUpdate(0);
}

void ECS::Update() {
	for (auto& sys : systems)
		sys.second->Update(0);
}

void ECS::PostUpdate() {
	for (auto& sys : systems)
		sys.second->PostUpdate(0);
}

void ECS::CleanUp() {
	for (auto& sys : systems)
		sys.second->CleanUp();
	systems.clear();
	entities.clear();
	base_entities.clear();
}


//===================================================================
// ECS SERIALIZATION
//===================================================================

JSON_Value* ECS::SerializeScene() {
	JSON_Value* retval = json_value_init_object();
	JSON_Object* base_obj = json_object(retval);

	json_object_set_string(base_obj, "scenename", scenename);

	JSON_Value* entities_val = json_value_init_array();
	JSON_Array* entities_arr = json_array(entities_val);

	for (int i = 0; i < entities.size(); ++i) {
		Entity& e = entities[i];
		JSON_Value* e_val = json_value_init_object();
		JSON_Object* e_obj = json_object(e_val);

		json_object_set_u64(e_obj, "id", e.id);
		json_object_set_u64(e_obj, "parent_id", e.parent_id);
		json_object_set_boolean(e_obj, "active", e.active);
		json_object_set_string_with_len(e_obj, "name", e.name, strlen(e.name));

		JSON_Value* children_val = json_value_init_array();
		JSON_Array* children_arr = json_array(children_val);
		for (int i = 0; i < e.children.size(); ++i)
			json_array_append_u64(children_arr, e.children[i]);

		json_object_set_value(e_obj, "children", children_val);

		JSON_Value* comps_val = json_value_init_array();
		JSON_Array* comps_arr = json_array(comps_val);

		JSON_Value* sys_val = json_object_get_value(base_obj, "systems");
		JSON_Array* sys_arr = json_array(sys_val);

		for (int i = 0; i < e.components.size(); ++i) {
			const CID& c = e.components[i];
			JSON_Value* c_val = json_value_init_object();
			JSON_Object* c_obj = json_object(c_val);

			json_object_set_u64(c_obj, "ctype", c.ctype);
			json_object_set_u64(c_obj, "id", c.id);

			json_array_append_value(comps_arr, c_val);
		}
		json_object_set_value(e_obj, "components", comps_val);

		json_array_append_value(entities_arr, e_val);
	}
	json_object_set_value(base_obj, "entities", entities_val);

	JSON_Value* sys_val = json_value_init_array();
	JSON_Array* sys_arr = json_array(sys_val);

	for (auto sys : systems) {
		System* s = sys.second;
		JSON_Value* curr_sys_val = json_value_init_object();
		JSON_Object* curr_sys_obj = json_object(curr_sys_val);
		json_object_set_string(curr_sys_obj, "name", s->GetName());
		json_object_set_u64(curr_sys_obj, "ctype", s->ctype);
		
		s->JSONSerializeComponents(curr_sys_obj);

		json_array_append_value(sys_arr, curr_sys_val);
	}
	json_object_set_value(base_obj, "systems", sys_val);

	return retval;
}


void System::SerializeSingleComponent(CID& cid, JSON_Array* comps_arr) {
	const Component* c = GetConstGeneric(cid);
	if (c != nullptr) {
		JSON_Value* v = JSONValueFromComponent(c);
		if (v != nullptr)
			json_array_append_value(comps_arr, v);
	}
}

// Recursive, bad but easy
void ECS::SerializePrefab(const uint64_t entity_id, JSON_Value* value) {
	if (entity_id == UINT64_MAX) return;
	Entity& e = *GetEntity(entity_id);
	JSON_Object* base_obj = json_object(value);
	JSON_Array* entity_arr = json_array(json_object_get_value(base_obj, "entities"));

	JSON_Value* e_val = json_value_init_object();
	JSON_Object* e_obj = json_object(e_val);

	json_object_set_u64(e_obj, "id", e.id);
	json_object_set_u64(e_obj, "parent_id", e.parent_id);
	json_object_set_boolean(e_obj, "active", e.active);
	json_object_set_string_with_len(e_obj, "name", e.name, sizeof(e.name));

	JSON_Value* children_val = json_value_init_array();
	JSON_Array* children_arr = json_array(children_val);
	for (int i = 0; i < e.children.size(); ++i)
		json_array_append_u64(children_arr, e.children[i]);

	json_object_set_value(e_obj, "children", children_val);

	JSON_Value* comps_val = json_value_init_array();
	JSON_Array* comps_arr = json_array(comps_val);
	
	JSON_Value* sys_val = json_object_get_value(base_obj, "systems");
	JSON_Array* sys_arr = json_array(sys_val);
	size_t num_systems = json_array_get_count(sys_arr);

	for (int i = 0; i < e.components.size(); ++i) {
		CID& c = e.components[i];
		JSON_Value* c_val = json_value_init_object();
		JSON_Object* c_obj = json_object(c_val);

		json_object_set_u64(c_obj, "ctype", c.ctype);
		json_object_set_u64(c_obj, "id", c.id);
		
		json_array_append_value(comps_arr, c_val);

		for (int j = 0; j < num_systems; ++j) {
			JSON_Object* sys_obj = json_array_get_object(sys_arr, j);
			uint64_t sys_type = json_object_get_u64(sys_obj, "ctype");
			if (sys_type == c.ctype)
				GetSystemOfType(sys_type)->SerializeSingleComponent(c, json_object_get_array(sys_obj, "components"));
		}
	}

	json_object_set_value(e_obj, "components", comps_val);

	json_array_append_value(entity_arr, e_val);
}

void ECS::DeserializePrefab(const JSON_Value* json_value) {
	const JSON_Object* base_obj = json_object(json_value);

	const char* temp_scenename = json_object_get_string(base_obj, "scenename");
	if(temp_scenename != nullptr)
		memcpy(scenename, temp_scenename, strlen(temp_scenename));

	const JSON_Array* entities_arr = json_object_get_array(base_obj, "entities");
	const size_t num_entities = json_array_get_count(entities_arr);
	for (int i = 0; i < num_entities; ++i) {
		const JSON_Object* e_obj = json_array_get_object(entities_arr, i);
		Entity* e = AddEntity(json_object_get_u64(e_obj, "parent_id"));

		e->parent_id = json_object_get_u64(e_obj, "parent_id");
		const uint64_t tid = json_object_get_u64(e_obj, "id");
		if (e->parent_id == UINT64_MAX) {
			for (int j = 0; j < base_entities.size(); ++j)
				if (base_entities[j] == e->id) {
					base_entities[j] = tid;
					break;
				}
		} e->id = tid;
		
		e->active = json_object_get_boolean(e_obj, "active");
		const char* name_str = json_object_get_string(e_obj, "name");
		memcpy(e->name, name_str, strlen(name_str));

		const JSON_Array* children_arr = json_object_get_array(e_obj, "children");
		const size_t num_children = json_array_get_count(children_arr);
		for (int j = 0; j < num_children; ++j)
			e->children.push_back(json_array_get_u64(children_arr, j));

		const JSON_Array* comps_arr = json_object_get_array(e_obj, "components");
		const size_t nump_components = json_array_get_count(comps_arr);
		for (int j = 0; j < nump_components; ++j) {
			const JSON_Object* comp_obj = json_array_get_object(comps_arr, j);
			e->components.push_back(CID(UINT64_MAX));
			CID& cid = e->components.back();
			cid.id = json_object_get_u64(comp_obj, "id");
			cid.ctype = json_object_get_u64(comp_obj, "ctype");
			cid.parent_id = e->id;
		}
	}

	const JSON_Array* sys_arr = json_object_get_array(base_obj, "systems");
	const size_t num_systems = json_array_get_count(sys_arr);
	for (int i = 0; i < num_systems; ++i) {
		const JSON_Object* sys_obj = json_array_get_object(sys_arr, i);
		const uint64_t sys_type = json_object_get_u64(sys_obj, "ctype");
		System* sys = GetSystemOfType(sys_type);
		if (sys == nullptr) {
			Engine_ERROR("System {} of type {}, has not been registered", sys_type, json_object_get_string(sys_obj, "name")); 
			continue;
		}
		sys->JSONDeserializeComponents(sys_obj);
	}

	// TODO: shuffle ids, how to do that? Could also, change ids to have prefabid/sceneid + working id, makes it easier
}

uint32_t ECS::ShouldILoad(const char* ext) {
	uint32_t ret = 0;
	if (!stricmp(ext, ".jsonscene"))
		ret = 1;

	return ret;
}

std::shared_ptr<FileVirtual>ECS::TryLoad(TempIfStream& disk_mem, const uint32_t internaltype) {
	
	std::shared_ptr<JSONVWrap> ret(new JSONVWrap());
	ret->Load(disk_mem);

	// TODO: Load into a local ecs
	// in the local_ecs, shuffle ids, as there are less operationst to do
	// then merge into full ecs

	DeserializePrefab(ret->value);

	return std::static_pointer_cast<FileVirtual>(ret);
}