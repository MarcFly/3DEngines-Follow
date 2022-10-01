#pragma once
#include <stdint.h>

struct OpenGLState {
	bool lighting = false;
	bool cull_faces = true;
	bool depth_test = true;
	bool color_material = true;
	bool texture2D = true;

	uint32_t src_blend, dst_blend;
};
