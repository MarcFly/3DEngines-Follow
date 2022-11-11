#include "EnginePCH.h"
#include "AssimpConverter.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/LogStream.hpp>

using namespace Engine;


void AssimpConverter::OnAttach() {
	stream = new aiLogStream();
	*stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(stream);
}

void AssimpConverter::OnDetach() {
	aiDetachLogStream(stream);
	delete stream;
}

uint32_t AssimpConverter::ShouldILoad(const char* extension) {
	if (!strcmp(extension, ".fbx") || !strcmp(extension, ".FBX"))
		return 1;

	return 0;
}

#include "ECS/ECS.h"
#include "ECS//DefaultComponents/CS_Transform.h"
#include "ECS/DefaultComponents/CS_MeshRenderer.h"

struct AiRefs {
	std::vector<uint64_t>* mat_ids;
	std::vector<uint64_t>* mesh_ids;
};

void TraverseAiNodes(const aiScene* scene, const aiNode* node, const uint64_t parent_id, const AiRefs& refs, ECS& local_ecs) {
	Entity* e = local_ecs.AddEntity(parent_id);
	uint32_t namesize = (node->mName.length > sizeof(e->name)) ? sizeof(e->name) : node->mName.length;
	snprintf(e->name, sizeof(e->name), "%s", node->mName.C_Str());
	CID tid = local_ecs.AddComponent<S_Transform>(e->id);
	C_Transform& t = local_ecs.GetComponent<C_Transform>(tid);

	aiVector3D aitranslate, aiscale;
	aiQuaternion aiquat;
	node->mTransformation.Decompose(aiscale, aiquat, aitranslate);
	float3 translate(aitranslate.x, aitranslate.y, aitranslate.z), scale(aiscale.x, aiscale.y, aiscale.z);
	Quat rot(aiquat.x, aiquat.y, aiquat.z, aiquat.w);
	t.local_mat = t.local_mat.FromTRS(translate, rot, scale);
	
	uint32_t mesh_idx; CID mid(UINT64_MAX); aiMesh* aimesh;
	for (int i = 0; i < node->mNumMeshes; ++i) {
		mid = local_ecs.AddComponent<S_MeshRenderer>(e->id);
		C_MeshRenderer& m = local_ecs.GetComponent<C_MeshRenderer>(mid);
		mesh_idx = node->mMeshes[i];
		aimesh = scene->mMeshes[mesh_idx];
		m.diskmesh = (*refs.mesh_ids)[mesh_idx];
		m.diskmat = (*refs.mat_ids)[aimesh->mMaterialIndex];
	}

	// TODO: Other default components...

	const uint64_t eid = e->id;
	for (int i = 0; i < node->mNumChildren; ++i)
		TraverseAiNodes(scene, node->mChildren[i], eid, refs, local_ecs);
}

FileVirtual* AssimpConverter::TryLoad(TempIfStream& bytes, const uint32_t internaltype) {
	// Assimp parsing of a scene, will do too many things
	// Generate Meshes + meta
	// Convert textures to .dds and meta...
	// Create materials + meta
	// Create prefab scene and have it already loaded in memory
	// After each data type created from an aiscene, an event of load for the meta should be sent, so that it becomes available

	Engine::ECS local_ecs; // TODO: external systems register too...
	const PlainData& pd = bytes.GetData();
	const aiScene* aiscene = aiImportFileFromMemory((const char*)pd.data, pd.size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, nullptr);

	const char* filename = FileNameExt(bytes.path.c_str());
	const std::string parent_path = ParentPath(bytes.path);

	std::vector<uint64_t> mat_ids;
	std::vector<uint64_t> mesh_ids;

	for (int i = 0; i < aiscene->mNumMaterials; ++i) {
		const aiMaterial* aimat = aiscene->mMaterials[i];
		uint64_t mat_id = ConvertMaterial(aimat, parent_path.c_str());
		mat_ids.push_back(mat_id);
	}

	const char* scenename = aiscene->GetShortFilename(bytes.path.c_str());

	for (int i = 0; i < aiscene->mNumMeshes; ++i) {
		const aiMesh* aimesh = aiscene->mMeshes[i];
		uint64_t mesh_id = ConvertMesh(aimesh, scenename, i);
		mesh_ids.push_back(mesh_id);
	}

	AiRefs refs;
	refs.mat_ids = &mat_ids;
	refs.mesh_ids = &mesh_ids;
	TraverseAiNodes(aiscene, aiscene->mRootNode, UINT64_MAX, refs, local_ecs);
	
	JSONVWrap ret;
	ret.value = local_ecs.SerializeScene();
	PlainData json_ser = ret.Serialize();

	static char filepath[256];
	snprintf(filepath, sizeof(filepath), "%s/Assets/Prefab/%s.jsonscene", FS::execpath, json_object_get_string(json_object(ret.value), "secenename"));
	FS::WriteToDisk(filepath, json_ser); 
	FS::TryLoadFile(filepath);
	
	return nullptr;
}

#include "Renderer/Renderer.h"
#include "../Loaders/MaterialLoader.h"
uint64_t AssimpConverter::ConvertMaterial(const aiMaterial* aimat, const char* parent_path) {
	
	int i, j, num_texs;
	aiString path;
	uint64_t texdiskid;
	std::vector<uint64_t> tex_diskids;
	std::vector<Engine::TextureTypes> tex_types;
	for (i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
		num_texs = aimat->GetTextureCount((aiTextureType)i);
		for (j = 0; j < num_texs; ++j) {
			aimat->GetTexture((aiTextureType)i, j, &path);
			texdiskid = Engine::FS::TryLoadFile(path.C_Str(), parent_path);
			texdiskid = FS::FindIDByName(path.C_Str());
			if (texdiskid == UINT64_MAX) {
				Engine_WARN("Could not load Texture: {}{}", parent_path, path.C_Str());
			}
			else {
				tex_diskids.push_back(texdiskid);
				tex_types.push_back((Engine::TextureTypes)i);
			}
		}	
	}

	
	
	// TODO: PBR values...
	// TODO: Stack variable data array...
	Engine::WriteStream out;
	num_texs = tex_diskids.size();
	out.Add(&num_texs);
	out.AddArr(tex_diskids.data(), num_texs);
	out.AddArr(tex_types.data(), num_texs);
	
	PlainData write;
	write.Acquire(out.stream, out.size);
	out.stream = nullptr;
	static char filepath[256];
	snprintf(filepath, sizeof(filepath), "%s/Assets/Materials/%s.material", FS::execpath, aimat->GetName().C_Str());
	FS::WriteToDisk(filepath, write);

	return FS::TryLoadFile(filepath);
}

#include "../Loaders/MeshLoader.h"

uint64_t AssimpConverter::ConvertMesh(const aiMesh* aimesh, const char* filename, int meshnum) {
	FileMesh test;
	int num_vtx = aimesh->mNumVertices;
	bool hasfaces, hasnormals, hastanbitan;

	test.vtx.resize(num_vtx);
	memcpy(test.vtx.data(), aimesh->mVertices, sizeof(float3)*num_vtx);
	
	hasfaces = aimesh->HasFaces();
	if (hasfaces) {
		uint32_t numIndices = aimesh->mFaces->mNumIndices;
		test.idx.resize(aimesh->mNumFaces * numIndices);
		for (int i = 0; i < aimesh->mNumFaces; ++i)
			memcpy(&test.idx.data()[i * numIndices], aimesh->mFaces[i].mIndices, sizeof(uint32_t) * numIndices);
	}

	hasnormals = aimesh->HasNormals();
	if (hasnormals) {
		test.vtx_normals.resize(num_vtx);
		memcpy(test.vtx_normals.data(), aimesh->mNormals, sizeof(float3) * num_vtx);
	}

	int num_uvchannels = aimesh->GetNumUVChannels();
	for (int i = 0; i < num_uvchannels; ++i) {
		if (aimesh->HasTextureCoords(i)) {
			test.uv_channels.push_back(FileMesh::uvs());
			FileMesh::uvs& uv = test.uv_channels.back();
			uv.num_uv_coords = aimesh->mNumUVComponents[i];
			uv.data = new float[uv.num_uv_coords * num_vtx];
			float* pointer = uv.data;
			for (int j = 0; j < num_vtx; ++j, pointer += uv.num_uv_coords)
				memcpy(pointer, &aimesh->mTextureCoords[i][j], sizeof(float) * uv.num_uv_coords);

			uv.name = std::string(aimesh->GetTextureCoordsName(i)->C_Str());
		}
	}

	hastanbitan = aimesh->HasTangentsAndBitangents();
	if (hastanbitan) {
		test.tangents.resize(num_vtx);
		test.bitangents.resize(num_vtx);
		memcpy(test.tangents.data(), aimesh->mTangents, sizeof(float3) * num_vtx);
		memcpy(test.bitangents.data(), aimesh->mBitangents, sizeof(float3) * num_vtx);
	}

	PlainData write = test.Serialize();
	static char filepath[256];
	snprintf(filepath, sizeof(filepath), "%s/Assets/Meshes/%s%d.mesh", FS::execpath, filename, meshnum);
	FS::WriteToDisk(filepath, write);

	return FS::TryLoadFile(filepath);
}