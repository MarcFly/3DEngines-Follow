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
PlainData AssimpImporter::ExportAssimpScene(const PlainData& data) {
	PlainData ret;

	const aiScene* aiscene = aiImportFileFromMemory(data.data, data.size, aiProcessPreset_TargetRealtime_Fast, nullptr);
	const aiMesh* aimesh = aiscene->mMeshes[0];
	ret = ExportAssimpMesh(aimesh);

	return ret;
}