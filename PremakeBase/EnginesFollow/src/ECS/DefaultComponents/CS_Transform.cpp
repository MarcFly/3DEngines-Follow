#include "EnginePCH.h"
#include "CS_Transform.h"

using namespace Engine;
COMPONENT_TYPE_STATIC_DATA(C_Transform);
TYPE_STATIC_DATA(S_Transform);
DEF_SystemGenericFuns(S_Transform, C_Transform);
// SYSTEM

S_Transform::~S_Transform() {
}

#include <imgui.h>
void C_Transform::DrawInspector() {
	static char headerid[64];
	static bool world_t = false;
	static Quat temp_quat;
	static float3 temp_rot, prev_rot;
	static float3 temp_pos, prev_pos;
	static float3 temp_scale, prev_scale;
	
	bool changed = false;

	snprintf(headerid, sizeof(headerid), "Transform##%llu", cid.id);
	if (ImGui::CollapsingHeader(headerid)) {
		snprintf(headerid, sizeof(headerid), "STATIC##%llu", cid.id);
		ImGui::Checkbox(headerid, &is_static);
		ImGui::SameLine();
		snprintf(headerid, sizeof(headerid), "RESET##%llu", cid.id);
		if (ImGui::Button(headerid)) local_mat = float4x4::identity;
		if (ImGui::RadioButton("World", world_t)) world_t = !world_t;
		ImGui::SameLine();
		if (ImGui::RadioButton("Local", !world_t)) world_t = !world_t;

		float4x4 use_mat = (world_t) ? world_mat * local_mat : local_mat;
		use_mat.Decompose(temp_pos, temp_quat, temp_scale);

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
}

JSON_Value* S_Transform::JSONValueFromComponent(const Component* c) {
	const C_Transform& t = *(const C_Transform*) c;

	JSON_Value* t_val = json_value_init_object();
	JSON_Object* t_obj = json_object(t_val);

	json_object_set_u64(t_obj, "id", t.cid.id);
	json_object_set_u64(t_obj, "ctype", t.cid.ctype);
	json_object_set_u64(t_obj, "parent_id", t.cid.parent_id);

	json_object_set_boolean(t_obj, "is_static", t.is_static);

	float3 pos, scale;
	Quat rot;
	t.local_mat.Decompose(pos, rot, scale);

	JSON_Value* t_pos_val = json_value_init_array();
	JSON_Array* t_pos_arr = json_array(t_pos_val);
	json_array_append_number(t_pos_arr, pos.x);
	json_array_append_number(t_pos_arr, pos.y);
	json_array_append_number(t_pos_arr, pos.z);
	json_object_set_value(t_obj, "position", t_pos_val);

	JSON_Value* t_scale_val = json_value_init_array();
	JSON_Array* t_scale_arr = json_array(t_scale_val);
	json_array_append_number(t_scale_arr, scale.x);
	json_array_append_number(t_scale_arr, scale.y);
	json_array_append_number(t_scale_arr, scale.z);
	json_object_set_value(t_obj, "scale", t_scale_val);

	JSON_Value* t_rot_val = json_value_init_array();
	JSON_Array* t_rot_arr = json_array(t_rot_val);
	json_array_append_number(t_rot_arr, rot.x);
	json_array_append_number(t_rot_arr, rot.y);
	json_array_append_number(t_rot_arr, rot.z);
	json_array_append_number(t_rot_arr, rot.w);
	json_object_set_value(t_obj, "rotation", t_rot_val);

	return t_val;
}
void S_Transform::ComponentFromJSONObject(const JSON_Object* t_obj) {
	components.push_back(C_Transform());
	C_Transform& t = components.back();

	t.cid.id = json_object_get_u64(t_obj, "id");
	t.cid.ctype = json_object_get_u64(t_obj, "ctype");
	t.cid.parent_id = json_object_get_u64(t_obj, "parent_id");

	t.is_static = json_object_get_boolean(t_obj, "is_static");

	float3 pos, scale;
	Quat rot;
	
	const JSON_Array* t_pos_arr = json_object_get_array(t_obj, "position");
	pos.x = json_array_get_number(t_pos_arr, 0);
	pos.y = json_array_get_number(t_pos_arr, 1);
	pos.z = json_array_get_number(t_pos_arr, 2);

	const JSON_Array* t_scale_arr = json_object_get_array(t_obj, "scale");
	scale.x = json_array_get_number(t_scale_arr, 0);
	scale.y = json_array_get_number(t_scale_arr, 1);
	scale.z = json_array_get_number(t_scale_arr, 2);
	
	const JSON_Array* t_rot_arr = json_object_get_array(t_obj, "rotation");
	rot.x = json_array_get_number(t_rot_arr, 0);
	rot.y = json_array_get_number(t_rot_arr, 1);
	rot.z = json_array_get_number(t_rot_arr, 2);
	rot.w = json_array_get_number(t_rot_arr, 3);

	t.local_mat = float4x4::FromTRS(pos, rot, scale);
}