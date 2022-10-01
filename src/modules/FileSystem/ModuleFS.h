#pragma once
#include "../Module.h"

#include <src/helpers/JSON/parson.h>

class ModuleFS : public Module {
	std::vector<JSON_Value*> jsons;
public:
	ModuleFS() : Module("filesystem", true) {};
	~ModuleFS() {};

	bool Init();

	bool WriteToDisk(const char* file_path, char* data, uint64_t size);

	bool CleanUp();
};