#include "ModuleFS.h"
#include <src/Application.h>

bool ModuleFS::Init()
{
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

	return true;
}