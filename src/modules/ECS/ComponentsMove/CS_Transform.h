#pragma once

#include "../ModuleECS.h"
#include <src/helpers/MathGeoLib/MathGeoLib.h>
#include <src/Application.h>

struct C_Transform : public Component {
	constexpr static ComponentTypes type = CT_Transform;
	C_Transform() { id.ctype = CT_Transform; id.id = PCGRand(); }

	bool is_static = false;
	float4x4 world_mat = float4x4::identity;
	float4x4 local_mat = float4x4::identity;
	
	bool valid_tree = false;

	void DrawInspector();

	void PropagateChanges();
};