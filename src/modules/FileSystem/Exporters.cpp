#include "Exporters.h"

// Assimp
#include <libs/assimp/cimport.h>
#include <libs/assimp/postprocess.h>
#include <libs/assimp/scene.h>
#include <libs/assimp/LogStream.hpp>
#pragma comment(lib, "libs/Assimp/libx86/assimp-vc142-mt.lib")

struct aiLogStream stream;

bool InitExporters() {
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool CleanUpExporters() {
	aiDetachAllLogStreams();

}

std::vector<WatchedData> TryExport(TempIfStream& file, const char* path) {
	std::vector<WatchedData> ret;
	const char* ext = strrchr(path, '.');

	uint32_t tex_type = 0;
	if (strcmp(ext, ".fbx") == 0 || strcmp(ext, ".FBX") == 0)
		ret = ExportAssimpScene(file.GetData()); // Assimp Scene never saved to memory
	else if ((tex_type = ExtensionToDevILType(ext)) != 0)
	{
		ret.push_back(WatchedData());
		WatchedData& curr = ret.back();
		curr.pd = ImportDevILTexture(file.GetData(), tex_type);
		curr.event_type = LOAD_TEX_TO_GPU;
		curr.uid = PCGRand();
	}

	return ret;
}