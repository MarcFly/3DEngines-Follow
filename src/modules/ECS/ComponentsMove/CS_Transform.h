#pragma once

#include "../ModuleECS.h"
#include <src/helpers/MathGeoLib/MathGeoLib.h>
#include <src/Application.h>

struct C_Transform : public Component {
	C_Transform() { id.ctype = CT_Transform; id.id = PCGRand(); }
	bool is_static = false;
	float4x4 world_mat = float4x4::identity;
	float4x4 local_mat = float4x4::identity;

	float3 pos;
	float3 scale;
	Quat rot;

	std::vector<ComponentID> transform_tree;
	bool valid_tree = false;

	void DrawInspector();
};

struct S_Transform : public System {
	S_Transform() : System(CT_Transform) {}
	std::vector<C_Transform> transforms;
	std::vector<ComponentID> to_delete;

	void JSONSerializeComponents(JSON_Object* sys_obj);
	void JSONDeserializeComponents(const JSON_Object* sys_obj);

	Component* AddC(const ComponentTypes ctype, const uint64_t eid);

	void AddToDeleteQ(const ComponentID& cid) { 
		to_delete.push_back(cid); 
	}

	inline void InvalidateChildren(const uint64_t invalid_id) {
		for (C_Transform& t : transforms) {
			for (ComponentID& id : t.transform_tree)
				if (id.id == invalid_id){
					t.valid_tree = false;
					break;
				}
		}
	}

	void DeleteComponent(const ComponentID& cid);
	Component* GetCByRef(const ComponentID& cid);

	Component* GetC(ComponentID& cid);

	void CreateTransformTree(C_Transform& t);

	update_status PreUpdate(float dt);
};