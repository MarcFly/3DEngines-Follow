#include "Primitives.h"
#include <src/helpers/MathGeoLib/MathGeoLib.h>
#include <libs/glew/include/GL/glew.h>

GLuint checkers_textureID;

std::vector<float3> raw_cube;
uint32_t raw_cube_id;
static float3 colors[] = {
	{1., 1., 0.}, {1., 0., 1.}, {0., 1., 1.},
	{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}
};

float3 cube_vertices[] = {
	{-1., -1., -1.}, {1., -1., -1.},
	{-1., 1., -1.}, {1., 1., -1.},
	{-1., -1., 1.}, {1., -1., 1.},
	{-1., 1., 1.}, {1., 1., 1.}

};
constexpr int num_cube_vertices = sizeof(cube_vertices) / sizeof(float3);

int cube_indices[] = {
	4,5,7,	4,7,6,
	3,1,0,	0,2,3,
	3,2,7,	2,6,7,
	1,5,0,	0,5,4,
	0,6,2,	0,4,6,
	1,3,7,	1,7,5
};
constexpr int num_cube_indices = sizeof(cube_indices) / sizeof(int);
uint32_t cube_id;

void VB_Cube() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, raw_cube_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	// … bind and use other buffers, such as vertex color, texcoords, normals,...

	glDrawArrays(GL_TRIANGLES, 0, raw_cube.size());
	glDisableClientState(GL_VERTEX_ARRAY);
}

//=================================
float3 pyramid_vertices[] = {
	{1., 1., 1.}, {1., -1., -1.}, {-1., 1., -1.}, {-1., -1., 1.}
};
constexpr int num_pyr_vertices = sizeof(pyramid_vertices) / sizeof(float3);
uint32_t pyramid_indices[] = {
	0,3,1,	0,1,2,
	0,2,3,	3,2,1,
};
constexpr int num_pyr_indices = sizeof(pyramid_indices) / sizeof(uint32_t);
uint32_t pyramid_vert_id;
uint32_t pyramid_idx_id;

float2 pyramid_uvs[] = {
	{1., 1.}, {1., 0.}, {0., 1.}, {0., 0.}
};
uint32_t pyr_uv_id;

void VBI_Pyramid() {
	
	glEnable(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, checkers_textureID);

	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_vert_id);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	//glNormalPointer(GL_FLOAT, 0, pyramid_vertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, pyr_uv_id);
	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_idx_id);
	glDrawElements(GL_TRIANGLES, num_pyr_indices, GL_UNSIGNED_INT, nullptr);
	glDisableClientState(GL_VERTEX_ARRAY);
}
//=================================
std::vector<float3> disk_sphere_vertices;
uint32_t disk_sphere_vert_id;
std::vector<float3> disk_sphere_normals;
uint32_t disk_sphere_norm_id;
std::vector<float2> disk_sphere_uvs;
uint32_t disk_sphere_uv_id;

std::vector<uint32_t> disk_sphere_indices;
uint32_t disk_sphere_idx_id;
float radius = 2.;
void GenDiskSphere(int h_divs, int w_divs) {
	float R = 1. / (float)(h_divs - 1);
	float S = 1. / (float)(w_divs - 1);

	int r, s;

	disk_sphere_vertices.resize(h_divs * w_divs);
	disk_sphere_normals.resize(h_divs * w_divs);
	disk_sphere_uvs.resize(h_divs * w_divs);
	auto vert_it = disk_sphere_vertices.begin();
	auto norm_it = disk_sphere_normals.begin();
	auto uv_it = disk_sphere_uvs.begin();
	for (r = 0; r < h_divs; ++r) {
		for (s = 0; s < w_divs; ++s) {
			const float y = sin(-pi / 2. + pi * r * R);
			const float x = cos(2. * pi * s * S) * sin(pi * r * R);
			const float z = sin(2. * pi * s * S) * sin(pi * r * R);

			*uv_it++ = float2(s * S, r * R);

			*vert_it++ = float3(x, y, z) * radius;

			*norm_it++ = float3(x, y, z);
		}
	}

	disk_sphere_indices.resize(h_divs * w_divs * 4);
	auto idx_it = disk_sphere_indices.begin();
	for (r = 0; r < h_divs; r++) for (s = 0; s < w_divs; s++) {
		*idx_it++ = r * w_divs + s;
		*idx_it++ = (r + 1) * w_divs + s;
		
		*idx_it++ = (r + 1) * w_divs + (s + 1);
		*idx_it++ = r * w_divs + (s + 1);
		
	}

	glGenBuffers(1, &disk_sphere_vert_id);
	glBindBuffer(GL_ARRAY_BUFFER, disk_sphere_vert_id);
	glBufferData(GL_ARRAY_BUFFER, disk_sphere_vertices.size() * sizeof(float3), disk_sphere_vertices.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &disk_sphere_norm_id);
	glBindBuffer(GL_ARRAY_BUFFER, disk_sphere_norm_id);
	glBufferData(GL_ARRAY_BUFFER, disk_sphere_normals.size() * sizeof(float3), disk_sphere_normals.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &disk_sphere_uv_id);
	glBindBuffer(GL_ARRAY_BUFFER, disk_sphere_uv_id);
	glBufferData(GL_ARRAY_BUFFER, disk_sphere_uvs.size() * sizeof(float2), disk_sphere_uvs.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &disk_sphere_idx_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, disk_sphere_idx_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, disk_sphere_indices.size() * sizeof(uint32_t), disk_sphere_indices.data(), GL_STATIC_DRAW);
}

void VBI_DiskSphere() {

	glEnable(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, checkers_textureID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, disk_sphere_vert_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, disk_sphere_norm_id);
	glNormalPointer(GL_FLOAT, 0, NULL);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, disk_sphere_uv_id);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, disk_sphere_idx_id);
	glDrawElements(GL_QUADS, disk_sphere_indices.size(), GL_UNSIGNED_INT, NULL);

	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

//=================================
#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64
#include <src/helpers/Globals.h>

GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
//GLuint checkers_textureID;
void InitCheckers() {
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkers_textureID);
	glBindTexture(GL_TEXTURE_2D, checkers_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	PLOG("Check for error: %s\n", gluErrorString(glGetError()));
}


void InitPrimitives() {
	for (int i : cube_indices) raw_cube.push_back(cube_vertices[i]);
	glGenBuffers(1, &raw_cube_id);
	glBindBuffer(GL_ARRAY_BUFFER, raw_cube_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* raw_cube.size(), raw_cube.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &pyramid_vert_id);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_vert_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &pyr_uv_id);
	glBindBuffer(GL_ARRAY_BUFFER, pyr_uv_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_uvs), pyramid_uvs, GL_STATIC_DRAW);
	glGenBuffers(1, &pyramid_idx_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_idx_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramid_indices), pyramid_indices, GL_STATIC_DRAW);

	GenDiskSphere(10, 10);

	InitCheckers();
}

void CleanUpPrimitives() {
	glDeleteBuffers(1, &cube_id);
	glDeleteTextures(1, &checkers_textureID);
}