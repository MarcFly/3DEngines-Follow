#pragma once
#include "../Module.h"

#include <src/helpers/JSON/parson.h>
#include "Importers.h"
#include "FSDataTypes.h"
#include <unordered_map>

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
	template<class T>
	const T* RetrievePValue(uint64_t id) {
		if (key_to_vec.find(id) == key_to_vec.end()) return nullptr;
		WatchedData& wd = allocs[key_to_vec.at(id)];
		//if(wd.loaded == false && wd.offload_id == UINT64_MAX)
		// 

		++wd.users;
		return (T*)wd.pd.data;
	}

	bool CleanUp();

	

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);

private:

	std::unordered_map<uint64_t, uint64_t> key_to_vec;
	std::vector<WatchedData> allocs;
};

std::vector<WatchedData> TryLoadFromDisk(const char* path);
bool WriteToDisk(const char* file_path, char* data, uint64_t size);
