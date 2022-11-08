#pragma once
#include <src/helpers/Globals.h>
#include <src/modules/Module.h>
#include <vector>
#include <string>
#include <src/helpers/JSON/parson.h>

enum ComponentTypes {
	CT_MeshRenderer,
	CT_Transform,

	CT_MAX
};

struct Entity;
struct ComponentID {
	ComponentTypes ctype;
	uint64_t id = PCGRand();
	Entity* parent = nullptr;
};

struct Component {
	ComponentID id;
	bool active = true;

	virtual void Init() {};
	virtual void Start() {};
	virtual void PreUpdate(float dt) {};
	virtual void Update(float dt) {};
	virtual void PostUpdate(float dt) {};
	virtual void DrawInspector() {};
	virtual void CleanUp() {};

	virtual void SerializeJSON(JSON_Object* obj) const {};
};

// Entities can modify components
static uint64_t default_name = 0;
struct Entity {
	bool active = true;
	bool isstatic = true;
	uint64_t id = PCGRand();
	Entity* parent = nullptr;
	uint64_t par_id = UINT64_MAX;
	char name[32] = "";
	// If you want to cash them, create them per entity

	std::vector<Entity*> children;
	std::vector<Component*> components;
	
	void Init() {
		for (int i = 0; i < components.size(); ++i)
			components[i]->Init();

		for (int i = 0; i < children.size(); ++i)
			children[i]->Init();
	}

	void Start() {
		for (int i = 0; i < components.size(); ++i)
			components[i]->Start();

		for (int i = 0; i < children.size(); ++i)
			children[i]->Start();
	}

	void PreUpdate(float dt) {
		for (int i = 0; i < components.size(); ++i)
			components[i]->PreUpdate(dt);

		for (int i = 0; i < children.size(); ++i)
			children[i]->PreUpdate(dt);
	}

	void Update(float dt) {
		for (int i = 0; i < components.size(); ++i)
			components[i]->Update(dt);

		for (int i = 0; i < children.size(); ++i)
			children[i]->Update(dt);
	}

	void PostUpdate(float dt) {
		for (int i = 0; i < components.size(); ++i)
			components[i]->PostUpdate(dt);

		for (int i = 0; i < children.size(); ++i)
			children[i]->PostUpdate(dt);

	}

	void CleanUp() {
		for (int i = 0; i < components.size(); ++i)
			components[i]->CleanUp();

		for (int i = 0; i < children.size(); ++i)
			children[i]->CleanUp();
	}

	~Entity() {
		for (int i = 0; i < children.size(); ++i)
			delete children[i];
		for (int i = 0; i < components.size(); ++i)
			delete components[i];
	}

	template<typename ComponentType>
	ComponentType* AddComponent(ComponentType* copy = nullptr) {
		components.push_back((Component*) new ComponentType());
		ComponentType* curr = (ComponentType*)components.back();
		if (copy != nullptr) *curr = *copy;
		curr->id.parent = this;
		return curr;
	}
	
	template<typename ComponentType>
	ComponentType* GetComponent() {
		for (int i = 0; i < components.size(); ++i)
			if (components[i]->id.ctype == ComponentType::type)
				return (ComponentType*)components[i];
		return nullptr;
	}

	void DeserializePrefab(const JSON_Object* obj) const;
	void SerializePrefab(JSON_Object* obj) const;
};

class ModuleECS : public Module {
public:
	Entity root;
	std::vector<Entity*> entities;

	ModuleECS();
	~ModuleECS();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();


public:

	void ReserveEntities(const uint64_t num_reserve) { entities.reserve(entities.capacity() + num_reserve); }

	Entity* AddEntity(const uint64_t par_id = UINT64_MAX) {
		entities.push_back(new Entity());
		Entity* entity = entities.back();
		Entity* parent = GetEntity(par_id);
		entity->parent = parent;
		std::string temp = std::to_string(default_name++);
		memcpy(entity->name, temp.c_str(), temp.length());

		if (parent == nullptr)
			root.children.push_back(entity);
		else {
			parent->children.push_back(entity);
		}
		return entity;
	}

	Entity* AddEntityCopy(const Entity e) {
		Entity* entity = AddEntity((e.parent == nullptr) ? UINT64_MAX : e.parent->id);
		*entity = e;
		// TODO: Copy all components to the new entity;
		return entity;
	}
	
	void DeleteEntity(const uint64_t eid) {
		int i;
		for (i = 0; i < entities.size(); ++i) {
			if (entities[i]->id == eid) {
				Entity* e = entities[i];

				for (Entity* child : e->children)
					DeleteEntity(child->id);

				e->children.clear();	

				entities[i] = entities.back();
				entities.pop_back();

				delete e;
				return;
				
			}
		}
	}

	Entity* GetEntity(const uint64_t eid) {
		for (auto& e : entities) if (e->id == eid) return e;
		return nullptr;
	}

	JSON_Value* SerializePrefab(const uint64_t eid = UINT64_MAX);
	void DeserializePrefab(const JSON_Value* value);


public:
	


};