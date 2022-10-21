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
	ctrans->rot = Quat(r.x, r.y, r.z, r.w);
	ctrans->scale = float3(s.x, s.y, s.z);
	ctrans->pos = float3(t.x, t.y, t.z);
	ctrans->local_mat = float4x4::FromTRS(ctrans->pos, ctrans->rot, ctrans->scale);
	ctrans->valid_tree = false;
}

void TraverseAiNodes(const aiScene* scene, const aiNode* node, const uint64_t parent_id, const NodeRefs& refs, ModuleECS& temp_ecs) {
	Entity* get = temp_ecs.AddEntity(parent_id);
	uint32_t namesize = (node->mName.length > sizeof(get->name)) ? sizeof(get->name) : node->mName.length;
	memcpy(get->name, node->mName.C_Str(), namesize);
	// Transform and Mesh
	C_Transform* ctrans = temp_ecs.AddComponent<C_Transform>(get->id, CT_Transform);
	InitializeTransform(ctrans, node->mTransformation);

	for (int i = 0; i < node->mNumMeshes; ++i) {
		C_MeshRenderer* cmesh = temp_ecs.AddComponent<C_MeshRenderer>(get->id, CT_MeshRenderer);
		cmesh->material = refs.material_refs[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
		cmesh->mesh = refs.mesh_refs[node->mMeshes[i]];
		cmesh->gl_state = 0; // Set Default
		cmesh->associated_transform = ctrans->id;
	}
	const uint64_t eid = get->id;
	for (int i = 0; i < node->mNumChildren; ++i)
		TraverseAiNodes(scene, node->mChildren[i], eid, refs, temp_ecs);
}

std::vector<WatchedData> ConvertAssimpScene(const TempIfStream& file) {
	ModuleECS local_ecs;
	local_ecs.Init();

	std::vector<WatchedData> ret;
	NodeRefs refs;

	const PlainData& data = file.GetData();
	const aiScene* aiscene = aiImportFileFromMemory(data.data, data.size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, nullptr);
	
	const char* filename = FileName(file.path.c_str());
	size_t filename_len = strlen(filename);
	std::string parent_path = file.path.substr(0, file.path.length() - filename_len);
	
	
	std::vector<WatchedData> mats;
	for (int i = 0; i < aiscene->mNumMaterials; ++i) {
		const aiMaterial* aimat = aiscene->mMaterials[i];
		std::vector<WatchedData> mat_texs = ConvertAssimpMaterial(aimat, parent_path.c_str());
		refs.material_refs.push_back(mat_texs.back().uid);
		AppendVec(mats, mat_texs);
	}
	
	std::vector<Entity*> entities;
	std::vector<WatchedData> meshes;
	
	
	for (int i = 0; i < aiscene->mNumMeshes; ++i) {
		WatchedData curr;
		const aiMesh* aimesh = aiscene->mMeshes[i];
		curr.pd = ConvertAssimpMesh(aimesh);
		NIMesh* m = (NIMesh*)curr.pd.data;
		m->material.uid = refs.material_refs[m->material.uid];
		curr.load_event_type = LOAD_MESH_TO_GPU;
		curr.uid = PCGRand();
		refs.mesh_refs.push_back(curr.uid);
		curr.str_len = FitString(curr.path, "Assets/Meshes/%s.mesh", aimesh->mName.C_Str());
		curr.loaded = true;
		meshes.push_back(curr);
	}

	// This will not be created directly in thend, will create prefab that you can instantiate
	// As well as prepare everything to be used separately
	TraverseAiNodes(aiscene, aiscene->mRootNode, UINT64_MAX, refs, local_ecs);


	
	JSON_Value* scene_prefab = local_ecs.SerializePrefab();
	ret.push_back(WatchedData());
	WatchedData& scene_json = ret.back();
	scene_json.loaded = true;
	scene_json.str_len = FitString(scene_json.path, "Assets/Prefabs/%s.json",  std::string(filename).substr(0, filename_len - 4).c_str());
	scene_json.pd.data = json_serialize_to_string_pretty(scene_prefab);
	scene_json.pd.size = strlen(scene_json.pd.data); // bad
	json_value_free(scene_prefab);

	AppendVec(ret, mats);
	AppendVec(ret, meshes);
	aiReleaseImport(aiscene);

	local_ecs.CleanUp();

	return ret;
}