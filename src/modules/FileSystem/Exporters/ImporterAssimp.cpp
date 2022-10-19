#include "../Importers.h"
#include <libs/assimp/cimport.h>
#include <libs/assimp/postprocess.h>
#include <libs/assimp/scene.h>

#include <fstream>
#include <src/modules/Render/RendererTypes.h>
#include <src/Application.h>

// Components for the exporters, not importers
#include <src/modules/ECS/ComponentsIncludeAll.h>
std::vector<uint64_t> mesh_refs;
std::vector<uint64_t> material_refs;

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
	ctrans->local_mat = float4x4::FromTRS(ctrans->pos, ctrans->rot, ctrans->scale);
	ctrans->valid_tree = false;

	for (int i = 0; i < node->mNumMeshes; ++i) {
		C_MeshRenderer* cmesh = App->ecs->AddComponent<C_MeshRenderer>(get->id, CT_MeshRenderer);
		cmesh->material = material_refs[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
		cmesh->mesh = mesh_refs[node->mMeshes[i]];
		cmesh->gl_state = 0; // Set Default
		cmesh->associated_transform = ctrans->id;
	}
	const uint64_t eid = get->id;
	for (int i = 0; i < node->mNumChildren; ++i)
		TraverseAiNodes(scene, node->mChildren[i], eid);
}

std::vector<WatchedData> ImportAssimpScene(const TempIfStream& file) {
	std::vector<WatchedData> ret;

	const PlainData& data = file.GetData();
	const aiScene* aiscene = aiImportFileFromMemory(data.data, data.size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, nullptr);
	
	const char* parent_path_fw = strrchr(file.path.c_str(), '/');
	const char* parent_path_bw = strrchr(file.path.c_str(), '\\');
	const char* parent_path = (parent_path_fw == nullptr) ? parent_path_bw : parent_path_fw;

	WatchedData curr;
	
	std::vector<WatchedData> mats;
	std::vector<uint64_t> mats_uids;
	for (int i = 0; i < aiscene->mNumMaterials; ++i) {
		const aiMaterial* aimat = aiscene->mMaterials[i];
		std::vector<WatchedData> mat_texs = ImportAssimpMaterial(aimat, parent_path);
		mats_uids.push_back(mat_texs.back().uid);
		AppendVec(mats, mat_texs);
	}
	
	std::vector<Entity*> entities;
	std::vector<WatchedData> meshes;
	
	for (int i = 0; i < aiscene->mNumMeshes; ++i) {
		const aiMesh* aimesh = aiscene->mMeshes[i];
		curr.pd = ImportAssimpMesh(aimesh);
		NIMesh* m = (NIMesh*)curr.pd.data;
		m->material.uid = mats_uids[m->material.uid];
		curr.load_event_type = LOAD_MESH_TO_GPU;
		curr.uid = PCGRand();
		meshes.push_back(curr);
	}

	// This will not be created directly in thend, will create prefab that you can instantiate
	// As well as prepare everything to be used separately
	TraverseAiNodes(aiscene, aiscene->mRootNode, UINT64_MAX);

	AppendVec(ret, mats);
	AppendVec(ret, meshes);
	aiReleaseImport(aiscene);

	return ret;
}