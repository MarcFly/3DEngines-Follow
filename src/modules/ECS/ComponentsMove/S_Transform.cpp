#include "CS_Transform.h"

Component* S_Transform::AddC(const ComponentTypes ctype, const uint64_t eid) {
	transforms.push_back(C_Transform());
	C_Transform& t = transforms.back();
	t.id.parent_id = eid;
	t.id.quick_ref = transforms.size() - 1;

	//CreateTransformTree(t);

	return (Component*)&t;
}

void S_Transform::DeleteComponent(const ComponentID& cid) {
	ComponentID temp = cid;
	InvalidateChildren(cid.id);
	C_Transform* ctrans = (C_Transform*)GetC(temp);
	*ctrans = transforms[transforms.size() - 1];
	transforms.pop_back();
}

Component* S_Transform::GetCByRef(const ComponentID& cid) {
	Component* ret = nullptr;
	if (cid.quick_ref > transforms.size() - 1 || transforms[cid.quick_ref].id.id != cid.id)
		return nullptr;

	return (Component*)&transforms[cid.quick_ref];
}

Component* S_Transform::GetC(ComponentID& cid) {
	Component* ret = nullptr;
	for (int i = 0; i < transforms.size(); ++i)
		if (transforms[i].id.id == cid.id) {
			cid.quick_ref = i;
			return &transforms[i];
		}
	return ret;
}

void S_Transform::CreateTransformTree(C_Transform& t) {
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

update_status S_Transform::PreUpdate(float dt) {
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

// =====================================================

void S_Transform::JSONSerializeComponents(JSON_Object* sys_obj) {
	json_object_set_string(sys_obj, "name", "Transform System");
	JSON_Value* trans_arr_val = json_value_init_array();
	JSON_Array* trans_arr = json_array(trans_arr_val);
	for (const C_Transform& t : transforms) {
		JSON_Value* t_val = json_value_init_object();
		JSON_Object* t_obj = json_object(t_val);
		json_object_set_u64(t_obj, "id", t.id.id);
		json_object_set_u64(t_obj, "pid", t.id.parent_id);
		// ctype is implied, quick ref cannot be bound

		// local_matrix -> All others can be calculated in runtime
		static char matrixnames[5];
		for (int i = 0; i < 16; ++i) {
			sprintf(matrixnames, "r%uc%u", i / 4, i - (i / 4) * 4);
			json_object_set_number(t_obj, matrixnames, t.local_mat.ptr()[i]);
		}
		json_array_append_value(trans_arr, t_val);
	}
	json_object_set_number(sys_obj, "num_transforms", transforms.size());
	json_object_set_value(sys_obj, "transforms", trans_arr_val);
}
void S_Transform::JSONDeserializeComponents(const JSON_Object* sys_obj) {
	const JSON_Array* trans_arr = json_object_get_array(sys_obj, "transforms");
	int num_transforms = json_object_get_number(sys_obj, "num_transforms");
	transforms.reserve(transforms.size() + num_transforms);

	for (int i = 0; i < num_transforms; ++i) {
		const JSON_Object* curr_t = json_array_get_object(trans_arr, i);
		transforms.push_back(C_Transform());
		C_Transform& t = transforms.back();
		t.id.id = json_object_get_u64(curr_t, "id");
		t.id.parent_id = json_object_get_u64(curr_t, "pid");

		t.local_mat.scaleX =	json_object_get_number(curr_t, "r0c0");
		t.local_mat.shearXy =	json_object_get_number(curr_t, "r0c1");
		t.local_mat.shearXz =	json_object_get_number(curr_t, "r0c2");
		t.local_mat.x =			json_object_get_number(curr_t, "r0c3");

		t.local_mat.shearYx =	json_object_get_number(curr_t, "r1c0");
		t.local_mat.scaleY =	json_object_get_number(curr_t, "r1c1");
		t.local_mat.shearYz =	json_object_get_number(curr_t, "r1c2");
		t.local_mat.y =			json_object_get_number(curr_t, "r1c3");

		t.local_mat.shearZx =	json_object_get_number(curr_t, "r2c0");
		t.local_mat.shearZy =	json_object_get_number(curr_t, "r2c1");
		t.local_mat.scaleZ =	json_object_get_number(curr_t, "r2c2");
		t.local_mat.z =			json_object_get_number(curr_t, "r2c3");

		t.local_mat.shearWx =	json_object_get_number(curr_t, "r3c0");
		t.local_mat.shearWy =	json_object_get_number(curr_t, "r3c1");
		t.local_mat.shearWz =	json_object_get_number(curr_t, "r3c2");
		t.local_mat.w =			json_object_get_number(curr_t, "r3c3");
	}
}