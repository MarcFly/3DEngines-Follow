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
	if (jsons[0] == NULL)
	{
		jsons[0] = json_value_init_object();
		json_object_set_string(json_object(jsons[0]), "name", "config.json");
		EV_SEND_JSON_OBJ(EventType::SAVE_CONFIG, json_object(jsons[0]));
	} 

	EV_SEND_JSON_OBJ(EventType::LOAD_CONFIG, json_object(jsons[0]));

	ilInit();
	iluInit();

	return true;
}

#include <fstream>
//uint64_t ModuleFS::RegisterFile(PlainData& data)
//{
//	uint64_t ret = allocs.size();
//	allocs.push_back(data);
//	return ret;
//}

const PlainData& ModuleFS::RetrieveData(uint64_t id)
{
	WatchedData& wd = allocs[id];
	//if(wd.loaded == false && wd.offload_id == UINT64_MAX)
	// 

	++wd.users;
	return wd.pd;
}


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

	for (auto& it : allocs) {
		WatchedData& data = it.second;
		if (data.pd.data != nullptr)
			delete data.pd.data;
		data.pd.data = nullptr;
	}

	CleanUpConverters();

	return true;
}

#include <cstring>

// TODO: Send the base filepath from which a scene/mesh was loaded!
// That is so that subdata that depends on knowing that can be loaded

std::vector<WatchedData> TryLoadFromDisk(const char* path, const char* parent_path) {
	std::vector<WatchedData> ret;
	TempIfStream file(path);
	if (file.GetData().size == 0) {
		if (parent_path == nullptr) return ret;
		// Try finding the file base path, find data in that base path

		file.path = parent_path + std::string(FileName(path));
		file.TryLoad(file.path.c_str());
		if(file.GetData().size == 0) return ret;
	}

	ret = TryConvert(file, path);
	if (ret.size() > 0) {
		for (const WatchedData& converted : ret) {
			WriteToDisk(converted.path, converted.pd.data, converted.pd.size);
		}
	}
	if(ret.size() == 0)
		ret = TryImport(file, path);
		
	return ret;
}

void ModuleFS::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
	std::vector<WatchedData> unregistered;
	// TODO: Data to unload vector

	for (std::shared_ptr<Event> ev : evt_vec) {
		std::vector<WatchedData> data;
		switch (ev->type) {
		case EventType::FILE_DROPPED:
			data = TryLoadFromDisk(ev->str);
			AppendVec(unregistered, data);
			continue;
		case EventType::USER_UNLOADED_DATA:
			--allocs[ev->uint64].users;
			continue;
		}
	}

	uint64_t num_registered = allocs.size();
	int num_unregistered = unregistered.size();
	for (int i = 0; i < num_unregistered; ++i){
		const WatchedData& unreg_data = unregistered[i];
		allocs.insert(allocpair(unregistered[i].uid, unregistered[i]));
	}

	unregistered.clear();
}