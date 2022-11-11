#pragma once

#include "../Globals.h"
#include <parson.h>
#include "Events/Events.h"
#include "Core/Layer.h"
#include "Component.h"
#include "Entity.h"

#define MISSING_FUN(returnv) Engine_WARN("Generic System Function is not defined..."); return returnv
namespace Engine {

	struct System {
		uint64_t type = UINT64_MAX;

		// Each System must at least hold 1 complete component type, might have references to other cached components too
		// Every new component a system brings, it MUST REGISTER IT to the ECS!
		// Ex: SystemTransforms() { ComponenTransform::Register(); System(type); }
#define SystemConstruct(SystemName, ComponentName) static uint64_t type;\
		SystemName() { ComponentName::Register(); type = ComponentName::type;} \
		static constexpr const char* name = #ComponentName; \
		const char* GetName() { return SystemName##::name; }
		virtual const char* GetName() { return "noname"; }
// TODO: Macro that defines the generic functions using the template given
		virtual ~System() {};

		virtual void Init() {}
		virtual void Start() {}
		virtual void PreUpdate(float dt) {};
		virtual void Update(float dt) {};
		virtual void PostUpdate(float dt) {};
		virtual void CleanUp() {};

		virtual Component* CreateComponent(const uint64_t type) { return nullptr; }

		// TODO: update data struct to use slotmaps!
		template<typename T>
		using data_struct = std::vector<T>;

		virtual void OnAdd(const CID& cid) {};

		virtual CID AddGeneric(const uint64_t eid) { MISSING_FUN(CID(UINT64_MAX)); }
		template<typename ComponentType>
		static CID Add(data_struct<ComponentType>& comps, const uint64_t entity_id) {
			comps.push_back(ComponentType());
			ComponentType& c = comps.back();
			c.cid.parent_id = entity_id;
			c.cid.quick_ref = comps.size() - 1;
			c.cid.ctype = ComponentType::type;

			return c.cid;
		}

		virtual Component* GetGeneric(CID& cid) { MISSING_FUN(nullptr); }
		template<typename CT>
		static CT& Get(data_struct<CT>& comps, CID& cid) {
			if (cid.quick_ref < comps.size() && cid.id == comps[cid.quick_ref].cid.id)
				return comps[cid.quick_ref];
			for (int i = 0; i < comps.size(); ++i)
				if (cid.id == comps[i].cid.id) {
					cid.quick_ref = i;
					return comps[i];
				}
			Engine_WARN("Could not find the component: {}:{}", CT::GetName(), cid.id);
			return CT::invalid;
		}

		template<typename CT>
		static const CT& GetConst(data_struct<CT>& comps, CID& cid) {
			return Get(comps, cid);
		}

		virtual CID AddCopyGeneric(CID& copy_component_cid, const uint64_t entity_id) { MISSING_FUN(CID(UINT64_MAX)); }
		template<typename CT>
		static CID AddCopy(data_struct<CT>& comps, CID& copy_component_cid, const uint64_t entity_id) {
			const CT& copy = GetConst<CT>(comps, copy_component_cid);
			comps.push_back(CT(copy));
			CT& c = comps.back();
			c.cid.parent_id = entity_id;
			c.cid.quick_ref = comps.size() - 1;
			c.cid.ctype = CT::type;

			return c.cid;
		}

		virtual CID AddCopyOfGeneric(const Component* copy_component, const uint64_t entity_id) { MISSING_FUN(CID(UINT64_MAX)); }
		template<typename CT>
		static CID AddCopyOf(data_struct<CT>& comps, const CT& copy_component, const uint64_t entity_id) {
			comps.push_back(CT(copy_component));
			CT& c = comps.back();
			c.cid.parent_id = entity_id;
			c.cid.quick_ref = comps.size() - 1;
			c.cid.ctype = CT::type;

			return c.cid;
		}

		template<typename CT>
		std::vector<CT> GetEntityComponents(data_struct<CT>& comps, const uint64_t type, const uint64_t entity_id) {
			// TODO: Get all components of a type from an entity
			return std::vector<CT>();
		}

		// Not in the same function as in slotmaps this will not be an issue
		virtual void OnDelete(const CID& cid) { MISSING_FUN(); }
		virtual void OnMoved(const CID& cid) { MISSING_FUN(); }

		virtual void DeleteGeneric(const CID& cid) {}
		template<typename CT>
		static void DeleteComponent(data_struct<CT>& comps, const CID& cid) {
			for (int i = 0; i < comps.size(); ++i) {
				if (comps[i].cid.id == cid.id) {
					comps[i] = comps.back();
					return;
				}
			}
			Engine_WARN("Could not find the component, in process of deleting: {}:{}", CT::GetName(), cid.id);
		}

		virtual void JSONSerializeComponents(JSON_Object* sys_obj) {}
		virtual void JSONDeserializeComponents(const JSON_Object* sys_obj) {}
		virtual void SerializeSingleComponent(const CID& cid, JSON_Array* component_arr) {};
	};

	struct ECS : public Layer, public FileTaker {
		ECS();
		~ECS();

		char scenename[256];

		// TODO: SWAP TO SLOTMAP!!!
		std::vector<Entity> entities;
		std::vector<uint64_t> base_entities;
		std::unordered_map<uint64_t, System*> systems;

		// TODO: static vector of system* that have been registered
		// So that when custom systems/components are set, they can be used from the converter and such...

		void OnAttach();
		void OnDetach();

		void PreUpdate();
		void Update();
		void PostUpdate();

		void CleanUp();

		Entity* AddEntity(const uint64_t _parent_id);
		Entity* AddCopyEntity(const Entity& entity_copy, const uint64_t parent_id);

		Entity* GetEntity(const uint64_t entity_id);
		const Entity* GetEntityConst(const uint64_t entity_id) const;
		void DeleteEntity(const uint64_t entity_id);

		template<typename SystemType>
		void RegisterSystem() {
			SystemType* new_system = new SystemType();
			if (GetSystemOfType(SystemType::type) == nullptr)
				systems.insert(std::pair<uint64_t,System*>(new_system->type, (System*)new_system));
		}

		inline System* GetSystemOfType(const uint64_t type);

		// System and Component should alwas match, so not an issue
		template<typename Type>
		inline System* GetSystem() {
			return GetSystemOfType(Type::type);
		}

		CID AddComponentGeneric(const uint64_t type, const uint64_t entity_id);
		template<typename SystemType>
		CID AddComponent(const uint64_t entity_id) { 
			if (GetEntity(entity_id) == nullptr) return CID(UINT64_MAX);
			SystemType* sys = (SystemType*)GetSystemOfType(SystemType::type);
			CID ret = System::Add(sys->components, entity_id);
			sys->OnAdd(ret);
			return ret; 
		}

		CID AddCopyComponentGeneric(CID& cid, const uint64_t entity_id);
		template<typename ST>
		CID AddCopyComponent(CID& cid, const uint64_t entity_id) {
			if (GetEntity(entity_id) == nullptr) return CID(UINT64_MAX);
			ST* sys = (ST*)GetSystemOfType(ST::type);
			CID ret = System::AddCopy(sys->components, cid, entity_id);
			sys->OnAdd(ret);
			return ret;
		}
		CID AddCopyOfComponentGeneric(const Component* component, const uint64_t entity_id);
		template<typename ST, typename ComponentType>
		CID AddCopyOfComponent(const ComponentType& copy_component, const uint64_t entity_id) {
			if (GetEntity(entity_id) == nullptr) return CID(UINT64_MAX);
			ST* sys = (ST*)GetSystemOfType(ST::type);
			CID ret = System::AddCopyOf(sys->components, copy_component, entity_id);
			sys->OnAdd(ret);
			return ret;
		}
		
		inline Component* GetComponentGeneric(CID& cid);
		template<typename CT>
		inline CT& GetComponent(CID& cid) { return (CT&)*GetComponentGeneric(cid); }
		template<typename ST, typename CT>
		CT& GetComponent(CID& cid) {
			ST* sys = (ST*)GetSystemOfType(ST::type);
			return System::Get(sys->components, cid)
		}


		std::vector<Component*> GetEntityComponents(const uint64_t eid, const uint64_t type);

		void DeleteComponent(const CID& cid);

		JSON_Value* SerializeScene();
		void SerializePrefab(const uint64_t entity_id, JSON_Value* value);
		void DeserializePrefab(const JSON_Value* json_value);

		DECL_DYN_ENGINE_EV_FUNS(AddEntity_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(DeleteEntity_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(ModifyEntity_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(AddComponent_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(DeleteComponent_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(LOADJSONPrefab_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(RequestEntities_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(RequestSingleEntity_EventFun);
		DECL_DYN_ENGINE_EV_FUNS(RequestEntityComponents_EventFun);
	};
};

#define DECL_SystemGenericFuns(SystemType, ComponentType) CID AddGeneric(const uint64_t eid); \
	Component* GetGeneric(CID& cid); \
	CID AddCopyGeneric(CID& copy_component_cid, const uint64_t entity_id); \
	CID AddCopyOfGeneric(const Component* copy_component, const uint64_t entity_id); \
	void DeleteGeneric(const CID& cid);

#define DEF_SystemGenericFuns(SystemType, ComponentType) CID SystemType::AddGeneric(const uint64_t eid) { return System::Add(components, eid); } \
	Component* SystemType::GetGeneric(CID& cid) { return (Component*)&System::Get(components, cid); } \
	CID SystemType::AddCopyGeneric(CID & copy_component_cid, const uint64_t entity_id) {return System::AddCopy(components, copy_component_cid, entity_id); } \
	CID SystemType::AddCopyOfGeneric(const Component* copy_component, const uint64_t entity_id) { return System::AddCopyOf(components, *(ComponentType*)copy_component, entity_id); } \
	void SystemType::DeleteGeneric(const CID& cid) { System::DeleteComponent(components, cid); }