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
		glColor3f(1.5f, 0.5f, 0.5f);

		//Z
		glVertex3i(GRID_SIZE - i, 0, GRID_SIZE);
		glVertex3i(GRID_SIZE - i, 0, -GRID_SIZE);

		//X
		glVertex3i(-GRID_SIZE, 0, -GRID_SIZE + i);
		glVertex3i(GRID_SIZE, 0, -GRID_SIZE + i);
		glEnd();
	}

	SetOpenGLState(default_state);
}

// ==========================================================================================

bool ModuleRenderer3D::EnsureMesh(const uint64_t id) {
	if (meshes.find(id) == meshes.end()) {
		const NIMesh* mesh = App->fs->RetrievePValue<NIMesh>(id);
		if (mesh == nullptr) return false;
		meshes.insert(std::pair<uint64_t, GPUMesh>(id, LoadMesh(mesh)));
	}

	return true;
}

GPUMesh ModuleRenderer3D::LoadMesh(const NIMesh* mesh)
{
	GPUMesh ret;
	ret.num_vtx = mesh->vertices.size();
	glGenBuffers(1, &ret.vtx_id);
	glBindBuffer(GL_ARRAY_BUFFER, ret.vtx_id);
	glBufferData(GL_ARRAY_BUFFER, ret.num_vtx * sizeof(float3), mesh->vertices.data(), GL_STATIC_DRAW);
	if (mesh->normals.size() > 0) {
		glGenBuffers(1, &ret.norm_id);
		glBindBuffer(GL_ARRAY_BUFFER, ret.norm_id);
		glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(float3), mesh->normals.data(), GL_STATIC_DRAW);
	}
	if (mesh->uvs.size() > 0) {
		glGenBuffers(1, &ret.uvs_id);
		glBindBuffer(GL_ARRAY_BUFFER, ret.uvs_id);
		glBufferData(GL_ARRAY_BUFFER, mesh->uvs.size() * sizeof(float2), mesh->uvs.data(), GL_STATIC_DRAW);
	}
	if (mesh->indices.size() > 0) {
		ret.num_idx = mesh->indices.size();
		glGenBuffers(1, &ret.idx_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.idx_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ret.num_idx * sizeof(uint32_t), mesh->indices.data(), GL_STATIC_DRAW);
	}

	ret.material = mesh->material;

	return ret;
}

void ModuleRenderer3D::UnloadMesh(GPUMesh& mesh) {
	std::vector<uint32_t> ids;
	if (mesh.vtx_id > 0) ids.push_back(mesh.vtx_id);
	if (mesh.norm_id > 0) ids.push_back(mesh.norm_id);
	if (mesh.uvs_id > 0) ids.push_back(mesh.uvs_id);
	if (mesh.idx_id > 0) ids.push_back(mesh.idx_id);

	glDeleteBuffers(ids.size(), ids.data());

	mesh.vtx_id = mesh.norm_id = mesh.uvs_id = mesh.idx_id = 0;
}

// ==========================================================================================

bool ModuleRenderer3D::EnsureTexture(const uint64_t id) {
	if (textures.find(id) == textures.end()) {
		const Texture* tex = App->fs->RetrievePValue<Texture>(id);
		if (tex == nullptr) return false;
		textures.insert(std::pair<uint64_t, GPUTex>(id, LoadTexture(tex)));
	}

	return true;
}

GPUTex ModuleRenderer3D::LoadTexture(const Texture* tex)
{
	GPUTex ret;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ret.img_id);
	glBindTexture(GL_TEXTURE_2D, ret.img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->w, tex->h, 0, tex->format, tex->unit_type, tex->bytes.data());

	ret.w = tex->w;
	ret.h = tex->h;

	return ret;
}

void ModuleRenderer3D::UnloadTex(GPUTex& tex)
{
	glDeleteTextures(1, &tex.img_id);
	tex.img_id = 0;
}

// ==========================================================================================

bool ModuleRenderer3D::EnsureMaterial(const uint64_t id) {
	if (materials.find(id) == materials.end()) {
		const Material* mat = App->fs->RetrievePValue<Material>(id);
		if (mat == nullptr) return false;
		materials.insert(std::pair<uint64_t, GPUMat>(id, LoadMaterial(mat)));
		for (const TexRelation& tr : mat->textures)
			EnsureTexture(tr.tex_uid);
	}

	return true;
}

GPUMat ModuleRenderer3D::LoadMaterial(const Material* mat) {
	GPUMat ret;
	ret.mat = mat;

	for (const TexRelation& tr : mat->textures) {
		ret.gpu_textures.push_back(tr);
		EnsureTexture(tr.tex_uid);
	}

	return ret;
}

void ModuleRenderer3D::SetMeshMats() {
	for (auto& mesh_it : meshes) {
		GPUMesh& mesh = mesh_it.second;
		for (int i = 0; i < materials.size(); ++i) {
			if (materials[i].disk_id.uid == mesh.material.uid) {
				mesh.material.data_pos = i;
				break;
			}
		}
	}
}

GPUFBO ModuleRenderer3D::GenerateScreenFBO() {
	GPUFBO ret;
	glGenFramebuffers(1, &ret.framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, ret.framebuffer_id);
	glGenTextures(1, &ret.attachment.img_id);
	glBindTexture(GL_TEXTURE_2D, ret.attachment.img_id);
	ret.attachment.w = App->window->w;
	ret.attachment.h = App->window->h;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ret.attachment.w, ret.attachment.h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ret.attachment.img_id, 0);

	glGenRenderbuffers(1, &ret.renderbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, ret.renderbuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ret.attachment.w, ret.attachment.h);
	glBindRenderbuffer(GL_RENDERBUFFER, ret.renderbuffer_id);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ret.renderbuffer_id);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	return ret;
}