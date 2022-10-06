#include "ModuleFS.h"
#include <src/Application.h>

#include <DevIL/include/IL/il.h>
#include <DevIL/include/IL/ilu.h>
#pragma comment(lib, "DevIL/libx86/DevIL.lib")
#pragma comment(lib, "DevIL/libx86/ILU.lib")

bool ModuleFS::Init()
{
	AssimpInit();

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
	write_file.open(file_path, std::ofstream::binary);
	write_file.write(data, size);
	write_file.close();

	return false;
}

bool ModuleFS::CleanUp() {
	for (JSON_Value* json : jsons) {
		json_serialize_to_file(json, json_object_get_string(json_object(json), "name"));
		json_value_free(json);
	}

	for (WatchedData& data : allocs) {
		if (data.pd.data != nullptr)
			delete data.pd.data;
		data.pd.data = nullptr;
	}

	AssimpCleanUp();

	return true;
}

#include <cstring>

// TODO: Send the base filepath from which a scene/mesh was loaded!
// That is so that subdata that depends on knowing that can be loaded

std::vector<WatchedData> TryLoadFromDisk(const char* path) {
	std::vector<WatchedData> ret;
	TempIfStream file(path);
	const char* ext = strrchr(path, '.');
	uint32_t tex_type = 0;
	if (strcmp(ext, ".fbx") == 0 || strcmp(ext, ".FBX") == 0)
		ret = ExportAssimpScene(file.GetData()); // Assimp Scene never saved to memory
	if ((tex_type = ExtensionToDevILType(ext)) != 0)
	{
		ret.push_back(WatchedData());
		WatchedData& curr = ret.back();
		curr.pd = ImportDevILTexture(file.GetData(), tex_type);
		curr.event_type = LOAD_TEX_TO_GPU;
		curr.uid = PCGRand();
	}
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
			//unregistered.insert(unregistered.end(), std::make_move_iterator(data.begin()), std::make_move_iterator( data.end()));
			//data.erase(data.begin(), data.end());
			//data.clear();
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
		if(unreg_data.event_type > 0) EV_SEND_UINT64((EventType)unreg_data.event_type, unreg_data.uid);
		// TODO: Prepare unregistered data for being put in the alloc vector
		key_to_vec.insert(std::pair<uint64_t, uint64_t>( unreg_data.uid, num_registered + i));
	}
	AppendVec(allocs, unregistered);
	//allocs.insert(allocs.end(), std::make_move_iterator(unregistered.begin()), std::make_move_iterator(unregistered.end()));
	//unregistered.erase(unregistered.begin(), unregistered.end());
	//unregistered.clear();
}


