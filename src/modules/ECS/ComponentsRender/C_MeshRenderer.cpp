#include "CSMeshRenderer.h"
#include <src/modules/EngineUI/DearImGUI/imgui.h>

static char headerid[64];
void C_MeshRenderer::DrawInspector() {
	sprintf(headerid, "MeshRenderer##%llu", id.id);
	if (ImGui::CollapsingHeader(headerid)) {

	}
}