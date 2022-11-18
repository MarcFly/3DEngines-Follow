#include "EnginePCH.h"
#include "DevILLoader.h"
#include "LoaderFileTypes.h"

#include <IL/il.h>
#include <IL/ilut.h>

using namespace Engine;

void FileTexture::Unload() {
	gputex.FreeFromGPU();
}

void FileTexture::Load(TempIfStream& disk_mem) {

	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(deviltype, disk_mem.bytes.get(), disk_mem.size);
	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
		ILinfo info;
		iluGetImageInfo(&info);
		
		w = info.Width;
		h = info.Height;

		// TODO: Texture Attributes...

		gputex.data_format = info.Format;
		gputex.var = GL_UNSIGNED_BYTE;
		gputex.var_size = 1;
		gputex.dimension_format = info.Type;
		gputex.use_format = info.Format;
		gputex.w = info.Width;
		gputex.h = info.Height;

		// TODO: Attributes from metadata...

		gputex.Create();
		gputex.Bind();
		gputex.ApplyAttributes();
		gputex.SendToGPU(ilGetData());
	}
	else {
		Engine_ERROR("Failed to Load Texture: {}", disk_mem.path.c_str());
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

std::shared_ptr<FileVirtual> DevILLoader::TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype) {
	std::shared_ptr<FileTexture> ret(new FileTexture());
	ret->deviltype = internaltype;
	ret->Load(raw_bytes);
	return ret;
}