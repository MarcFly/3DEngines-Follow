#include "EnginePCH.h"
#include "glad/glad.h"
#include "Render.h"


using namespace Engine;

TexAttrib::TexAttrib(const char* name, int32_t _id, float* value, int _num_values) : 
	name(name), id(_id), isfloat(true), num_values(_num_values) {
	fvalues = new float[num_values];
	memcpy(fvalues, value, num_values * sizeof(float));
}

TexAttrib::TexAttrib(const char* name, int32_t _id, int* value, int _num_values) : 
	name(name), id(_id), isfloat(false), num_values(_num_values) {
	ivalues = new int[num_values];
	memcpy(ivalues, value, num_values * sizeof(int));
}

TexAttrib::~TexAttrib() {
	if (isfloat)
		delete fvalues;
	else
		delete ivalues;
}

void Texture::AddAttributeF(const char* name, int32_t id, float value) {
	attributes.push_back(TexAttrib(name, id, &value));
}

void Texture::AddAttributeI(const char* name, int32_t id, int value) {
	attributes.push_back(TexAttrib(name, id, &value));
}

void Texture::ApplyAttributes() {
	for (TexAttrib& a : attributes) {
		if (a.isfloat)
			glTexParameterfv(dimension_format, a.id, a.fvalues);
		else
			glTexParameteriv(dimension_format, a.id, a.ivalues);
	}
}

void Texture::Create() {
	glGenTextures(1, &tex_id);
}

void Texture::GenMipmaps() {
	//glGenerateTextureMipmap(tex_id);
}

void Texture::SendToGPU(void* pixels, const int mipmap_level) {
	glTexImage2D(dimension_format, mipmap_level, use_format, w, h, 0, data_format, var, pixels);
}

void Texture::FreeFromGPU() {
	glDeleteTextures(1, &tex_id);
}

void Texture::Bind() {
	glBindTexture(dimension_format, tex_id);
}