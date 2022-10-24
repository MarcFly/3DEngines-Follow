#include "CSMeshRenderer.h"
#include <src/modules/EngineUI/DearImGUI/imgui.h>

static char headerid[64];

static Quat temp_quat;
static float3 temp_rot, prev_rot;
static float3 temp_pos, prev_pos;
static float3 temp_scale, prev_scale;

void C_MeshRenderer::DrawInspector() {
	bool changed = false;

	sprintf(headerid, "MeshRenderer##%llu", id.id);
	if (ImGui::CollapsingHeader(headerid)) {
		local.Decompose(temp_pos, temp_quat, temp_scale);

		prev_pos = temp_pos;
		prev_rot = temp_rot = temp_quat.ToEulerXYZ();
		prev_scale = temp_scale;
		sprintf(headerid, "POSITION##%llu", id.id);
		changed |= ImGui::DragFloat3(headerid, temp_pos.ptr(), .2f);
		sprintf(headerid, "ROTATION##%llu", id.id);
		changed |= ImGui::DragFloat3(headerid, temp_rot.ptr(), .1f);
		sprintf(headerid, "SCALE##%llu", id.id);
		changed |= ImGui::DragFloat3(headerid, temp_scale.ptr(), .2f);

		if (changed) {
			temp_pos -= prev_pos;
			temp_rot -= prev_rot;
			temp_quat = Quat::FromEulerXYZ(temp_rot.x, temp_rot.y, temp_rot.z);
			temp_scale -= prev_scale;

			Quat prev_quat;
			local.Decompose(prev_pos, prev_quat, prev_scale);
			prev_pos += temp_pos;
			prev_quat = temp_quat.Mul(prev_quat);
			prev_scale += temp_scale;

			// Check no smaller than 0.01f, Mathgeolib limit
			prev_scale.x = (prev_scale.x < 0.01f) ? 0.01f : prev_scale.x;
			prev_scale.y = (prev_scale.y < 0.01f) ? 0.01f : prev_scale.y;
			prev_scale.z = (prev_scale.z < 0.01f) ? 0.01f : prev_scale.z;


			local = float4x4::FromTRS(prev_pos, prev_quat, prev_scale);
		}
	}
}