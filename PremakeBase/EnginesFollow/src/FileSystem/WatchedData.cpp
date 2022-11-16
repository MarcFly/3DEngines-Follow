#include "EnginePCH.h"
#include "FS.h"
using namespace Engine;

void WatchedData::CheckNecessity() {
	const size_t num_users = users.size();
	if (num_users > 0 && !loaded) {
		mem = FS::TryLoad(path.c_str());
		loaded = true;
	}
	if (num_users == 0 && last_check_ts > FS::unload_bucket_ms) {
		last_check_ts = 0;
		Delete();
		loaded = false;
	}

	// TODO: increment last_check_ts
}

void WatchedData::AddUser() {
	//TODO: Get actual userid
	users.push_back(0);
	CheckNecessity();
}

void WatchedData::RemoveUser() {
	//TODO: Get actual userid
	users.pop_back();
}

#include "DefaultFileTypes.h"

void WatchedData::Load(TempIfStream& disk_mem) {
	JSONVWrap meta;
	meta.Load(disk_mem);
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

void WatchedData::Load_FromDisk() {
	mem = FS::TryLoad(path.c_str());
	if (mem != nullptr)
		loaded = true;
}