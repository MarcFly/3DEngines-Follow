#include "ModuleFS.h"
#include <src/Application.h>

#include "Converters.h"

#include <DevIL/include/IL/il.h>
#include <DevIL/include/IL/ilu.h>
#pragma comment(lib, "DevIL/libx86/DevIL.lib")
#pragma comment(lib, "DevIL/libx86/ILU.lib")

static char execpath[512];
static size_t execpath_len;

const char* ModuleFS::GetExecPath() { return execpath; }

bool ModuleFS::Init()
{
	execpath_len = GetCurrentDirectory(512, execpath);
	(*strrchr(execpath, '\\')) = '\0';

	char temp[1024];
	sprintf(temp, "mkdir -p %s\\Game\\Assets\\Prefabs", execpath);
	system(temp);
	sprintf(temp, "mkdir -p %s\\Game\\Assets\\Materials", execpath);
	system(temp);
	sprintf(temp, "mkdir -p %s\\Game\\Assets\\Textures", execpath);
	system(temp);
	sprintf(temp, "mkdir -p %s\\Game\\Assets\\Meshes", execpath);
	system(temp);

	InitConverters();

	jsons.push_back(json_parse_file("config.json"));

	
	if (jsons.back() == NULL)
	{
		jsons.back() = json_value_init_object();
		json_object_set_string(json_object(jsons.back()), "name", "config.json");
		App->Save(json_object(jsons.back()));
	} 
	
	App->Load(json_object(jsons.back()));

	ilInit();
	iluInit();

	return true;
}

#include <fstream>



bool WriteToDisk(const char* file_path, char* data, uint64_t size)
{
	std::ofstream write_file;
	write_file.open(file_path, std::ios::binary);
	write_file.write(data, size);
	write_file.close();

	return false;
}

bool ModuleFS::CleanUp() {
	for (JSON_Value* json : jsons) {
		json_serialize_to_file(json, json_object_get_string(json_object(json), "name"));
		json_value_free(json);
	}

	CleanUpConverters();

	return true;
}

#include <cstring>

// TODO: Send the base filepath from which a scene/mesh was loaded!
// That is so that subdata that depends on knowing that can be loaded

void TryLoad_WithParentPath(const char* path, const char* parent_path, TempIfStream& stream) {
	if (stream.GetData().size == 0) {
		if (parent_path == nullptr) return;
		// Try finding the file base path, find data in that base path

		stream.path = parent_path + std::string(FileName(path));
		stream.TryLoad(stream.path.c_str());
	}
}

std::vector<WatchedData> TryLoadFromDisk(const char* path, const char* parent_path) {
	std::vector<WatchedData> ret;
	TempIfStream file(path);
	if (file.GetData().size == 0) TryLoad_WithParentPath(path, parent_path, file);

	TryConvert(file, path);
		
	return ret;
}