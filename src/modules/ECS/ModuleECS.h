#pragma once
#include <src/helpers/Globals.h>
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

struct Entity {
	uint64_t id = PCGRand();
	std::vector<ComponentID> components;
	// If you want to cash them, create them per entity
};

// Systems don't care about init and close time
// Only Runtime
struct System {
	ComponentTypes ctype;

	System() {};
	virtual ~System() {}; // Destroy all the components held in the system!

	virtual update_status PreUpdate(float dt) { return UPDATE_CONTINUE; }
	virtual update_status Update(float dt) { return UPDATE_CONTINUE; }
	virtual update_status PostUpdate(float dt) {return UPDATE_CONTINUE;}
	
	virtual Component* AddC(const ComponentTypes ctype, const uint64_t eid) { return nullptr; }
	// Allow ComponentID to be ref, so to update quick_ref overtime (slotmap would solve this)
	virtual Component* GetCQuick(const uint32_t quick_ref) { return nullptr; } // Try and go directly with the id, check out of bounds
	virtual Component* GetC(ComponentID& ctype) { return nullptr; }
	virtual std::vector<Component*> GetCs(const ComponentTypes ctype) { return std::vector<Component*>(); }
	virtual std::vector<Component*> GetCsFromEntity(const uint64_t eid, const ComponentTypes ctype) { return std::vector<Component*>(); }
};

class ModuleECS : public Module {
public:
	inline System* GetSystemOfType(const ComponentTypes ctype) { for (auto it : systems) { if (it->ctype == ctype) return it; } return nullptr; }

	template<class T>
	T* AddComponent(const uint64_t eid, const ComponentTypes ctype) {
		System* GetSystemOfType(ctype);
		return T*(system->AddC(ctype, eid));
	}

	template<class T>
	T* GetComponent(ComponentID& ctype) {
		System* sys = GetSystemOfType(ctype.ctype);
		Component* ret = sys->GetCQuick(ctype.quick_ref);
		if (ret == nullptr || ret->id.id != ctype.id)
			ret = sys->GetC(ctype);

		return T * (ret);
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
private:

	std::vector<Entity*> entities;
	std::vector<uint32_t> base_entities;
	std::vector<System*> systems;
};