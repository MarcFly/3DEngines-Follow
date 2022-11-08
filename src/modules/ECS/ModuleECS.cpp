#include "ModuleECS.h"
#include "ComponentsIncludeAll.h"
#include <src/helpers/Timer/sque_timer.h>

ModuleECS::ModuleECS() : Module("ECS") {
	InitTimer();
	CalibrateTimer();
}

ModuleECS::~ModuleECS() {
	
}


bool ModuleECS::Init()
{


	return true;
}

bool ModuleECS::Start()
{

	return true;
}

update_status ModuleECS::PreUpdate(float dt)
{
	root.PreUpdate(dt);

	return UPDATE_CONTINUE;
}

update_status ModuleECS::Update(float dt)
{
	root.Update(dt);

	return UPDATE_CONTINUE;
}

update_status ModuleECS::PostUpdate(float dt)
{
	root.PostUpdate(dt);
	return UPDATE_CONTINUE;
}

bool ModuleECS::CleanUp()
{
	while (entities.size() > 0)
		DeleteEntity(entities[0]->id);
	entities.clear();
	root.children.clear();

	return true;
}

void ModuleECS::DeserializePrefab(const JSON_Value* value) {
	if (value == nullptr) return;
	const JSON_Object* prefab = json_object(value);
	// (prefab, )

	uint64_t num_entities = json_object_get_u64(prefab, "num_entities");
	entities.reserve(entities.size() + num_entities);
	const JSON_Array* entity_arr = json_object_get_array(prefab, "entities");

	std::vector<Entity*> temp;
	for (int i = 0; i < num_entities; ++i) {
		const JSON_Object* e_obj = json_array_get_object(entity_arr, i);
		
		Entity* e = AddEntity();
		temp.push_back(e);
		e->id = json_object_get_u64(e_obj, "id");
		e->par_id = json_object_get_u64(e_obj, "pid");
		memcpy(e->name, json_object_get_string(e_obj, "name"), json_object_get_u64(e_obj, "name_len"));
		
		// TODO: Load components from the scene file
	}

	for (int i = 0; i < temp.size(); ++i) {
		Entity* par = GetEntity(temp[i]->par_id);
		if (par != nullptr) par->children.push_back(temp[i]);
		temp[i]->parent = par;
	}
}

void Entity::SerializePrefab(JSON_Object* obj) const {
	json_object_set_u64(obj, "id", id);
	json_object_set_u64(obj, "pid", par_id);
	json_object_set_u64(obj, "name_len", strlen(name));
	json_object_set_string(obj, "name", name);

	json_object_set_boolean(obj, "active", active);
	json_object_set_boolean(obj, "isstatic", isstatic);

	JSON_Value* comp_arr_v = json_value_init_array();
	JSON_Array* comp_arr = json_array(comp_arr_v);
	for (const Component* c : components) {
		JSON_Value* comp_v = json_value_init_object();
		JSON_Object* comp_obj = json_object(comp_v);
		c->SerializeJSON(comp_obj);
		json_array_append_value(comp_arr, comp_v);
	}
}

JSON_Value* ModuleECS::SerializePrefab(const uint64_t eid) {
	JSON_Value* ret = json_value_init_object();
	JSON_Value* entity_arr_v = json_value_init_array();
	JSON_Array* entity_arr = json_array(entity_arr_v);
	
	Entity* e = GetEntity(eid);
	if (e == nullptr) e = &root;
	JSON_Value* entity_v = json_value_init_object();
	JSON_Object* entity_o = json_object(entity_v);
	json_object_set_string(entity_o, "name", e->name);

	for (const Entity* e : e->children) {
		JSON_Value* entity_v2 = json_value_init_object();
		JSON_Object* entity_o2 = json_object(entity_v2);
		e->SerializePrefab(entity_o2);
		json_array_append_value(entity_arr, entity_v2);
	}

	return ret;
}