#pragma once
#include <vector>
#include <stdint.h>
#include <src/helpers/MathGeoLib/MathGeoLib.h>

struct MaterialGroup {
	uint32_t material;
	std::vector<uint32_t> meshes;
	std::vector<uint32_t> world_matrices;
};
struct RenderGroup {
	uint32_t gl_state;
	std::vector<MaterialGroup> materialgroups;
};

struct FullGroup {
	std::vector<RenderGroup> gl_state_groups;
	std::vector<float4x4> transforms;
};