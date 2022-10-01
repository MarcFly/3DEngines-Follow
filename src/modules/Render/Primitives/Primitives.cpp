#include "Primitives.h"

#include <libs/glew/include/GL/glew.h>
#include <gl/GL.h>

void DDCube()
{
	glBegin(GL_TRIANGLES);
	glColor3f(1., 1., 0.);

	/// <summary>
	///  8 Vertex
	/// x = -1 or 1, same for y, z
	/// Permutations
	/// 1 1 1 = 8
	/// 1 1 -1 = 4
	/// 1 -1 1 = 6
	/// -1 1 1 = 7
	/// -1 -1 1 = 5
	/// -1 1 -1 = 3
	/// -1 -1 -1 = 1
	/// 1 -1 -1 = 2
	/// </summary>

	// Remember Counterclockwise Order (Right Hand Rule)
	// glVertex3f(-1., -1., -1.);	// 1
	// glVertex3f(1., -1., -1.);	// 2
	// glVertex3f(-1., 1., -1.);	// 3
	// glVertex3f(1., 1., -1.);		// 4
	// glVertex3f(-1., -1., 1.);	// 5
	// glVertex3f(1., -1., 1.);		// 6
	// glVertex3f(-1., 1., 1.);		// 7
	// glVertex3f(1., 1., 1.);		// 8

	// Z = 1 / TriB
	glColor3f(1., 1., 0.);
	glVertex3f(-1., -1., 1.);	// 5
	glVertex3f(1., -1., 1.);	// 6
	glVertex3f(1., 1., 1.);		// 8

	// Z = 1 / TriT
	glVertex3f(-1., -1., 1.);	// 5
	glVertex3f(1., 1., 1.);		// 8
	glVertex3f(-1., 1., 1.);	// 7

	// *Backfaces have to be drawn in the inverse order you expect
	// You will look at them when you are rotated backwards
	// Z = -1 / TriB
	glColor3f(1., 0., 1.);
	glVertex3f(1., 1., -1.);	// 4
	glVertex3f(1., -1., -1.);	// 2
	glVertex3f(-1., -1., -1.);	// 1

	// Z = -1 / TriT
	glVertex3f(-1., -1., -1.);	// 1
	glVertex3f(-1., 1., -1.);	// 3
	glVertex3f(1., 1., -1.);	// 4

	// Top bottom and sides now...
	glColor3f(0., 1., 1.);
	// Top / TriR
	glVertex3f(1., 1., -1.);	// 4
	glVertex3f(-1., 1., -1.);	// 3
	glVertex3f(1., 1., 1.);		// 8

	// Top / TriL
	glVertex3f(-1., 1., -1.);	// 3
	glVertex3f(-1., 1., 1.);	// 7
	glVertex3f(1., 1., 1.);		// 8

	// Bottom / TriR
	glColor3f(1., 0., 0.);
	glVertex3f(1., -1., -1.);	// 2
	glVertex3f(1., -1., 1.);	// 6
	glVertex3f(-1., -1., -1.);	// 1

	// Bottom / TriL
	glVertex3f(-1., -1., -1.);	// 1
	glVertex3f(1., -1., 1.);	// 6
	glVertex3f(-1., -1., 1.);	// 5

	// SideL / TriT
	glColor3f(0., 1., 0.);
	glVertex3f(-1., -1., -1.);	// 1
	glVertex3f(-1., 1., 1.);	// 7
	glVertex3f(-1., 1., -1.);	// 3

	// SideL / TriB
	glVertex3f(-1., -1., -1.);	// 1
	glVertex3f(-1., -1., 1.);	// 5
	glVertex3f(-1., 1., 1.);	// 7

	// SideR / TriT
	glColor3f(0., 0., 1.);
	glVertex3f(1., -1., -1.);	// 2
	glVertex3f(1., 1., -1.);	// 4
	glVertex3f(1., 1., 1.);		// 8

	glVertex3f(1., -1., -1.);	// 2
	glVertex3f(1., 1., 1.);		// 8
	glVertex3f(1., -1., 1.);	// 6

	glEnd();
}

#include <src/helpers/MathGeoLib/MathGeoLib.h>

float3 vertices[] = {
	{-1., -1., -1.}, {1., -1., -1.},
	{-1., 1., -1.}, {1., 1., -1.},
	{-1., -1., 1.}, {1., -1., 1.},
	{-1., 1., 1.}, {1., 1., 1.}

};
int indices[] = {
	5,6,8,	5,8,7,
	4,2,1,	1,3,4,
	4,3,8,	3,7,8,
	2,6,1,	1,6,5,
	1,7,3,	1,5,7,
	2,4,8,	2,8,6
};

#define GLVertexDD(idx) {float3& v = vertices[*idx - 1]; glVertex3f(v.x, v.y, v.z);}

float3 colors[] = {
	{1., 1., 0.}, {1., 0., 1.}, {0., 1., 1.},
	{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}
};

void DDCube_VecIndices() {
	glBegin(GL_TRIANGLES);
	int col = -1;
	for (int i = 0; i < sizeof(indices) / sizeof(int); ++i) {
		if (i % 6 == 0) { 
			++col; 
			glColor3f(colors[col].x, colors[col].y, colors[col].z); 
		}
		GLVertexDD(&indices[i]);
	}
	glEnd();
}

void DDCube_BadIndices()
{
	glBegin(GL_TRIANGLES);
	int* idx = indices;

	// Front Face
	glColor3f(1., 1., 0.);
	GLVertexDD(idx); idx++; // 5
	GLVertexDD(idx); idx++; // 6
	GLVertexDD(idx); idx++; // 8
	
	GLVertexDD(idx); idx++; // 5
	GLVertexDD(idx); idx++; // 8
	GLVertexDD(idx); idx++; // 7

	// Back Fave
	glColor3f(1., 0., 1.);
	GLVertexDD(idx); idx++; // 4
	GLVertexDD(idx); idx++; // 2
	GLVertexDD(idx); idx++; // 1

	GLVertexDD(idx); idx++; // 1
	GLVertexDD(idx); idx++; // 3
	GLVertexDD(idx); idx++; // 4

	// Top Face
	glColor3f(0., 1., 1.);
	GLVertexDD(idx); idx++; // 4
	GLVertexDD(idx); idx++; // 3
	GLVertexDD(idx); idx++; // 8

	GLVertexDD(idx); idx++; // 3
	GLVertexDD(idx); idx++; // 7
	GLVertexDD(idx); idx++; // 8

	// Bottom Face
	glColor3f(1., 0., 0.);
	GLVertexDD(idx); idx++; // 2
	GLVertexDD(idx); idx++; // 6
	GLVertexDD(idx); idx++; // 1

	GLVertexDD(idx); idx++; // 1
	GLVertexDD(idx); idx++; // 6
	GLVertexDD(idx); idx++; // 5

	// SideL / TriT
	glColor3f(0., 1., 0.);
	GLVertexDD(idx); idx++; // 1
	GLVertexDD(idx); idx++; // 7
	GLVertexDD(idx); idx++; // 3

	GLVertexDD(idx); idx++; // 1
	GLVertexDD(idx); idx++; // 5
	GLVertexDD(idx); idx++; // 7

	// SideR / TriT
	glColor3f(0., 0., 1.);
	GLVertexDD(idx); idx++; // 2
	GLVertexDD(idx); idx++; // 4
	GLVertexDD(idx); idx++; // 8

	GLVertexDD(idx); idx++; // 2
	GLVertexDD(idx); idx++; // 8
	GLVertexDD(idx); idx++; // 6

	glEnd();
}