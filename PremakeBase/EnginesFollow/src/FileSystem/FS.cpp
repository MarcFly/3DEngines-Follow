#include "EnginePCH.h"
#include "FS.h"
#include "Events/DefaultEvents.h"
#include "Core/Logger.h"
#include "../Globals.h"
#include "Converters/AssimpConverter.h"
#include "Converters/DevILConverter.h"
#include "Loaders/DevILLoader.h"
#include "Loaders/MaterialLoader.h"
#include "Loaders/MeshLoader.h"

using namespace Engine;

std::vector<FileTaker*> FS::filetakers;
std::unordered_map<uint64_t, WatchedData> FS::allocs;
double FS::unload_bucket_ms = 10000;

FileTaker::FileTaker() {}


DEF_STATIC_MEMBER_EV_FUN(FilesDropped, FS, TryLoadFiles_EventFun) {
	for (int i = 0; i < ev->count; ++i) {
		TryLoadFile(ev->filepaths[i], nullptr);
	}
}}

char FS::execpath[512];
size_t FS::execpath_len;

void FS::Init() {
	RegisterFiletaker<AssimpConverter>();
	RegisterFiletaker<DevILConverter>();
	RegisterFiletaker<DevILLoader>();
	RegisterFiletaker<MaterialLoader>();
	RegisterFiletaker<MeshLoader>();
	execpath_len = GetCurrentDirectoryA(512, execpath);
	(*strrchr(execpath, '\\')) = '\0';

	char temp[1024];
	sprintf(temp, "mkdir -p %s\\Assets\\Prefabs", execpath);
	system(temp);
	sprintf(temp, "mkdir -p %s\\Assets\\Materials", execpath);
	system(temp);
	sprintf(temp, "mkdir -p %s\\Assets\\Textures", execpath);
	system(temp);
	sprintf(temp, "mkdir -p %s\\Assets\\Meshes", execpath);
	system(temp);
	sprintf(temp, "mkdir -p %s\\Assets\\Metafiles", execpath);
	system(temp);

	Events::Subscribe<FilesDropped>(TryLoadFiles_EventFun);
}

void FS::Close() {
	allocs.clear();

	for (FileTaker* ft : filetakers)
		delete ft;
}

const char* FileNameExt(const char* path) {
	const char* parent_path_fw = strrchr(path, '/');
	const char* parent_path = (parent_path_fw != nullptr) ? parent_path_fw : strrchr(path, '\\');
	return (parent_path != nullptr) ? &(parent_path[1]) : path; // If // or \ not found, already a filename
}

void FileName(const char* path, char* out) {
	const char* name_ext = FileNameExt(path);
	if (name_ext == nullptr) return;
	int len = strlen(name_ext);
	char temps[64];
	if (len > 64) return;
	memcpy(out, name_ext, len);
	
	char* pointer = strchr(out, '.');
	if (pointer != nullptr) 
		*pointer = '\0';
}

std::string ParentPath(const std::string & path) {
	const char* filename = FileNameExt(path.c_str());
	size_t filename_len = strlen(filename);
	return path.substr(0, path.length() - filename_len);
}

uint64_t FS::TryLoadFile(const char* path, const char* parent_path) {
	uint64_t ret = UINT64_MAX;
	Engine_INFO("TryLoadFiles: {}", path);
	FileVirtual* push = TryLoad(path, parent_path);
	// TODO: Load or create metadata
	if(push != nullptr) {
		static char metapath[256];
		snprintf(metapath, sizeof(metapath), "%s/Assets/Metafiles/%s.jsonmeta", execpath, FileNameExt(path));
		TempIfStream temp_meta((const char*)metapath, nullptr);
		WatchedData pushreal(push);
		pushreal.path = path;
		if (temp_meta.GetData().data != nullptr) pushreal.ParseBytes(temp_meta);
		else {
			pushreal.path = std::string(path); // All should be loaded from assets!
			WriteToDisk(metapath, pushreal.Serialize());
		}
		uint64_t tmpid = pushreal.id;
		auto retpair = allocs.emplace(tmpid, pushreal);
		ret = pushreal.id;
	}
	
	return ret;
}

FileVirtual* FS::TryLoad(const char* path, const char* parent_path) {
	const char* extension = strrchr(path, '.');

	FileVirtual* ret;
	for (FileTaker* ft : filetakers) {
		uint32_t internal_type = ft->ShouldILoad(extension);
		if (internal_type > 0) {
			TempIfStream temp(path, parent_path);
			ret = ft->TryLoad(temp, internal_type);
			if (ret != nullptr)
				return ret;
		}
	}
	return nullptr;
}

void FS::WriteToDisk(const char* path, const PlainData& data) {
	std::ofstream write_file;
	write_file.open(path, std::ios::binary);
	write_file.write((const char*)data.data, data.size);
	write_file.close();
}

uint64_t FS::FindIDByName(const char* str) {
	const char* tmp = FileNameExt(str);
	for (auto& it : allocs) {
		if(it.second.path.find(tmp) != std::string::npos)
			return it.first;
	}

	return UINT64_MAX;
}