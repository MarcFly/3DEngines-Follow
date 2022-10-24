#include "CSMeshRenderer.h"

void S_MeshRenderer::TestInit() {
	// Cost Test
	renderers.resize(NUM_TEST_MESHES);
	send.transforms.resize(NUM_TEST_MESHES);
	for (int i = 0; i < NUM_TEST_MESHES; ++i) {
		C_MeshRenderer& curr = renderers[i];
		curr.cached_wm = i;
		send.transforms.emplace_back(float4x4::identity);
		// Worst case scenario in cpu and GPU then
		curr.gl_state = NUM_GL_STATES * (PCGRand() / (double)UINT64_MAX);// State per mesh
		curr.material = NUM_MATERIALS * (PCGRand() / (double)UINT64_MAX); // Material per mesh
		curr.mesh = PCGRand(); // All different meshes	
	}

	send.gl_state_groups.reserve(NUM_GL_STATES);
	for (int i = 0; i < send.gl_state_groups.size(); ++i) {
		send.gl_state_groups[i].materialgroups.reserve(NUM_MATERIALS);
	}
	CacheGroups();
}

void S_MeshRenderer::CacheGroups() {
	for (int i = 0; i < renderers.size(); ++i) {
		const C_Transform* get = App->ecs->GetComponent<C_Transform>(renderers[i].associated_transform);
		if (recache || renderers[i].cached_wm == UINT32_MAX) {
			send.transforms.emplace_back(get->world_mat * get->local_mat);
			renderers[i].cached_wm = send.transforms.size() - 1;
		}
		send.transforms[renderers[i].cached_wm] = send.transforms[renderers[i].cached_wm] * renderers[i].local;

		if (!recache && renderers[i].cached_group) continue;
		//if (renderers[i].cached_wm == UINT32_MAX) {
		//	// Get Transform through Parenting...
		//	float4x4 t;
		//	global_transform.emplace_back(t);
		//	renderers[i].cached_wm = global_transform.size() - 1;
		//}
		for (int j = 0; j < send.gl_state_groups.size(); ++j) {
			if (send.gl_state_groups[j].gl_state == renderers[i].gl_state) {
				SetMat(send.gl_state_groups[j], renderers[i]);
				continue;
			}
		}
		if (renderers[i].cached_group) continue;
		send.gl_state_groups.emplace_back(RenderGroup());
		RenderGroup& g = send.gl_state_groups.back();
		g.gl_state = renderers[i].gl_state;
		SetMat(g, renderers[i]);
	}
}

void S_MeshRenderer::SetMeshTransf(MaterialGroup& mg, C_MeshRenderer& cr) {
	for (int i = 0; i < mg.meshes.size(); ++i)
		if (mg.meshes[i] == cr.mesh) {
			mg.world_matrices[i] = cr.cached_wm;
			cr.cached_group = true;
			return;
		}

	mg.material = cr.material;
	mg.meshes.emplace_back(cr.mesh);
	// Get Transform through Parenting...
	//float4x4 t;
	//global_transform.emplace_back(t);
	mg.world_matrices.push_back(cr.cached_wm);
	cr.cached_group = true;
	//cr.cached_wm = global_transform.size() - 1;
}

void S_MeshRenderer::SetMat(RenderGroup& g, C_MeshRenderer& cr) {
	for (int i = 0; i < g.materialgroups.size(); ++i)
		if (g.materialgroups[i].material == cr.material) {
			SetMeshTransf(g.materialgroups[i], cr);
			return;
		}
	g.materialgroups.emplace_back(MaterialGroup());
	MaterialGroup& mg = g.materialgroups.back();
	mg.material = cr.material;
	//mg.meshes.emplace_back(cr.mesh);

	SetMeshTransf(mg, cr);
}

update_status S_MeshRenderer::PreUpdate(float dt) {
	// Recreate rendergroups each frame
	// Reasons: Invalidation of data, not ideal

	// TODO: Change this to be recreated only when there are changes		
	// Should not be called per frame, only push and pop operations!
	send.transforms.clear();
	recache = true;
	CacheGroups();

	return UPDATE_CONTINUE;
}


update_status S_MeshRenderer::Update(float dt) {
	EV_SEND_POINTER(ECS_RENDERABLES, (void*)&send);

	if (to_delete.size() > 0) {
		for (int i = 0; i < to_delete.size(); ++i)
			DeleteComponent(to_delete[i]);

		to_delete.clear();
	}
	return UPDATE_CONTINUE;
}

void S_MeshRenderer::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec) {
	for (std::shared_ptr<Event> ev : evt_vec) {
		switch (ev->type) {
			
		}
	}
}

//========================================================================================

void S_MeshRenderer::JSONSerializeComponents(JSON_Object* sys_obj) {
	json_object_set_string(sys_obj, "name", "MeshRenderer System");
	JSON_Value* mesh_arr_val = json_value_init_array();
	JSON_Array* mesh_arr = json_array(mesh_arr_val);

	for (const C_MeshRenderer& m : renderers) {
		JSON_Value* m_val = json_value_init_object();
		JSON_Object* m_obj = json_object(m_val);
		json_object_set_u64(m_obj, "id", m.id.id);
		json_object_set_u64(m_obj, "pid", m.id.parent_id);

		json_object_set_u64(m_obj, "transform_id", m.associated_transform.id);

		json_object_set_u64(m_obj, "gl_state", m.gl_state);
		json_object_set_u64(m_obj, "mesh_id", m.mesh);

		json_object_set_u64(m_obj, "material_id", m.material);
		
		static char matrixnames[5];
		for (int i = 0; i < 16; ++i) {
			sprintf(matrixnames, "r%uc%u", i / 4, i - (i / 4) * 4);
			json_object_set_number(m_obj, matrixnames, m.local.ptr()[i]);
		}

		json_array_append_value(mesh_arr, m_val);
	}

	json_object_set_number(sys_obj, "num_meshes", renderers.size());
	json_object_set_value(sys_obj, "meshes", mesh_arr_val);
}

void S_MeshRenderer::JSONDeserializeComponents(const JSON_Object* sys_obj) {
	const JSON_Array* mesh_arr = json_object_get_array(sys_obj, "meshes");
	int num_meshes = json_object_get_number(sys_obj, "num_meshes");
	renderers.reserve(renderers.size() + num_meshes);

	for (int i = 0; i < num_meshes; ++i) {
		const JSON_Object* curr_m = json_array_get_object(mesh_arr, i);
		renderers.push_back(C_MeshRenderer());
		C_MeshRenderer& m = renderers.back();
		m.id.id = json_object_get_u64(curr_m, "id");
		m.id.parent_id = json_object_get_u64(curr_m, "pid");

		m.associated_transform.id = json_object_get_u64(curr_m, "transform_id");
		m.associated_transform.ctype = ComponentTypes::CT_Transform;
		m.associated_transform.parent_id = m.id.parent_id;

		m.gl_state = json_object_get_u64(curr_m, "gl_state");
		m.mesh = json_object_get_u64(curr_m, "mesh_id");
		m.material = json_object_get_u64(curr_m, "material_id");

;		m.local.scaleX = json_object_get_number(curr_m, "r0c0");
		m.local.shearXy = json_object_get_number(curr_m, "r0c1");
		m.local.shearXz = json_object_get_number(curr_m, "r0c2");
		m.local.x = json_object_get_number(curr_m, "r0c3");

		m.local.shearYx = json_object_get_number(curr_m, "r1c0");
		m.local.scaleY = json_object_get_number(curr_m, "r1c1");
		m.local.shearYz = json_object_get_number(curr_m, "r1c2");
		m.local.y = json_object_get_number(curr_m, "r1c3");

		m.local.shearZx = json_object_get_number(curr_m, "r2c0");
		m.local.shearZy = json_object_get_number(curr_m, "r2c1");
		m.local.scaleZ = json_object_get_number(curr_m, "r2c2");
		m.local.z = json_object_get_number(curr_m, "r2c3");

		m.local.shearWx = json_object_get_number(curr_m, "r3c0");
		m.local.shearWy = json_object_get_number(curr_m, "r3c1");
		m.local.shearWz = json_object_get_number(curr_m, "r3c2");
		m.local.w = json_object_get_number(curr_m, "r3c3");
	}
}
