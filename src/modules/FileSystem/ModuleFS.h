#pragma once
#include "../Module.h"

#include <src/helpers/JSON/parson.h>
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
		if (allocs.find(id)->first != id) return nullptr;
		WatchedData& wd = allocs.at(id);
		//if(wd.loaded == false && wd.offload_id == UINT64_MAX)
		// 

		++wd.users;
		return (T*)wd.pd.data;
	}

	bool CleanUp();

	

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);
	const char* GetExecPath();

private:
	typedef std::pair<uint64_t, WatchedData> allocpair;
	std::unordered_map<uint64_t, WatchedData> allocs;
};

std::vector<WatchedData> TryLoadFromDisk(const char* path, const char* parent_path = nullptr);
bool WriteToDisk(const char* file_path, char* data, uint64_t size);
