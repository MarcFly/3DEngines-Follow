#pragma once

#include "../ModuleECS.h"
#include <src/modules/Render/ModuleRenderer3D.h>
#include <src/modules/Render/RendererTypes.h>

struct C_MeshRenderer : public Component {
	C_MeshRenderer() { id.ctype = CT_MeshRenderer; }
	uint32_t gl_state;
	uint32_t mesh;
	uint32_t material;
	
	ComponentID associated_transform;
	uint32_t cached_wm = UINT32_MAX;
};

#include "../ComponentsMove/CS_Transform.h"

struct MaterialGroup {
	uint32_t material;
	std::vector<uint32_t> meshes;
	std::vector<uint32_t> world_matrices;
};
struct RenderGroup {
	uint32_t gl_state;
	std::vector<MaterialGroup> materialgroups;
};

struct S_MeshRenderer : public System {
	ComponentTypes ctype = CT_MeshRenderer;
	std::vector<C_MeshRenderer> renderers;
	std::vector<float4x4> global_transform;

	// GL State might have multiple materials
	// Inside each material inside each GL_State, might have multiple meshes
	std::vector<RenderGroup> groups;

	void SetMeshTransf(MaterialGroup& mg, C_MeshRenderer& cr) {
		for (int i = 0; i < mg.meshes.size(); ++i)
			if (mg.meshes[i] == cr.mesh) {
				mg.world_matrices[i] = cr.cached_wm;
				return;
			}
		
		mg.material = cr.material;
		mg.meshes.emplace_back(cr.mesh);
		// Get Transform through Parenting...
		float4x4 t;
		global_transform.emplace_back(t);

		cr.cached_wm = global_transform.size() - 1;
	}

	void SetMat(RenderGroup& g, C_MeshRenderer& cr) {
		for (int i = 0; i < g.materialgroups.size(); ++i)
			if (g.materialgroups[i].material == cr.material) {
				SetMeshTransf(g.materialgroups[i], cr);
				return;
			}
		g.materialgroups.emplace_back(MaterialGroup());
		MaterialGroup& mg = g.materialgroups.back();
		mg.material = cr.material;
		mg.meshes.emplace_back(cr.mesh);

		SetMeshTransf(mg, cr);
	}
	

	update_status PreUpdate(float dt) {
		// Recreate rendergroups each frame
		// Reasons: Invalidation of data
		groups.clear();
		for (int i = 0; i < renderers.size(); ++i) {
			if (renderers[i].cached_wm == UINT32_MAX) {
				// Get Transform through Parenting...
				float4x4 t;
				global_transform.emplace_back(t);
				renderers[i].cached_wm = global_transform.size() - 1;
			}
			for (int j = 0; j < groups.size(); ++j) {
				if (groups[j].gl_state == renderers[i].gl_state) {
					SetMat(groups[j], renderers[i]);
					continue;
				}
				
				groups.emplace_back(RenderGroup());
				RenderGroup& g = groups.back();
				SetMat(g, renderers[i]);				
			}
		}
	}

};