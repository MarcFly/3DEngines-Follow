#include "ModuleFS.h"
#include <src/Application.h>

bool ModuleFS::Init()
{
	assimp.Init();

	jsons.push_back(json_parse_file("config.json"));
	if (jsons[0] == NULL)
	{
		jsons[0] = json_value_init_object();
		json_object_set_string(json_object(jsons[0]), "name", "config.json");
		EV_SEND_JSON_OBJ(EventType::SAVE_CONFIG, json_object(jsons[0]));
	} 

	EV_SEND_JSON_OBJ(EventType::LOAD_CONFIG, json_object(jsons[0]));

	return true;
}

#include <fstream>
uint64_t ModuleFS::RegisterFile(PlainData& data)
{
	uint64_t ret = allocs.size();
	allocs.push_back(data);
	return ret;
}

const PlainData& ModuleFS::RetrieveData(uint64_t id)
{
	return allocs[id];
}

bool ModuleFS::WriteToDisk(const char* file_path, char* data, uint64_t size)
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


	assimp.CleanUp();

	return true;
}

#include <cstring>

PlainData ModuleFS::WorkWithExtension(const char* ext, const char* path) {
	PlainData ret;
	std::ifstream file;
	file.open(path, std::ifstream::binary);
	PlainData temp;
	file.seekg(0, std::ios::end);
	temp.size = file.tellg();
	file.seekg(0, std::ios::beg);
	temp.data = new char[temp.size + 1];
	file.read((char*)temp.data, temp.size);

	if (strcmp(ext, ".fbx") == 0 || strcmp(ext, ".FBX") == 0)
		ret = assimp.ExportAssimpScene(temp);


	// Close and delete file and temp

	return ret;
}

void ModuleFS::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
	for (std::shared_ptr<Event> ev : evt_vec) {
		switch (ev->type) {
		case EventType::FILE_DROPPED:
			const char* ext = strrchr(ev->str, '.');
			PlainData data = WorkWithExtension(ext, ev->str);
			if (data.size > 0) EV_SEND_UINT64(LOAD_MESH_TO_GPU, RegisterFile(data));
			continue;
		}
	}
}


