#pragma once
#include "../Module.h"

#include <src/helpers/JSON/parson.h>
#include "Importers/AssimpImporter.h"
#include "FSDataTypes.h"

class ModuleFS : public Module {
	std::vector<JSON_Value*> jsons;
public:
	ModuleFS() : Module("filesystem", true) {};
	~ModuleFS() {};

	bool Init();

	// TODO: Give the ModuleFS some data to be registered for Resource Management purposes
	// TODO: Basic Struct for a File - char* data, uint64_t size
	// TODO: Watched Struct File - File, load checker, load function, unload function,...
	uint64_t RegisterFile(PlainData& data);
	const PlainData& RetrieveData(uint64_t id);

	bool WriteToDisk(const char* file_path, char* data, uint64_t size);

	bool CleanUp();

	PlainData ModuleFS::WorkWithExtension(const char* ext, const char* path);

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);

private:
	AssimpImporter assimp;
	std::vector<PlainData> allocs;
};