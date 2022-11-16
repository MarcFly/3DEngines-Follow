#include "EnginePCH.h"

#include <imgui.h>
#include "CS_MeshRenderer.h"

using namespace Engine;

COMPONENT_TYPE_STATIC_DATA(C_MeshRenderer);
TYPE_STATIC_DATA(S_MeshRenderer);
DEF_SystemGenericFuns(S_MeshRenderer, C_MeshRenderer);

S_MeshRenderer::~S_MeshRenderer() {}
void C_MeshRenderer::DrawInspector() {
	static char headerid[64];
	static bool world_t = false;
	static Quat temp_quat;
	static float3 temp_rot, prev_rot;
	static float3 temp_pos, prev_pos;
	static float3 temp_scale, prev_scale;

	bool changed = false;

	snprintf(headerid, sizeof(headerid), "MeshRenderer##%llu", cid.id);
	if (ImGui::CollapsingHeader(headerid)) {
		snprintf(headerid, sizeof(headerid), "Mesh Transform##%llu", cid.id);
		if (ImGui::CollapsingHeader(headerid)) {
			snprintf(headerid, sizeof(headerid), "RESET##%llu", cid.id);

			local_mat.Decompose(temp_pos, temp_quat, temp_scale);

			prev_pos = temp_pos;
			prev_rot = temp_rot = temp_quat.ToEulerXYZ();
			prev_scale = temp_scale;
			snprintf(headerid, sizeof(headerid), "POSITION##%llu", cid.id);
			changed |= ImGui::DragFloat3(headerid, temp_pos.ptr(), .2f);
			snprintf(headerid, sizeof(headerid), "ROTATION##%llu", cid.id);
			changed |= ImGui::DragFloat3(headerid, temp_rot.ptr(), .1f);
			snprintf(headerid, sizeof(headerid), "SCALE##%llu", cid.id);
			changed |= ImGui::DragFloat3(headerid, temp_scale.ptr(), .2f);

			if (changed) {
				temp_pos -= prev_pos;
				temp_rot -= prev_rot;
				temp_quat = Quat::FromEulerXYZ(temp_rot.x, temp_rot.y, temp_rot.z);
				temp_scale -= prev_scale;

				Quat prev_quat;
				local_mat.Decompose(prev_pos, prev_quat, prev_scale);
				prev_pos += temp_pos;
				prev_quat = temp_quat.Mul(prev_quat);
				prev_scale += temp_scale;

				// Check no smaller than 0.01f, Mathgeolib limit
				prev_scale.x = (prev_scale.x < 0.01f) ? 0.01f : prev_scale.x;
				prev_scale.y = (prev_scale.y < 0.01f) ? 0.01f : prev_scale.y;
				prev_scale.z = (prev_scale.z < 0.01f) ? 0.01f : prev_scale.z;


				local_mat = float4x4::FromTRS(prev_pos, prev_quat, prev_scale);
			}
		}

		ImGui::Text("Disk Mesh ID: %llu", mesh.id);
		ImGui::Text("Disk Material ID: %llu", mat.id);
	}
}

JSON_Value* S_MeshRenderer::JSONValueFromComponent(const Component* c) {
	const C_MeshRenderer& m = *(const C_MeshRenderer*)c;

	JSON_Value* ret = json_value_init_object();
	JSON_Object* m_obj = json_object(ret);

	json_object_set_u64(m_obj, "id", m.cid.id);
	json_object_set_u64(m_obj, "ctype", m.cid.ctype);
	json_object_set_u64(m_obj, "parent_id", m.cid.parent_id);

	json_object_set_u64(m_obj, "diskmesh", m.mesh.id);
	json_object_set_u64(m_obj, "diskmat", m.mat.id);

	float3 pos, scale;
	Quat rot;
	m.local_mat.Decompose(pos, rot, scale);

	JSON_Value* t_pos_val = json_value_init_array();
	JSON_Array* t_pos_arr = json_array(t_pos_val);
	json_array_append_number(t_pos_arr, pos.x);
	json_array_append_number(t_pos_arr, pos.y);
	json_array_append_number(t_pos_arr, pos.z);
	json_object_set_value(m_obj, "position", t_pos_val);

	JSON_Value* t_scale_val = json_value_init_array();
	JSON_Array* t_scale_arr = json_array(t_scale_val);
	json_array_append_number(t_scale_arr, scale.x);
	json_array_append_number(t_scale_arr, scale.y);
	json_array_append_number(t_scale_arr, scale.z);
	json_object_set_value(m_obj, "scale", t_scale_val);

	JSON_Value* t_rot_val = json_value_init_array();
	JSON_Array* t_rot_arr = json_array(t_rot_val);
	json_array_append_number(t_rot_arr, rot.x);
	json_array_append_number(t_rot_arr, rot.y);
	json_array_append_number(t_rot_arr, rot.z);
	json_array_append_number(t_rot_arr, rot.w);
	json_object_set_value(m_obj, "rotation", t_rot_val);

	return ret;
}

void S_MeshRenderer::ComponentFromJSONObject(const JSON_Object* m_obj) {
	components.push_back(C_MeshRenderer());
	C_MeshRenderer& m = components.back();

	m.cid.id = json_object_get_u64(m_obj, "id");
	m.cid.ctype = json_object_get_u64(m_obj, "ctype");
	m.cid.parent_id = json_object_get_u64(m_obj, "parent_id");

	m.mat.id = json_object_get_u64(m_obj, "diskmat");
	m.mesh.id = json_object_get_u64(m_obj, "diskmesh");

	float3 pos, scale;
	Quat rot;

	const JSON_Array* t_pos_arr = json_object_get_array(m_obj, "position");
	pos.x = json_array_get_number(t_pos_arr, 0);
	pos.y = json_array_get_number(t_pos_arr, 1);
	pos.z = json_array_get_number(t_pos_arr, 2);

	const JSON_Array* t_scale_arr = json_object_get_array(m_obj, "scale");
	scale.x = json_array_get_number(t_scale_arr, 0);
	scale.y = json_array_get_number(t_scale_arr, 1);
	scale.z = json_array_get_number(t_scale_arr, 2);

	const JSON_Array* t_rot_arr = json_object_get_array(m_obj, "rotation");
	rot.x = json_array_get_number(t_rot_arr, 0);
	rot.y = json_array_get_number(t_rot_arr, 1);
	rot.z = json_array_get_number(t_rot_arr, 2);
	rot.w = json_array_get_number(t_rot_arr, 3);

	m.local_mat = float4x4::FromTRS(pos, rot, scale);
}