#include "EnginePCH.h"
#include "DevILLoader.h"

#include <IL/il.h>
#include <IL/ilut.h>

using namespace Engine;

void FileTexture::Load_VRAM() {
	if (bytes.data == nullptr) {
		Engine_ERROR("Texture data is not on RAM, can't send pixels to GPU!");
	}
	else {
		if (gputex.tex_id != 0) gputex.FreeFromGPU(); // Delete and redo, safer
		gputex.Create();
		gputex.Bind();
		gputex.ApplyAttributes();
		gputex.SendToGPU(bytes.data);
	}
}

void FileTexture::Unload_VRAM() {
	if (gputex.tex_id != 0)
		gputex.FreeFromGPU();
}

void FileTexture::Unload_RAM() {
	if (bytes.data != nullptr) {
		delete bytes.data;
		bytes.data = nullptr;
		bytes.size = 0;
	}
}

void FileTexture::ParseBytes(TempIfStream& disk_mem) {
	const PlainData& d = disk_mem.GetData();

	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(deviltype, d.data, d.size);
	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
		ILinfo info;
		iluGetImageInfo(&info);
		gputex.data_format = GL_RGBA;
		gputex.var = GL_UNSIGNED_BYTE;
		gputex.var_size = 1;
		gputex.dimension_format = GL_TEXTURE_2D;
		gputex.use_format = GL_RGBA;
		gputex.w = info.Width;
		gputex.h = info.Height;

		// TODO: Texture Attributes...
		bytes.size = info.SizeOfData;
		bytes.data = new byte[bytes.size];
		memcpy(bytes.data, ilGetData(), info.SizeOfData);
	}
	ilDeleteImage(id_img);
}

void DevILLoader::OnAttach() {}
void DevILLoader::OnDetach() {}

uint32_t DevILLoader::ShouldILoad(const char* extension) {
	uint32_t ret = 0;
	if (!strcmp(extension, ".dds") || !strcmp(extension, ".DDS"))
		ret = IL_DDS;

	return ret;
}

FileVirtual* DevILLoader::TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype) {
	FileTexture* ret = new FileTexture();
	ret->deviltype = internaltype;
	ret->ParseBytes(raw_bytes);
	return ret;
}