#pragma once
#include "../Module.h"

#include <src/helpers/JSON/parson.h>
#include "Importers.h"
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
	//uint64_t RegisterFile(WatchedData& data);
	const PlainData& RetrieveData(uint64_t id);

	bool WriteToDisk(const char* file_path, char* data, uint64_t size);

	bool CleanUp();

	std::vector<WatchedData> ModuleFS::TryLoadFromDisk(const char* path);

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);

private:
	AssimpImporter assimp;
	std::vector<WatchedData> allocs;
};