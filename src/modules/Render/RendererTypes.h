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

	uint32_t src_blend = GL_SRC_ALPHA, dst_blend = GL_ONE_MINUS_SRC_ALPHA;
	uint32_t poly_mode = GL_FRONT_AND_BACK, poly_fill = GL_FILL;
};


struct VTX_arr {
	std::vector<float3> vertices;
	GLenum draw_mode = GL_STATIC_DRAW;
};

// Non-Interleaved Mesh - Data Blocks
struct NIMesh {
	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<float2> uvs;

	std::vector<uint32_t> indices;
	std::vector<uint16_t> h_indices; // in case it has halfsized indices

	GLenum draw_mode;
	GLenum polytype;
};

struct GPUMesh {
	uint32_t vtx_id = 0;
	uint32_t norm_id = 0;
	uint32_t uvs_id = 0;
	uint32_t num_vtx = 0;

	uint32_t idx_id = 0;
	uint64_t num_idx = 0;
};