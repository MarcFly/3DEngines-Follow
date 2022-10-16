#pragma once
#include <src/Application.h>

#include "../ModuleECS.h"
#include <src/modules/Render/ModuleRenderer3D.h>
#include <src/modules/Render/RendererTypes.h>
#include "../ComponentsMove/CS_Transform.h"
#include "ECS_RendererTypes.h"


struct C_MeshRenderer : public Component {
	C_MeshRenderer() { id.ctype = CT_MeshRenderer; }
	uint32_t gl_state;
	uint32_t mesh;
	uint32_t material;
	bool cached_group = false;

	ComponentID associated_transform;
	uint32_t cached_wm = UINT32_MAX;
};

#define NUM_TEST_MESHES 500000
#define NUM_MATERIALS 10000
#define NUM_GL_STATES 100

struct S_MeshRenderer : public System {
	ComponentTypes ctype = CT_MeshRenderer;
	std::vector<C_MeshRenderer> renderers;
	std::vector<float4x4> global_transform;

	// GL State might have multiple materials
	// Inside each material inside each GL_State, might have multiple meshes
	std::vector<RenderGroup> groups;
	FullGroup send;

	void TestInit() {
		// Cost Test
		renderers.resize(NUM_TEST_MESHES);
		global_transform.resize(NUM_TEST_MESHES);
		for (int i = 0; i < NUM_TEST_MESHES; ++i) {
			C_MeshRenderer& curr = renderers[i];
			curr.cached_wm = i;
			global_transform.emplace_back(float4x4::identity);
			// Worst case scenario in cpu and GPU then
			curr.gl_state = NUM_GL_STATES * (PCGRand() / (double)UINT64_MAX);// State per mesh
			curr.material = NUM_MATERIALS * (PCGRand() / (double)UINT64_MAX); // Material per mesh
			curr.mesh = PCGRand(); // All different meshes	
		}
		
		groups.reserve(NUM_GL_STATES);
		for (int i = 0; i < groups.size(); ++i) {
			groups[i].materialgroups.reserve(NUM_MATERIALS);
		}
		CacheGroups();
	}

	S_MeshRenderer() {
		//TestInit();
		send.gl_state_groups = &groups;
		send.transforms = &global_transform;
	}
	inline void CacheGroups() {
		for (int i = 0; i < renderers.size(); ++i) {
			if (renderers[i].cached_group) continue;
			//if (renderers[i].cached_wm == UINT32_MAX) {
			//	// Get Transform through Parenting...
			//	float4x4 t;
			//	global_transform.emplace_back(t);
			//	renderers[i].cached_wm = global_transform.size() - 1;
			//}
			for (int j = 0; j < groups.size(); ++j) {
				if (groups[j].gl_state == renderers[i].gl_state) {
					SetMat(groups[j], renderers[i]);
					continue;
				}
			}
			if (renderers[i].cached_group) continue;
			groups.emplace_back(RenderGroup());
			RenderGroup& g = groups.back();
			g.gl_state = renderers[i].gl_state;
			SetMat(g, renderers[i]);
		}
	}

	inline void SetMeshTransf(MaterialGroup& mg, C_MeshRenderer& cr) {
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

	inline void SetMat(RenderGroup& g, C_MeshRenderer& cr) {
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
	

	update_status PreUpdate(float dt) {
		// Recreate rendergroups each frame
		// Reasons: Invalidation of data, not ideal
		
		// TODO: Change this to be recreated only when there are changes		
		// Should not be called per frame, only push and pop operations!
		CacheGroups(); 

		return UPDATE_CONTINUE;
	}

	update_status Update(float dt) {
		EV_SEND_POINTER(ECS_RENDERABLES, (void*)& send);

		return UPDATE_CONTINUE;
	}

};