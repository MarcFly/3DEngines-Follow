#include "EnginePCH.h"
#include "Render.h"

using namespace Engine;

//==================================================================
// VTX_BUF INITIALIZATION
//==================================================================

uint16_t VTX_Buf::VertSize() {
	vtx_size = 0;
	for (VertAttrib& a : attributes)
		vtx_size += a.var_size * a.num_components;

	return vtx_size;
}

void VTX_Buf::SetInterleave() {
	VertSize();
	uint16_t offset = 0;
	for (VertAttrib& a : attributes) {
		a.offset = offset;
		offset += a.var_size * a.num_components;
	}
}

void VTX_Buf::SetBlock() {
	VertSize();
	uint16_t offset = 0;
	for (VertAttrib& a : attributes) {
		a.offset = offset;
		offset += a.var_size * a.num_components * vtx_num;
	}
}

uint16_t VTX_Buf::GetAttributeSize(const char* name) {
	for (VertAttrib& a : attributes) {
		if (!strcmp(a.name, name))
			return a.var_size * a.num_components;
	}

	return 0;
}

uint16_t VTX_Buf::GetAttributeSize(int id) {
	if (id >= attributes.size()) return 0;
	return attributes[id].var_size * attributes[id].num_components;
}

#include <glad/glad.h>

void VTX_Buf::BindLocations() {
	Bind();
	VertSize();
	for (int i = 0; i < attributes.size(); ++i) {
		VertAttrib& a = attributes[i];
		a.id = i;
		// Enable Attribute!
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
			a.num_components,
			a.var,
			a.normalize,
			vtx_size,
			(void*)a.offset);
	}
}

//==================================================================
// VTX_BUF MEMORY MANAGEMENT
//==================================================================

void VTX_Buf::Create() {
	glGenBuffers(1, &vtx_id);
	glGenVertexArrays(1, &vao);
}

void VTX_Buf::SendToGPU(void* vtx_data) {
	glBufferData(GL_ARRAY_BUFFER, vtx_size * vtx_num, vtx_data, draw_mode);
}

void VTX_Buf::FreeFromGPU() {
	glDeleteBuffers(1, &vtx_id);
}

//==================================================================
// VTX_BUF USAGE
//==================================================================

void VTX_Buf::Bind() {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vtx_id);	
}

void VTX_Buf::Draw() {
	Bind();
	glDrawArrays(draw_config, NULL, vtx_num);
}

//==================================================================
// MESH MEMORY MANAGEMENT
//==================================================================

void Mesh::Create() {
	if (idx_id != 0) FreeFromGPU();
	vtx.Create();
	glGenBuffers(1, &idx_id);
}

void Mesh::SendToGPU(void* vtx_data, void* idx_data) {
	vtx.SendToGPU(vtx_data);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_var_size * idx_num, idx_data, vtx.draw_mode);
}

void Mesh::FreeFromGPU() {
	vtx.FreeFromGPU();
	glDeleteBuffers(1, &idx_id);
}

//==================================================================
// MESH USAGE
//==================================================================

void Mesh::Bind() {
	vtx.Bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_id);
}

void Mesh::Draw() {
	Bind();
	glDrawElements(vtx.draw_config, idx_num, idx_var, NULL);
}

//==================================================================
// MULTIPLE BUFFER AND MESH GENERATION
//==================================================================

void RenderAPI::GenBuffers(const uint32_t num, uint32_t* id_arr) {
	glGenBuffers(num, id_arr);
}

void RenderAPI::GenVAOs(const uint32_t num, uint32_t* vao_arr) {
	glGenVertexArrays(num, vao_arr);
}