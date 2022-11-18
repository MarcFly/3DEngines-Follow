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
	snprintf(e->name, sizeof(e->name), "%s", node->mName.C_Str());

	// Every game object has at least 1 transform
	CID cid = local_ecs.AddComponent<S_Transform>(e->id);
	C_Transform& t = local_ecs.GetComponent<C_Transform>(cid);

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
		m.mesh.id = (*refs.mesh_ids)[mesh_idx];
		m.mat.id = (*refs.mat_ids)[aimesh->mMaterialIndex];
	}

	// TODO: Other default components...

	const uint64_t eid = e->id;
	for (int i = 0; i < node->mNumChildren; ++i)
		TraverseAiNodes(scene, node->mChildren[i], eid, refs, local_ecs);
}

std::shared_ptr<FileVirtual> AssimpConverter::TryLoad(TempIfStream& bytes, const uint32_t internaltype) {
	// Assimp parsing of a scene, will do too many things
	// Generate Meshes + meta
	// Convert textures to .dds and meta...
	// Create materials + meta
	// Create prefab scene and have it already loaded in memory
	// After each data type created from an aiscene, an event of load for the meta should be sent, so that it becomes available

	Engine::ECS local_ecs; // TODO: external systems register too...
	const aiScene* aiscene = aiImportFileFromMemory((const char*)bytes.bytes.get(), bytes.size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, nullptr);

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
	snprintf(local_ecs.scenename, sizeof(local_ecs.scenename), "%s", filename);
	ret.value = local_ecs.SerializeScene();
	PlainData json_ser = ret.Serialize();

	
	static char filepath[256];
	snprintf(filepath, sizeof(filepath), "%s/Assets/Prefabs/%s.jsonscene", FS::execpath, json_object_get_string(json_object(ret.value), "scenename"));
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
	
	PlainData write(out.stream, out.size);
	out.stream = nullptr;
	static char filepath[256];
	snprintf(filepath, sizeof(filepath), "%s/Assets/Materials/%s.material", FS::execpath, aimat->GetName().C_Str());
	FS::WriteToDisk(filepath, write);

	return FS::TryLoadFile(filepath);
}

#include "../Loaders/MeshLoader.h"

uint64_t AssimpConverter::ConvertMesh(const aiMesh* aimesh, const char* filename, int meshnum) {
	FileMesh test;
	test.mesh = std::make_shared<RAMMesh>();
	if (aimesh->HasFaces()) {
		size_t num_indicesperface = aimesh->mFaces->mNumIndices;
		size_t num_indices = test.mesh->num_idx = aimesh->mNumFaces * num_indicesperface; // Assimp only has 4byte indices
		test.mesh->idx = std::shared_ptr<byte[]>(new byte[num_indices * sizeof(uint32_t)]);
		for (uint32_t i = 0; i < aimesh->mNumFaces; ++i) {
			memcpy(	&test.mesh->idx.get()[i * sizeof(uint32_t) * num_indicesperface],
					aimesh->mFaces[i].mIndices,
					sizeof(uint32_t) * num_indicesperface);
		}

		test.mesh->idx_size = sizeof(uint32_t);
		test.mesh->idx_var = GL_UNSIGNED_INT;
	}

	int num_vtx = aimesh->mNumVertices;
	test.mesh->vtx_desc.vtx_num = num_vtx;
	test.mesh->vtx_desc.attributes.reserve(15);
	test.mesh->vtx_desc.attributes.push_back("position");
	VertAttrib* pos_attrib = &test.mesh->vtx_desc.attributes.back();

	bool hasnormals = aimesh->HasNormals();
	VertAttrib* normals_attrib = nullptr;
	if (hasnormals) {
		test.mesh->vtx_desc.attributes.push_back("normal");
		normals_attrib = &test.mesh->vtx_desc.attributes.back();
	}

	bool hastanbitan = aimesh->HasTangentsAndBitangents();
	VertAttrib* tan_attrib = nullptr;
	VertAttrib* bitan_attrib = nullptr;
	if (hastanbitan) {
		test.mesh->vtx_desc.attributes.push_back("tangent");
		tan_attrib = &test.mesh->vtx_desc.attributes.back();
		test.mesh->vtx_desc.attributes.push_back("bitangent");
		bitan_attrib = &test.mesh->vtx_desc.attributes.back();
	}

	size_t num_uv_channels = aimesh->GetNumUVChannels();
	static char uvchname[8];
	std::vector<VertAttrib*> uvattribs;
	for (int i = 0; i < num_uv_channels; ++i) {
		snprintf(uvchname, sizeof(uvchname), "uv%d", i);
		test.mesh->vtx_desc.attributes.push_back(uvchname);
		test.mesh->vtx_desc.attributes.back().num_components = aimesh->mNumUVComponents[i];
		uvattribs.push_back(&test.mesh->vtx_desc.attributes.back());
	}

	size_t vtx_bufsize = test.mesh->vtx_desc.VertSize() * num_vtx;

	test.mesh->vtx = std::shared_ptr<byte[]>(new byte[vtx_bufsize]);

	// HERE THINK IF YOU WANT INTERLEAVED OR BLOCK
	// BLOCK FOR NOW
	byte* pointer = test.mesh->vtx.get();
	size_t offset = pos_attrib->num_components * pos_attrib->var_size * num_vtx;
	memcpy(pointer, aimesh->mVertices, offset);
	pointer += offset;

	if (hasnormals) {
		offset = normals_attrib->num_components * normals_attrib->var_size * num_vtx;
		memcpy(pointer, aimesh->mNormals, offset);
		pointer += offset;
	}

	if (hastanbitan) {
		offset = tan_attrib->num_components * tan_attrib->var_size * num_vtx;
		memcpy(pointer, aimesh->mTangents, offset);
		pointer += offset;

		offset = bitan_attrib->num_components * bitan_attrib->var_size * num_vtx;
		memcpy(pointer, aimesh->mBitangents, offset);
		pointer += offset;
	}

	for (int i = 0; i < num_uv_channels; ++i) {
		offset = uvattribs[i]->num_components * uvattribs[i]->var_size;
		for (int j = 0; j < uvattribs[i]->num_components; ++j) {
			memcpy(pointer, &aimesh->mTextureCoords[i][j], offset);
			pointer += offset;
		}
	}	

	PlainData write = test.Serialize();
	static char filepath[256];
	snprintf(filepath, sizeof(filepath), "%s/Assets/Meshes/%s%d.mesh", FS::execpath, filename, meshnum);
	FS::WriteToDisk(filepath, write);

	return FS::TryLoadFile(filepath);
}