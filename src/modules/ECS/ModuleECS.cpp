#include "ModuleECS.h"
#include "ComponentsIncludeAll.h"
#include <src/helpers/Timer/sque_timer.h>

ModuleECS::ModuleECS() : Module("ECS") {
	InitTimer();
	CalibrateTimer();
	systems.push_back(new S_MeshRenderer());
	systems.push_back(new S_Transform());
}

ModuleECS::~ModuleECS() {
	for (System* system : systems)
		delete system;
	systems.clear();
}


bool ModuleECS::Init()
{
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->Init();

	return true;
}

bool ModuleECS::Start()
{
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->Start();

	return true;
}

update_status ModuleECS::PreUpdate(float dt)
{
	SQUE_Timer t;
	t.Start();
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->PreUpdate(dt);
	//PLOG("%f\n", t.ReadMicroSec());
	return UPDATE_CONTINUE;
}

update_status ModuleECS::Update(float dt)
{
	for (int i = 0; i < systems.size(); ++i)
		systems[i]->Update(dt);
	return UPDATE_CONTINUE;
}

update_status ModuleECS::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleECS::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
	for (std::shared_ptr<Event> ev : evt_vec) {
		switch (ev->type) {
		case ECS_REQUEST_NEW_ENTITY: {
			Entity* e = AddEntity(ev->uint64);
			AddComponent<C_Transform>(e->id, CT_Transform);
			continue;
		}
		case ECS_REQUEST_DELETE_ENTITY:
			DeleteEntity(ev->uint64);
			continue;
		case ECS_REQUEST_MODIFY_ENTITY: {
			Entity* cpy = (Entity*)ev->generic_pointer;
			Entity* get = GetEntity(cpy->id);
			if (get == nullptr) { EV_SEND_UINT64(ECS_REQUEST_DELETE_ENTITY, cpy->id); }
			else { *GetEntity(cpy->id) = *cpy; }
			continue;
		}
		case ECS_ADD_COMPONENT: {
			Entity* e = GetEntity(ev->uint64_pair[0]);
			Component* c = AddComponent<Component>(e->id, (ComponentTypes)ev->uint64_pair[1]);
			e->AddComponent(c->id);
			continue;
		}
		case ECS_LOAD_JSONPREFAB: {
			DeserializePrefab(ev->uint64);
			continue;
		}
		}
	}
}

bool ModuleECS::CleanUp()
{
	for (System* sys : systems) {
		sys->CleanUp();
		delete sys;
		sys = nullptr;
	}
	systems.clear();
	entities.clear();
	base_entities.clear();

	return true;
}

void ModuleECS::DeserializePrefab(const uint64_t json_prefab_id) {
	const JSONVWrapper* jvw = App->fs->RetrievePValue<JSONVWrapper>(json_prefab_id);
	const JSON_Value* prefab_v = jvw->value;
	if (prefab_v == nullptr) return;
	const JSON_Object* prefab = json_object(prefab_v);
	// (prefab, )
	uint64_t num_entities = json_object_get_u64(prefab, "num_entities");
	entities.reserve(entities.size() + num_entities);
	const JSON_Array* entity_arr = json_object_get_array(prefab, "entities");
	for (int i = 0; i < num_entities; ++i) {
		const JSON_Object* e_obj = json_array_get_object(entity_arr, i);
		Entity e;
		e.id = json_object_get_u64(e_obj, "id");
		e.parent = json_object_get_u64(e_obj, "pid");
		memcpy(e.name, json_object_get_string(e_obj, "name"), json_object_get_u64(e_obj, "name_len"));
		
		e.children.reserve(json_object_get_u64(e_obj, "num_children"));
		//const JSON_Array* child_arr = json_object_get_array(e_obj, "children");		
		//for (int j = 0; j < e.children.size(); ++j) {
		//	e.children[j] = json_array_get_u64(child_arr, j);
		//}

		e.components.resize(json_object_get_u64(e_obj, "num_components"));
		const JSON_Array* comp_arr = json_object_get_array(e_obj, "components");
		for (int j = 0; j < e.components.size(); ++j) {
			ComponentID& c = e.components[j];

			c.ctype = (ComponentTypes)json_array_get_u64(comp_arr, j * 2);
			c.id = json_array_get_u64(comp_arr, j * 2 + 1);
		}

		AddEntityCopy(e);
	}
	
	// Here systems should always be already initialized...
	uint64_t num_systems = json_object_get_u64(prefab, "num_systems");
	const JSON_Array* systems = json_object_get_array(prefab, "systems");
	for (int i = 0; systems != nullptr && i < num_systems; ++i) {
		const JSON_Object* sys_obj = json_array_get_object(systems, i);
		std::vector<ComponentTypes> ctype; 
		ctype.resize(json_object_get_u64(sys_obj, "num_component_types"));
		const JSON_Array* ctypes_arr = json_object_get_array(sys_obj, "component_types");
		for (int j = 0; j < ctype.size(); ++j)
			ctype[j] = (ComponentTypes)json_array_get_u64(ctypes_arr, j);

		System* curr_sys = GetSystemOfTypes(ctype);
		if (curr_sys != nullptr) {
			curr_sys->JSONDeserializeComponents(json_object_get_object(sys_obj, "system_data"));
		}
	}
	

}

JSON_Value* ModuleECS::SerializePrefab(const uint64_t eid) {
	JSON_Value* ret = json_value_init_object();
	JSON_Value* entity_arr_v = json_value_init_array();
	JSON_Array* entity_arr = json_array(entity_arr_v);
	for (const Entity& e : entities) {
		JSON_Value* entity_v = json_value_init_object();
		JSON_Object* entity_o = json_object(entity_v);
		// Entity info
		json_object_set_u64(entity_o, "id", e.id);
		json_object_set_u64(entity_o, "pid", e.parent);
		json_object_set_u64(entity_o, "name_len", strlen(e.name));
		json_object_set_string(entity_o, "name", e.name);
		
		// Components
		json_object_set_u64(entity_o, "num_components", e.components.size());
		JSON_Value* comp_val = json_value_init_array();
		JSON_Array* comp_arr = json_array(comp_val);
		for (const ComponentID& cid : e.components) {		
			json_array_append_u64(comp_arr, cid.ctype);
			json_array_append_u64(comp_arr, cid.id);
		}
		json_object_set_value(entity_o, "components", comp_val);

		// Children
		json_object_set_u64(entity_o, "num_children", e.children.size());
		JSON_Value* childs_val = json_value_init_array();
		JSON_Array* childs_arr = json_array(childs_val);
		for (const uint64_t& child_id : e.children) {
			json_array_append_u64(childs_arr, child_id);
		}
		json_object_set_value(entity_o, "children", childs_val);

		// Append to Entity Array
		json_array_append_value(entity_arr, entity_v);
	}
	// Add the number of entities and entities to JSON file
	json_object_set_u64(json_object(ret), "num_entities", entities.size());
	json_object_set_value(json_object(ret), "entities", entity_arr_v);

	// Add systems
	json_object_set_u64(json_object(ret), "num_systems", systems.size());
	JSON_Value* sys_arr_val = json_value_init_array();
	JSON_Array* sys_arr = json_array(sys_arr_val);
	std::vector<JSON_Value*> sys_values;
	for (System* sys : systems) {
		JSON_Value* curr_sys = json_value_init_object();
		JSON_Object* sys_obj = json_object(curr_sys);

		// Number of component types the system handles
		const std::vector<ComponentTypes> ctypes = sys->GetVecCTYPES();
		json_object_set_u64(sys_obj, "num_component_types", ctypes.size());
		JSON_Value* ctypes_arr_v = json_value_init_array();
		JSON_Array* ctypes_arr = json_array(ctypes_arr_v);
		for (const ComponentTypes& ctype : ctypes)
			json_array_append_u64(ctypes_arr, ctype);
		json_object_set_value(sys_obj, "component_types", ctypes_arr_v);

		// Components: Object as it might want to save the internal components separately...
		JSON_Value* components_value = json_value_init_object();
		sys->JSONSerializeComponents(json_object(components_value));

		json_object_set_value(sys_obj, "system_data", components_value);

		sys_values.push_back(curr_sys);
	}
	
	for (JSON_Value* v : sys_values)
		json_array_append_value(sys_arr, v);

	json_object_set_value(json_object(ret), "systems", sys_arr_val);

	return ret;
}