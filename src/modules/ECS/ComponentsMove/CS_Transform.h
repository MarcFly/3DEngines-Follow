#pragma once

#include "../ModuleECS.h"
#include <src/helpers/MathGeoLib/MathGeoLib.h>

struct C_Transform : public Component {
	C_Transform() { id.ctype = CT_Transform; }
	float4x4 world_mat;
	float4x4 local_mat;
	std::vector<uint32_t> transform_tree;
	bool valid_tree = false;
};

struct S_Transform : public System {
	ComponentTypes ctype = CT_Transform;
	
};