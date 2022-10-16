#pragma once
#include <src/helpers/Globals.h>
#include <src/modules/Module.h>
#include <vector>

enum ComponentTypes {
	CT_MeshRenderer,
	CT_Transform,

	CT_MAX
};

struct ComponentID {
	ComponentTypes ctype;
	uint64_t id = PCGRand();
	uint32_t parent_id = UINT32_MAX;
	uint32_t quick_ref = UINT32_MAX;
};

struct Component {
	ComponentID id;
	uint32_t version = 0;
};

// Entities can modify components
static uint64_t default_name = 0;
struct Entity {
	bool active = true;
	uint64_t id = PCGRand();
	uint64_t parent = UINT64_MAX;
	std::vector<ComponentID> components;
	std::vector<uint64_t> children;
	char name[32] = "";
	// If you want to cash them, create them per entity
};

// Systems don't care about init and close time
// Only Runtime
struct System {
	ComponentTypes ctype;

	System() {};
	virtual ~System() {}; // Destroy all the components held in the system!

	virtual bool Init() { return true; }
	virtual bool Start() { return true; }
	virtual update_status PreUpdate(float dt) { return UPDATE_CONTINUE; }
	virtual update_status Update(float dt) { return UPDATE_CONTINUE; }
	virtual update_status PostUpdate(float dt) {return UPDATE_CONTINUE;}
	virtual bool CleanUp() { return true; }

	// Getters and Setters
	virtual Component* AddC(const ComponentTypes ctype, const uint64_t eid) { return nullptr; }
	// Allow ComponentID to be ref, so to update quick_ref overtime (slotmap would solve this)
	virtual Component* GetCQuick(const uint32_t quick_ref) { return nullptr; } // Try and go directly with the id, check out of bounds
	virtual Component* GetC(ComponentID& ctype) { return nullptr; }
	virtual std::vector<Component*> GetCs(const ComponentTypes ctype) { return std::vector<Component*>(); }
	virtual std::vector<Component*> GetCsFromEntity(const uint64_t eid, const ComponentTypes ctype) { return std::vector<Component*>(); }
};

class ModuleECS : public Module {
public:
	ModuleECS();
	~ModuleECS();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);
	bool CleanUp();


public:
	void AddEntity(const uint64_t par_id) {
		entities.push_back(new Entity());
		entities.back()->parent = par_id;
		std::string temp = std::to_string(default_name++);
		memcpy(entities.back()->name, temp.c_str(), temp.length());

		if (par_id == UINT64_MAX)
			base_entities.push_back(entities.back()->id);
		else
			GetEntity(par_id)->children.push_back(entities.back()->id);
	}

	void DeleteEntity(const uint64_t eid) {
		int i;
		for (i = 0; i < entities.size(); ++i) {
			if (entities[i]->id == eid) {
				for (auto cid : entities[i]->children) {
					DeleteEntity(cid);
				}
				// TODO: Delete Components

				// Delete From Base Entities before deleting entity
				if (entities[i]->parent == UINT64_MAX) {
					std::vector<uint64_t> swapvec;
					swapvec.reserve(base_entities.size());
					for (int j = 0; j < base_entities.size(); j++) {
						if (base_entities[j] == eid) continue;
						swapvec.push_back(base_entities[j]);
					}
					base_entities.swap(swapvec);
				}

				Entity* pe = GetEntity(entities[i]->parent);
				if (pe != nullptr) 
				{
					std::vector<uint64_t> swapvec;
					swapvec.reserve(pe->children.size());
					for (int j = 0; j < pe->children.size(); j++) {
						if (pe->children[j] == eid) continue;
						swapvec.push_back(pe->children[j]);
					}
					pe->children.swap(swapvec);
				}

				entities[i] = entities.back();
				entities.pop_back();
				return;
				
			}
		}
	}

	Entity* GetEntity(const uint64_t eid) {
		for (auto e : entities) if (e->id == eid) return e;
		return nullptr;
	}

	inline System* GetSystemOfType(const ComponentTypes ctype) { for (auto it : systems) { if (it->ctype == ctype) return it; } return nullptr; }

	template<class T>
	T* AddComponent(const uint64_t eid, const ComponentTypes ctype) {
		System* GetSystemOfType(ctype);
		return T*(system->AddC(ctype, eid));
	}

	inline Component* GetComponentGeneric(ComponentID& ctype) {
		System* sys = GetSystemOfType(ctype.ctype);
		Component* ret = sys->GetCQuick(ctype.quick_ref);
		if (ret == nullptr || ret->id.id != ctype.id)
			ret = sys->GetC(ctype);
		return ret;
	}

	template<class T>
	inline T* GetComponent(ComponentID& ctype) {
		return T * (GetComponentGeneric(ctype));
	}
	
	template<class T>
	std::vector<T*> GetComponentFromEntity(const uint64_t eid, const ComponentTypes ctype) {
		Entity* et = nullptr;
		for (auto e : entities) { if (eid == e->id) { et = e; break; } }
		if (et == nullptr)
			return ret;

		System* sys = GetSystemOfType(ctype);
		return sys->GetCsFromEntity(eid, ctype);
	}

public:
	std::vector<Entity*> entities;
	std::vector<uint64_t> base_entities;
	std::vector<System*> systems;


};