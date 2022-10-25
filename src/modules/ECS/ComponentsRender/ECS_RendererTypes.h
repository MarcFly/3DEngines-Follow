#pragma once
#include <vector>
#include <stdint.h>
#include <src/helpers/MathGeoLib/MathGeoLib.h>

struct MaterialGroup {
	uint64_t material;
	std::vector<uint64_t> meshes;
	std::vector<uint64_t> world_matrices;
};
struct RenderGroup {
	uint64_t gl_state;
	std::vector<MaterialGroup> materialgroups;
};

struct FullGroup {
	std::vector<RenderGroup> gl_state_groups;
	std::vector<float4x4> transforms;
};