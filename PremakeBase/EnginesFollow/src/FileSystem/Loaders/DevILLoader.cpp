#include "EnginePCH.h"
#include "DevILLoader.h"

#include <IL/il.h>
#include <IL/ilut.h>

using namespace Engine;

void FileTexture::Unload() {
	if (gputex.tex_id != 0)
		gputex.FreeFromGPU();
}

void FileTexture::Load(TempIfStream& disk_mem) {

	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(deviltype, disk_mem.bytes.get(), disk_mem.size);
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
		bytes.bytes = std::shared_ptr<byte[]>(new byte[bytes.size]);
		memcpy(bytes.bytes.get(), ilGetData(), info.SizeOfData);
	}
	ilDeleteImage(id_img);

	if (bytes.bytes == nullptr) {
		Engine_ERROR("Texture data is not on RAM, can't send pixels to GPU!");
	}
	else {
		if (gputex.tex_id != 0) gputex.FreeFromGPU(); // Delete and redo, safer
		gputex.Create();
		gputex.Bind();
		gputex.ApplyAttributes();
		gputex.SendToGPU(bytes.bytes.get());
	}
}

void DevILLoader::OnAttach() {}
void DevILLoader::OnDetach() {}

uint32_t DevILLoader::ShouldILoad(const char* extension) {
	uint32_t ret = 0;
	if (!strcmp(extension, ".dds") || !strcmp(extension, ".DDS"))
		ret = IL_DDS;

	return ret;
}

std::shared_ptr<FileVirtual> DevILLoader::TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype) {
	std::shared_ptr<FileTexture> ret(new FileTexture());
	ret->deviltype = internaltype;
	ret->Load(raw_bytes);
	return ret;
}