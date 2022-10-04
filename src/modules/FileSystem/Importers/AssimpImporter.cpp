#include "AssimpImporter.h"
#include <libs/assimp/cimport.h>
#include <libs/assimp/postprocess.h>
#include <libs/assimp/scene.h>
#include <libs/assimp/LogStream.hpp>

#pragma comment(lib, "libs/Assimp/libx86/assimp-vc142-mt.lib")

struct aiLogStream stream;

bool AssimpImporter::Init() {
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool AssimpImporter::CleanUp() {
	aiDetachAllLogStreams();
	return true;
}

#include <fstream>
std::vector<WatchedData> AssimpImporter::ExportAssimpScene(const PlainData& data) {
	std::vector<WatchedData> ret;

	const aiScene* aiscene = aiImportFileFromMemory(data.data, data.size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, nullptr);
	
	WatchedData curr;
	for (int i = 0; i < aiscene->mNumMeshes; ++i) {
		const aiMesh* aimesh = aiscene->mMeshes[i];
		curr.pd = ExportAssimpMesh(aimesh);
		curr.event_type = LOAD_MESH_TO_GPU;
		ret.push_back(curr);
	}

	return ret;
}