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

	Component* AddC(const ComponentTypes ctype, const uint64_t eid) {
		transforms.push_back(C_Transform());
		C_Transform& t = transforms.back();
		t.id.parent_id = eid;
		t.id.quick_ref = transforms.size() - 1;

		//CreateTransformTree(t);

		return (Component*)&t;
	}

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

	void DeleteComponent(const ComponentID& cid) {
		ComponentID temp = cid;
		InvalidateChildren(cid.id);
		C_Transform* ctrans = (C_Transform*)GetC(temp);
		*ctrans = transforms[transforms.size() - 1];
		transforms.pop_back();
	}

	Component* GetCByRef(const ComponentID& cid) {
		Component* ret = nullptr;
		if (cid.quick_ref > transforms.size() - 1 || transforms[cid.quick_ref].id.id != cid.id) 
			return nullptr;

		return (Component*)&transforms[cid.quick_ref];
	}
	Component* GetC(ComponentID& cid) {
		Component* ret = nullptr;
		for(int i = 0; i < transforms.size(); ++i)
			if (transforms[i].id.id == cid.id) {
				cid.quick_ref = i;
				return &transforms[i];
			}
		return ret;
	}

	void CreateTransformTree(C_Transform& t) {
		Entity* e = App->ecs->GetEntity(t.id.parent_id);
		t.world_mat = float4x4::identity;
		std::vector<float4x4> cache;
		uint64_t next_pid = e->parent;
		while ((e = App->ecs->GetEntity(next_pid)) != nullptr) {
			const C_Transform* cid = (C_Transform*)GetComponent(e->GetComponent(CT_Transform));
			if (cid != nullptr) {
				t.transform_tree.push_back(cid->id);
				cache.push_back(cid->local_mat);
				next_pid = e->parent;
			}
		}
		
		t.world_mat = float4x4::identity;
		for (int i = 0; i < cache.size(); ++i)
			t.world_mat = cache[i].Mul(t.world_mat);
		
		t.valid_tree = true;
	}

	update_status PreUpdate(float dt) {
		std::vector<float4x4> cache;
		for (C_Transform& t : transforms) {
			if (t.is_static) continue;
			if (!t.valid_tree) {
				CreateTransformTree(t);
				continue;
			}

			cache.clear();
			C_Transform* curr = nullptr;
			for (ComponentID ref : t.transform_tree) {
				curr = (C_Transform*)GetComponent(ref);
				if (curr == nullptr) to_delete.push_back(ref);
				else cache.push_back(curr->local_mat);
			}

			t.world_mat = float4x4::identity;
			for (int i = 0; i < cache.size(); ++i)
				t.world_mat = cache[i].Mul(t.world_mat);
		}


		return UPDATE_CONTINUE;
	}
};