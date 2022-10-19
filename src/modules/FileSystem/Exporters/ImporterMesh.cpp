#include "../Importers.h"

// 

PlainData ImportAssimpMesh(const aiMesh* aimesh) {
	PlainData ret;
	NIMesh* mesh = new NIMesh();

	mesh->vertices.resize(aimesh->mNumVertices);
	memcpy(mesh->vertices.data(), aimesh->mVertices, aimesh->mNumVertices * sizeof(float3));
	
	if (aimesh->HasNormals()) {
		mesh->normals.resize(aimesh->mNumVertices);
		memcpy(mesh->normals.data(), aimesh->mNormals, aimesh->mNumVertices * sizeof(float3));
	}

	if (aimesh->HasFaces()) {
		mesh->indices.resize(aimesh->mNumFaces * 3);
		for (int i = 0; i < aimesh->mNumFaces; ++i) {
			memcpy(&mesh->indices[i * 3], aimesh->mFaces[i].mIndices, sizeof(uint32_t) * 3);
		}
	}
	
	// Only allow 1 set of uvs for now
	if (aimesh->HasTextureCoords(0)) {
		mesh->uvs.resize(aimesh->mNumVertices);
		if (aimesh->mNumUVComponents[0] == 2) {
			for (int i = 0; i < aimesh->mNumVertices; ++i) {
				memcpy(&mesh->uvs[i], &aimesh->mTextureCoords[0][i], sizeof(float2));
			}
		}
		else {
			memcpy(mesh->uvs.data(), aimesh->mTextureCoords[0], aimesh->mNumVertices * sizeof(float3));
		}
	}

	mesh->material.uid = aimesh->mMaterialIndex;



	// TODO: Base Color / Bounding Box / 

	ret.data = (char*)mesh;
	ret.size = sizeof(NIMesh);

	return ret;
}


