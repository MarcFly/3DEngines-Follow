#include "Converters.h"

// Assimp
#include <libs/assimp/cimport.h>
#include <libs/assimp/postprocess.h>
#include <libs/assimp/scene.h>
#include <libs/assimp/LogStream.hpp>
#pragma comment(lib, "libs/Assimp/libx86/assimp-vc142-mt.lib")

struct aiLogStream stream;

std::vector<WatchedData> TryConvert(const TempIfStream& file, const char* path) {
	std::vector<WatchedData> ret;
	const char* ext = strrchr(path, '.');

	uint32_t tex_type = 0;
	if (strcmp(ext, ".fbx") == 0 || strcmp(ext, ".FBX") == 0)
		ret = ConvertAssimpScene(file); // Assimp Scene never saved to memory
	else if(ExtensionToDevILType_Convert(ext) != 0){
		WatchedData tex_try = TryConvertTexture(file);
		if (tex_try.pd.size != 0) ret.push_back(tex_try);
	}
	return ret;
}

void PrepFromFile(WatchedData& data, TempIfStream& file) {
	data.str_len = file.path.length();
	data.path = new char[data.str_len+1];
	memcpy(data.path, file.path.c_str(), data.str_len+1);
	data.path[data.str_len] = '\0';
	std::string metapath(file.path);
	metapath = metapath.substr(0, metapath.length() - strlen(strrchr(data.path, '.'))) + std::string(".jsonmeta");
	JSON_Value* v = json_parse_file(metapath.c_str());
	data.uid = json_object_get_u64(json_object(v), "uid");
	json_value_free(v);
}

#include <src/Application.h>
std::vector<WatchedData> TryImport(TempIfStream& file, const char* path) {
	std::vector<WatchedData> ret;
	const char* ext = strrchr(path, '.');

	uint32_t tex_type = 0;
	if (strcmp(ext, ".jsonscene") == 0) {
		ret.push_back(ImportJsonScene(file)); // Assimp Scene never saved to memory
		WatchedData& push_data = ret.back();
		PrepFromFile(push_data, file);
		EV_SEND_UINT64(ECS_LOAD_JSONPREFAB, push_data.uid);
	}
	else if (strcmp(ext, ".mesh") == 0)
	{
		ret.push_back(WatchedData());
		WatchedData& push_data = ret.back();
		PrepFromFile(push_data, file);
		push_data.pd = ImportMesh(file);
	}
	else if (strcmp(ext, ".material") == 0) {
		ret.push_back(WatchedData());
		WatchedData& push_data = ret.back();
		PrepFromFile(push_data, file);
		push_data.pd = ImportMaterial(file);
	}
	else if (ExtensionToDevILType_Import(ext) != 0) {
		ret.push_back(TryImportTexture(file));
		WatchedData& push_data = ret.back();
		PrepFromFile(push_data, file);
	}
		

	return ret;
}


bool InitConverters() {
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool CleanUpConverters() {
	aiDetachAllLogStreams();
	return true;
}

