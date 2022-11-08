#pragma once
#include "../Module.h"

#include <src/helpers/JSON/parson.h>
#include "FSDataTypes.h"
#include <unordered_map>
typedef std::pair<uint64_t, WatchedData> allocpair;


class ModuleFS : public Module {
	std::vector<JSON_Value*> jsons;
public:
	ModuleFS() : Module("filesystem", true) {};
	~ModuleFS() {};

	bool Init();

	bool CleanUp();


	const char* GetExecPath();
};

void TryLoad_WithParentPath(const char* path, const char* parent_path, TempIfStream& stream);

std::vector<WatchedData> TryLoadFromDisk(const char* path, const char* parent_path = nullptr);
bool WriteToDisk(const char* file_path, char* data, uint64_t size);
