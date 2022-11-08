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
		ConvertAssimpScene(file); // Assimp Scene never saved to memory
	
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

