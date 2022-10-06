#include "../Importers.h"
#include <libs/assimp/cimport.h>
#include <libs/assimp/postprocess.h>
#include <libs/assimp/scene.h>
#include <libs/assimp/LogStream.hpp>

#pragma comment(lib, "libs/Assimp/libx86/assimp-vc142-mt.lib")

struct aiLogStream stream;

bool AssimpInit() {
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool AssimpCleanUp() {
	aiDetachAllLogStreams();
	return true;
}

#include <fstream>
#include <src/helpers/pcg/pcg_basic.h>
#include <src/modules/Render/RendererTypes.h>

std::vector<WatchedData> ExportAssimpScene(const PlainData& data) {
	std::vector<WatchedData> ret;

	const aiScene* aiscene = aiImportFileFromMemory(data.data, data.size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, nullptr);
	
	WatchedData curr;
	
	std::vector<WatchedData> mats;
	std::vector<uint64_t> mats_uids;
	for (int i = 0; i < aiscene->mNumMaterials; ++i) {
		const aiMaterial* aimat = aiscene->mMaterials[i];
		std::vector<WatchedData> mat_texs = ExportAssimpMaterial(aimat);
		mats_uids.push_back(mat_texs.back().uid);
		AppendVec(mats, mat_texs);
	}
	

	for (int i = 0; i < aiscene->mNumMeshes; ++i) {
		const aiMesh* aimesh = aiscene->mMeshes[i];
		curr.pd = ExportAssimpMesh(aimesh);
		NIMesh* m = (NIMesh*)curr.pd.data;
		m->material.uid = mats_uids[m->material.uid];
		curr.event_type = LOAD_MESH_TO_GPU;
		curr.uid = PCGRand();
		ret.push_back(curr);
	}
	AppendVec(ret, mats);
	aiReleaseImport(aiscene);

	return ret;
}