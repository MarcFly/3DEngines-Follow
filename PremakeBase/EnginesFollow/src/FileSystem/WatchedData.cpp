#include "EnginePCH.h"
#include "FS.h"
using namespace Engine;

void WatchedData::Delete_RAM() {
	if (mem != nullptr) {
		mem->Unload_RAM();
		loaded_ram = false;
		delete mem;
	}
}

void WatchedData::CheckNecessity() {
	if (ram_users > 0 && !loaded_ram) {
		mem = FS::TryLoad(path.c_str());
		loaded_ram = true;
	}
	if (vram_users > 0 && !loaded_vram) {
		mem->Load_VRAM();
		loaded_vram = true;
	}

	if (last_check_ts > FS::unload_bucket_ms) {
		if (ram_users == 0 && loaded_ram) {
			Delete_RAM();
			loaded_ram = false;
		}
		if (vram_users == 0 && loaded_vram) {
			mem->Unload_VRAM();
			loaded_vram = false;
		}

		// TODO: Add ms since last check
	}
}

void WatchedData::AddUser() {
	++ram_users;
	CheckNecessity();
}

void WatchedData::RemoveUser() {
	--ram_users;
}

#include "DefaultFileTypes.h"

void WatchedData::ParseBytes(TempIfStream& disk_mem) {
	JSONVWrap meta;
	meta.ParseBytes(disk_mem);
	JSON_Object* obj = json_object(meta.value);
	id = json_object_get_u64(obj, "id");
	path = std::string(json_object_get_string(obj, "relative_path")); // When loaded, full path will be the one of creation...
}

PlainData WatchedData::Serialize() {
	JSONVWrap meta;
	
	meta.value = json_value_init_object();
	JSON_Object* obj = json_object(meta.value);
	json_object_set_u64(obj, "id", id);
	const char* rel_path = &path.c_str()[path.find("/Assets")];
	json_object_set_string(obj, "relative_path", &path.c_str()[path.find("/Assets")]);
	json_object_set_string(obj, "last_full_path", path.c_str());
	// TODO: project folder structures... for ui/organization purposes
	PlainData ret = meta.Serialize();
	
	return ret;

}