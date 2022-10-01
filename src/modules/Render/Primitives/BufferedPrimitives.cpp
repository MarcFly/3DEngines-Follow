#include "Primitives.h"
#include <src/helpers/MathGeoLib/MathGeoLib.h>
#include <libs/glew/include/GL/glew.h>

std::vector<float3> raw_cube;
uint32_t raw_cube_id;
float3 colors[] = {
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
	5,6,8,	5,8,7,
	4,2,1,	1,3,4,
	4,3,8,	3,7,8,
	2,6,1,	1,6,5,
	1,7,3,	1,5,7,
	2,4,8,	2,8,6
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

void InitPrimitives() {
	for (int i : cube_indices) raw_cube.push_back(cube_vertices[i - 1]);
	glGenBuffers(1, &raw_cube_id);
	glBindBuffer(GL_ARRAY_BUFFER, raw_cube_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* raw_cube.size(), raw_cube.data(), GL_STATIC_DRAW);
}

void CleanUpPrimitives() {
	glDeleteBuffers(1, &cube_id);
}