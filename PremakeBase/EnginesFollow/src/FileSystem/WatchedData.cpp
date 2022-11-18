#include "EnginePCH.h"
#include "FS.h"
using namespace Engine;

void WatchedData::CheckNecessity() {
	if (users.size() > 0 && !mem) {
		mem = FS::TryLoad(path.c_str());
		mem->LoadMeta(json_object(json_object_get_value(json_object(meta->value), "meta")));
		loaded = true;
	}
	if (users.size() == 0 && last_check_ts > FS::unload_bucket_ms) {
		last_check_ts = 0;
		Delete();
		loaded = false;
	}

	// TODO: increment last_check_ts
}

void WatchedData::AddUser(const uint64_t& userid) {
	users.insert(userid);
	CheckNecessity();
}

void WatchedData::RemoveUser(const uint64_t& userid) {
	users.erase(userid);
}

#include "DefaultFileTypes.h"

void WatchedData::Load(TempIfStream& disk_mem) {
	if (meta != nullptr) meta.reset();
	meta = std::make_shared<JSONVWrap>();
	meta->Load(disk_mem);
	JSON_Object* obj = json_object(meta->value);
	id = json_object_get_u64(obj, "id");
	path = std::string(json_object_get_string(obj, "last_full_path")); // When loaded, full path will be the one of creation...
	//TODO: Project folder structures...
}

PlainData WatchedData::Serialize() {
	if (meta == nullptr) meta = std::make_shared<JSONVWrap>();
	meta->value = json_value_init_object();
	JSON_Object* obj = json_object(meta->value);
	json_object_set_u64(obj, "id", id);
	const char* rel_path = &path.c_str()[path.find("Assets")];
	json_object_set_string(obj, "relative_path", &path.c_str()[path.find("Assets")]);
	json_object_set_string(obj, "last_full_path", path.c_str());
	// TODO: project folder structures... for ui/organization purposes
	
	AddUser(UINT64_MAX);
	JSON_Value* mem_v = json_value_init_object();
	mem->SaveMeta(json_object(mem_v));
	json_object_set_value(obj, "meta", mem_v);
	RemoveUser(UINT64_MAX);
	return meta->Serialize();

}

void WatchedData::Load_FromDisk() {
	mem = FS::TryLoad(path.c_str());
	if (mem != nullptr)
		loaded = true;
}