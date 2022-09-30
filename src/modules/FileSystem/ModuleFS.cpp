#include "ModuleFS.h"
#include <src/Application.h>

bool ModuleFS::Init()
{
	jsons.push_back(json_parse_file("config.json"));
	if (jsons[0] == NULL)
	{
		jsons[0] = json_value_init_object();
		App->events->RegisterEvent(std::make_shared<Event>(EventType::SAVE_CONFIG));
	}

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
