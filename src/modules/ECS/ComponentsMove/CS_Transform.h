#pragma once

#include "../ModuleECS.h"
#include <src/helpers/MathGeoLib/MathGeoLib.h>
#include <src/Application.h>

struct C_Transform : public Component {
	C_Transform() { id.ctype = CT_Transform; id.id = PCGRand(); }
	float4x4 world_mat;
	float4x4 local_mat;

	float3 pos;
	float3 scale;
	Quat rot;

	std::vector<uint32_t> transform_tree;
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

		CreateTransformTree(t);

		return (Component*)&t;
	}

	void AddToDeleteQ(const ComponentID& cid) { to_delete.push_back(cid); }

	void DeleteComponent(const ComponentID& cid) {
		ComponentID temp = cid;
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
		Entity* e = nullptr;
		t.world_mat = float4x4::identity;
		std::vector<float4x4> cache;
		while ((e = App->ecs->GetEntity(t.id.parent_id)) != nullptr) {
			const C_Transform* cid = (C_Transform*)GetComponent(e->GetComponent(CT_Transform));
			if (cid != nullptr) { 
				t.transform_tree.push_back(cid->id.quick_ref);
				cache.push_back(cid->local_mat);
			}
		}
		if (cache.size() != 0) {
			t.world_mat = cache[cache.size() - 1];
			for (int i = cache.size() - 2; i > -1; --i)
				t.world_mat = t.world_mat * cache[i];
			t.world_mat = t.world_mat * t.local_mat;
		}
		t.valid_tree = true;
	}

	update_status PreUpdate(float dt) {
		std::vector<float4x4> cache;
		for (C_Transform& t : transforms) {
			cache.clear();
			for (uint32_t ref : t.transform_tree) cache.push_back(transforms[ref].local_mat);

			while (cache.size() > 0) {
				t.world_mat = cache[cache.size() - 1];
				for (int i = cache.size() - 2; i > -1; --i)
					t.world_mat = t.world_mat * cache[i];
				t.world_mat = t.world_mat * t.local_mat;
			}
		}


		return UPDATE_CONTINUE;
	}
};