#include "Primitives.h"
#include <src/helpers/MathGeoLib/MathGeoLib.h>
#include <libs/glew/include/GL/glew.h>

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

void VBI_Pyramid() {
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_vert_id);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	//glNormalPointer(GL_FLOAT, 0, pyramid_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_idx_id);
	glDrawElements(GL_TRIANGLES, num_pyr_indices, GL_UNSIGNED_INT, nullptr);
	glDisableClientState(GL_VERTEX_ARRAY);
}
//=================================
std::vector<float3> disk_sphere_vertices;
uint32_t disk_sphere_vert_id;
std::vector<uint32_t> disk_sphere_indices;
uint32_t disk_sphere_idx_id;
float radius = 2.;
void GenDiskSphere(int h_divs, int w_divs) {
	float R = 1. / (float)(h_divs - 1);
	float S = 1. / (float)(w_divs - 1);

	int r, s;

	disk_sphere_vertices.resize(h_divs * w_divs);
	auto vert_it = disk_sphere_vertices.begin();
	for (r = 0; r < h_divs; ++r)for (s = 0; s < w_divs; ++s) {
		float y = sin(-pi / 2. + pi * r * R);
		float x = cos(2. * pi * s * S) * sin(pi * r * R);
		float z = sin(2. * pi * s * S) * sin(pi * r * R);

		//*t++ = s * S;
		//*t++ = r * R;
		
		*vert_it++ = float3(x,y,z) * radius;

		//*n++ = x;
		//*n++ = y;
		//*n++ = z;
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
	
	glGenBuffers(1, &disk_sphere_idx_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, disk_sphere_idx_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, disk_sphere_indices.size() * sizeof(uint32_t), disk_sphere_indices.data(), GL_STATIC_DRAW);
}

void VBI_DiskSphere() {
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, disk_sphere_vert_id);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, disk_sphere_idx_id);
	glDrawElements(GL_QUADS, disk_sphere_indices.size(), GL_UNSIGNED_INT, nullptr);
}

//=================================

void InitPrimitives() {
	for (int i : cube_indices) raw_cube.push_back(cube_vertices[i]);
	glGenBuffers(1, &raw_cube_id);
	glBindBuffer(GL_ARRAY_BUFFER, raw_cube_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* raw_cube.size(), raw_cube.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &pyramid_vert_id);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_vert_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &pyramid_idx_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_idx_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramid_indices), pyramid_indices, GL_STATIC_DRAW);

	GenDiskSphere(10, 10);
}

void CleanUpPrimitives() {
	glDeleteBuffers(1, &cube_id);
}