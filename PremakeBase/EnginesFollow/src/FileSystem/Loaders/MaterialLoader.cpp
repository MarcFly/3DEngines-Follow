#include "EnginePCH.h"
#include "MaterialLoader.h"

using namespace Engine;

void FileMaterial::ParseBytes(TempIfStream& disk_mem) {
	ReadStream read;
	read.SetData(disk_mem.GetData().data, disk_mem.GetData().size);

	int num_texs = 0;
	read.Get(&num_texs);
	std::vector<uint64_t> tex_ids(num_texs);
	std::vector<uint32_t> tex_types(num_texs);
	read.GetArr(tex_ids.data(), num_texs);
	read.GetArr(tex_types.data(), num_texs);

	for (int i = 0; i < num_texs; ++i)
		textures.push_back(std::pair(tex_ids[i], tex_types[i]));
}

PlainData Engine::FileMaterial::Serialize()
{
	return PlainData();
}

uint32_t Engine::MaterialLoader::ShouldILoad(const char* extension)
{
	uint32_t ret = 0;
	if (!strcmp(extension, ".material"))
		ret = 1;

	return ret;
}

FileTaker_DefaultTryLoad(MaterialLoader, FileMaterial);