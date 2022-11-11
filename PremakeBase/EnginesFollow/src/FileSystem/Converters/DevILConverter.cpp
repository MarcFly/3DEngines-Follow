#include "EnginePCH.h"
#include "DevILConverter.h"
#include "Events/DefaultEvents.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

using namespace Engine;

void DevILConverter::OnAttach() {
	ilInit();
	iluInit();
}

void DevILConverter::OnDetach() {

}
	
uint32_t DevILConverter::ShouldILoad(const char* extension) {
	uint32_t ret = 0;
	if (!strcmp(extension, ".png") || !strcmp(extension, ".PNG"))
		ret = IL_PNG;
	else if (!strcmp(extension, ".tiff") || !strcmp(extension, ".TIFF") || !strcmp(extension, ".TIF") || !strcmp(extension, ".tif"))
		ret = IL_TIF;
	else if (!strcmp(extension, ".tga") || !strcmp(extension, ".TGA"))
		ret = IL_TGA;

	return ret;
}

FileVirtual* DevILConverter::TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype) {
	const PlainData& d = raw_bytes.GetData();
	PlainData temp;

	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(internaltype, d.data, d.size);

	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
		ILinfo info;
		iluGetImageInfo(&info);
		if (info.Origin != IL_ORIGIN_LOWER_LEFT)
			iluFlipImage();

		temp.size = ilSaveL(IL_DDS, nullptr, 0);
		temp.data = new byte[temp.size];
		ilSaveL(IL_DDS, temp.data, info.SizeOfData);
	}

	ilDeleteImage(id_img);

	static char filepath[256];
	snprintf(filepath, sizeof(filepath), "%s/Assets/Textures/%s.dds", FS::execpath, FileNameExt(raw_bytes.path.c_str()));
	FS::WriteToDisk(filepath, temp);

	FS::TryLoadFile(filepath);

	return nullptr;
}