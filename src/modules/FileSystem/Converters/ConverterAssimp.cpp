#include "../Converters.h"
#include <libs/assimp/cimport.h>
#include <libs/assimp/postprocess.h>
#include <libs/assimp/scene.h>

#include <fstream>
#include <src/modules/Render/RendererTypes.h>
#include <src/Application.h>

// Components for the exporters, not Converters
#include <src/modules/ECS/ComponentsIncludeAll.h>

struct NodeRefs {
	std::vector<uint64_t> mesh_refs;
	std::vector<uint64_t> material_refs;
};

struct AssimpPrefab {
	std::vector<Entity> entities;
	std::vector<Component*> serialized_components;
};

void InitializeTransform(C_Transform* ctrans, const aiMatrix4x4& mat) {
	aiVector3D t, s;
	aiQuaternion r;
	mat.Decompose(s, r, t);
	Quat rot = Quat(r.x, r.y, r.z, r.w);
	float3 scale = float3(s.x, s.y, s.z);
	float3 pos = float3(t.x, t.y, t.z);
	ctrans->local_mat = float4x4::FromTRS(pos, rot, scale);
	ctrans->valid_tree = false;
}

void TraverseAiNodes(const aiScene* scene, const char* parent_path, const aiNode* node, Entity* parent) {
	
	Entity* get = App->ecs->AddEntity((parent!= nullptr) ? parent->id : UINT64_MAX);
	uint32_t namesize = (node->mName.length > sizeof(get->name)) ? sizeof(get->name) : node->mName.length;
	memcpy(get->name, node->mName.C_Str(), namesize);
	// Transform and Mesh
	C_Transform* ctrans = get->AddComponent<C_Transform>();
	InitializeTransform(ctrans, node->mTransformation);

	for (int i = 0; i < node->mNumMeshes; ++i) {
		C_MeshRenderer* cmesh = get->AddComponent<C_MeshRenderer>();
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		ConvertAssimpMesh(aimesh, cmesh->mesh_data);
		cmesh->mesh_use = cmesh->mesh_data.LoadToGPU();

		ConvertAssimpMaterial(scene->mMaterials[aimesh->mMaterialIndex], parent_path, cmesh->mat_data);
		cmesh->mat_use = cmesh->mat_data.LoadToGPU();
	}
	const uint64_t eid = get->id;
	for (int i = 0; i < node->mNumChildren; ++i)
		TraverseAiNodes(scene, parent_path, node->mChildren[i], get);
}

void ConvertAssimpScene(const TempIfStream& file) {
	const PlainData& data = file.GetData();
	const aiScene* aiscene = aiImportFileFromMemory(data.data, data.size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, nullptr);
	
	const char* filename = FileName(file.path.c_str());
	size_t filename_len = strlen(filename);
	std::string parent_path = file.path.substr(0, file.path.length() - filename_len);
	
	TraverseAiNodes(aiscene, parent_path.c_str(), aiscene->mRootNode,nullptr);

	aiReleaseImport(aiscene);
}