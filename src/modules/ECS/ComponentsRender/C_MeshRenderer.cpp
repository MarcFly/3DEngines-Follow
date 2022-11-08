#include "CSMeshRenderer.h"
#include <src/modules/EngineUI/DearImGUI/imgui.h>

static char headerid[64];

void C_MeshRenderer::DrawInspector() {
	bool changed = false;

	sprintf(headerid, "MeshRenderer##%llu", id.id);
	if (ImGui::CollapsingHeader(headerid)) {
		
	}
}

void C_MeshRenderer::PostUpdate(float dt) {
	C_Transform* t = id.parent->GetComponent<C_Transform>();
	float4x4 curr = t->world_mat * t->local_mat;
	glPushMatrix();
	glMultMatrixf(curr.Transposed().ptr());

	mesh_use.Bind();
	mat_use.Bind();
	mesh_use.Draw();

	glPopMatrix();
}