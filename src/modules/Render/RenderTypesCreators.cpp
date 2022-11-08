#pragma once
#include "ModuleRenderer3D.h"
#include "RendererTypes.h"
#include <src/Application.h>

#include <libs/glew/include/GL/glew.h>
#include <libs\SDL\include\SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#define GRID_SIZE 10

void ModuleRenderer3D::RenderGrid() const
{
	SetOpenGLState(grid_state);
	for (int i = 0; i < GRID_SIZE * 2 + 1; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.5f, 0.5f);

		//Z
		glVertex3i(GRID_SIZE - i, 0, GRID_SIZE);
		glVertex3i(GRID_SIZE - i, 0, -GRID_SIZE);

		//X
		glVertex3i(-GRID_SIZE, 0, -GRID_SIZE + i);
		glVertex3i(GRID_SIZE, 0, -GRID_SIZE + i);
		glEnd();
	}
	glColor3f(1.f, 1.f, 1.f);
	SetOpenGLState(default_state);
}

// ==========================================================================================

GPUMesh NIMesh::LoadToGPU()
{
	GPUMesh ret;
	ret.num_vtx = vertices.size();
	glGenBuffers(1, &ret.vtx_id);
	glBindBuffer(GL_ARRAY_BUFFER, ret.vtx_id);
	glBufferData(GL_ARRAY_BUFFER, ret.num_vtx * sizeof(float3), vertices.data(), GL_STATIC_DRAW);
	if (normals.size() > 0) {
		glGenBuffers(1, &ret.norm_id);
		glBindBuffer(GL_ARRAY_BUFFER, ret.norm_id);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float3), normals.data(), GL_STATIC_DRAW);
	}
	if (uvs.size() > 0) {
		glGenBuffers(1, &ret.uvs_id);
		glBindBuffer(GL_ARRAY_BUFFER, ret.uvs_id);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float2), uvs.data(), GL_STATIC_DRAW);
	}
	if (indices.size() > 0) {
		ret.num_idx = indices.size();
		glGenBuffers(1, &ret.idx_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.idx_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ret.num_idx * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	}

	return ret;
}

void GPUMesh::UnloadFromGPU() {
	std::vector<uint32_t> ids;
	if (vtx_id > 0) ids.push_back(vtx_id);
	if (norm_id > 0) ids.push_back(norm_id);
	if (uvs_id > 0) ids.push_back(uvs_id);
	if (idx_id > 0) ids.push_back(idx_id);

	glDeleteBuffers(ids.size(), ids.data());

	vtx_id = norm_id = uvs_id = idx_id = 0;
}

void GPUMesh::Bind() {
					
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vtx_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	if (norm_id != 0) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, norm_id);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}
	if (uvs_id != 0) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_id);
}

void GPUMesh::Draw() {
	if (idx_id > 0)
		glDrawElements(GL_TRIANGLES, num_idx, GL_UNSIGNED_INT, nullptr);
	else
		glDrawArrays(GL_TRIANGLES, 0, num_vtx);
}

// ==========================================================================================

GPUTex Texture::LoadToGPU() const
{
	GPUTex ret;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ret.img_id);
	glBindTexture(GL_TEXTURE_2D, ret.img_id);
	// TODO: change parameters separately
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, unit_type, bytes.data());

	ret.w = w;
	ret.h = h;

	return ret;
}

void GPUTex::UnloadFromGPU()
{
	glDeleteTextures(1, &img_id);
	img_id = 0;
}

void GPUTex::Bind() {
	glBindTexture(GL_TEXTURE_2D, img_id);
}

// ==========================================================================================

GPUMat Material::LoadToGPU() {
	GPUMat ret;

	for (const Texture& tr : textures_data)
		ret.texture_use.push_back(tr.LoadToGPU());

	// TODO: Create Uniforms from the properties

	return ret;
}

void GPUMat::UnloadFromGPU() {
	for (GPUTex& tr : texture_use)
		tr.UnloadFromGPU();
	texture_use.clear();
}

void GPUMat::Bind() {
	int baset = GL_TEXTURE0;

	for (GPUTex& tr : texture_use) {
		glEnable(baset);
		tr.Bind();
		++baset;
	}
}

// ==========================================================================================

void GPUFBO::Create(int w, int h) {
	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glGenTextures(1, &attachment.img_id);
	glBindTexture(GL_TEXTURE_2D, attachment.img_id);
	attachment.w = w;
	attachment.h = h;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, attachment.w, attachment.h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, attachment.img_id, 0);

	glGenRenderbuffers(1, &renderbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, attachment.w, attachment.h);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_id);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void GPUFBO::Destroy() {
	glDeleteFramebuffers(1, &framebuffer_id);
	glDeleteTextures(1, &attachment.img_id);
	glDeleteRenderbuffers(1, &renderbuffer_id);
}

void GPUFBO::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
}