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

	float4x4 local;
	ComponentID associated_transform;
	uint32_t cached_wm = UINT32_MAX;

	void DrawInspector();
};

#define NUM_TEST_MESHES 1000000
#define NUM_MATERIALS 1000
#define NUM_GL_STATES 100

struct S_MeshRenderer : public System {
	S_MeshRenderer() : System(CT_MeshRenderer) {};
	std::vector<C_MeshRenderer> renderers;
	std::vector<ComponentID> to_delete;
	// GL State might have multiple materials
	// Inside each material inside each GL_State, might have multiple meshes
	//std::vector<RenderGroup> groups;
	FullGroup send;
	bool recache = false;

	void TestInit();

	inline void CacheGroups();

	inline void SetMeshTransf(MaterialGroup& mg, C_MeshRenderer& cr);

	inline void SetMat(RenderGroup& g, C_MeshRenderer& cr);
	

	update_status PreUpdate(float dt);

	update_status Update(float dt);

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);

	Component* AddC(const ComponentTypes ctype, const uint64_t eid) {
		
		renderers.push_back(C_MeshRenderer());
		C_MeshRenderer& t = renderers.back();
		t.id.parent_id = eid;
		t.id.quick_ref = renderers.size() - 1;
		return (Component*)&t;
	}

	void AddToDeleteQ(const ComponentID& cid) { to_delete.push_back(cid); }

	void DeleteComponent(const ComponentID& cid) {
		ComponentID temp = cid;
		C_MeshRenderer* cmesh = (C_MeshRenderer*)GetC(temp);
		*cmesh = renderers[renderers.size() - 1];
		cmesh->cached_wm = UINT32_MAX;
		send.gl_state_groups.clear();
		send.transforms.clear();
		renderers.pop_back();
		recache = true;
	}

	Component* GetCByRef(const ComponentID& cid) {
		Component* ret = nullptr;
		if (cid.quick_ref > renderers.size() - 1 || renderers[cid.quick_ref].id.id != cid.id)
			return nullptr;

		return (Component*)&renderers[cid.quick_ref];
	}
	Component* GetC(ComponentID& cid) {
		Component* ret = nullptr;
		for (int i = 0; i < renderers.size(); ++i)
			if (renderers[i].id.id == cid.id) {
				cid.quick_ref = i;
				return &renderers[i];
			}
		return ret;
	}

};