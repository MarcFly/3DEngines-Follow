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
#include <src/Application.h>
#include <src/modules/ECS/ComponentsIncludeAll.h>

std::vector<uint32_t> mesh_refs;
std::vector<uint32_t> material_refs;

void TraverseAiNodes(const aiScene* scene, const aiNode* node, const uint64_t parent_id) {
	Entity* get = App->ecs->AddEntity(parent_id);
	uint32_t namesize = (node->mName.length > sizeof(get->name)) ? sizeof(get->name) : node->mName.length;
	memcpy(get->name, node->mName.C_Str(), namesize);
	// Transform and Mesh
	C_Transform* ctrans = App->ecs->AddComponent<C_Transform>(get->id, CT_Transform);
	aiVector3D t, s;
	aiQuaternion r;
	node->mTransformation.Decompose(s,r,t);
	ctrans->rot = Quat(r.x, r.y, r.z, r.w);
	ctrans->scale = float3(s.x, s.y, s.z);
	ctrans->pos = float3(t.x, t.y, t.z);
	ctrans->local_mat.FromQuat(ctrans->rot);
	ctrans->local_mat.Translate(ctrans->pos);
	ctrans->local_mat.Scale(ctrans->scale);
	
	// Pointer is invalidated on a reserve...
	// Can't multithread this...

	for (int i = 0; i < node->mNumMeshes; ++i) {
		C_MeshRenderer* cmesh = App->ecs->AddComponent<C_MeshRenderer>(get->id, CT_MeshRenderer);
		cmesh->material = material_refs[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
		cmesh->mesh = mesh_refs[node->mMeshes[i]];
		cmesh->gl_state = 0; // Set Default
		cmesh->associated_transform = ctrans->id;
	}
	const uint64_t eid = get->id; // Avoid fucking up the pointers...
	for (int i = 0; i < node->mNumChildren; ++i)
		TraverseAiNodes(scene, node->mChildren[i], eid);
}

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
		std::vector<uint32_t> texrefs;
		for (int i = 0; i < mat_texs.size() - 1; ++i)
			texrefs.push_back(App->renderer3D->LoadTexture((Texture*)mat_texs[i].pd.data));
		uint32_t mat_ref = App->renderer3D->LoadMaterial((Material*)mat_texs[mat_texs.size() - 1].pd.data);
		for (int i = 0; i < texrefs.size(); ++i) {
			TexRelation rel;
			rel.tex_uid = texrefs[i];
			App->renderer3D->materials[mat_ref].gpu_textures.push_back(rel);
		}
		material_refs.push_back(mat_ref);
		AppendVec(mats, mat_texs);

	}
	
	std::vector<Entity*> entities;
	std::vector<WatchedData> meshes;
	
	for (int i = 0; i < aiscene->mNumMeshes; ++i) {
		const aiMesh* aimesh = aiscene->mMeshes[i];
		curr.pd = ExportAssimpMesh(aimesh);
		NIMesh* m = (NIMesh*)curr.pd.data;
		m->material.uid = mats_uids[m->material.uid];
		curr.event_type = LOAD_MESH_TO_GPU;
		curr.uid = PCGRand();
		meshes.push_back(curr);

		// Temporarly load it
		uint32_t mesh_ref = App->renderer3D->LoadMesh(m);
		mesh_refs.push_back(mesh_ref);
	}

	// This will not be created directly in thend, will create prefab that you can instantiate
	// As well as prepare everything to be used separately
	TraverseAiNodes(aiscene, aiscene->mRootNode, UINT64_MAX);

	AppendVec(ret, mats);
	AppendVec(ret, meshes);
	aiReleaseImport(aiscene);

	return ret;
}