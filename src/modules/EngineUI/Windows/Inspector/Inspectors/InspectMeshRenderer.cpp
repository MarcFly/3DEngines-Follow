#include "../ComponentInspector.h"
#include <src/modules/ECS/ComponentsRender/CSMeshRenderer.h>
#include <src/modules/ECS/ComponentsRender/ECS_RendererTypes.h>

bool InspectMeshRenderer(Component* c) {
	bool changes_happened = false;
	C_MeshRenderer* m = (C_MeshRenderer*)c;
	if (ImGui::CollapsingHeader("MeshRenderer##" + c->id.id)) {

	}

	return changes_happened;
}