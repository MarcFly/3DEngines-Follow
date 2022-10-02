#pragma once
#include <stdint.h>
#include <libs/glew/include/GL/glew.h>
#include <gl/GL.h>
#include <src/helpers/MathGeoLib/MathGeoLib.h>

struct OpenGLState {
	bool lighting = false;
	bool cull_faces = true;
	bool depth_test = true;
	bool color_material = true;
	bool texture2D = true;

	uint32_t src_blend, dst_blend;
	uint32_t poly_mode = GL_FRONT_AND_BACK, poly_fill = GL_FILL;
};


struct VTX_arr {
	std::vector<float3> vertices;
	GLenum draw_mode = GL_STATIC_DRAW;
};