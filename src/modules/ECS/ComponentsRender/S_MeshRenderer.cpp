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