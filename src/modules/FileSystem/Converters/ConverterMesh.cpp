#include "../Converters.h"

PlainData ImportMesh(const TempIfStream& file) {
	PlainData ret;

	NIMesh* mesh = new NIMesh();
	SetPlainData(ret, mesh, sizeof(NIMesh));
	PlainData read_pd = file.GetData();
	ReadStream read;
	read.SetData((byte*)read_pd.data, read_pd.size);

	uint32_t num_vertices, num_normals, num_uvs, num_indices;
	bool half;
	read.Get(&num_vertices); mesh->vertices.resize(num_vertices);
	read.Get(&num_normals); mesh->normals.resize(num_normals);
	read.Get(&num_uvs); mesh->uvs.resize(num_uvs);
	read.Get(&half);
	read.Get(&num_indices); 

	read.GetArr(mesh->vertices.data(), num_vertices);
	read.GetArr(mesh->normals.data(), num_normals);
	read.GetArr(mesh->uvs.data(), num_uvs);
	
	if (half) {
		mesh->h_indices.resize(num_indices);
		read.GetArr(mesh->h_indices.data(), num_indices);
	}
	else {
		mesh->indices.resize(num_indices);
		read.GetArr(mesh->indices.data(), num_indices);
	}

	return ret;
}

PlainData SerializeNIMesh(const NIMesh& nimesh) {
	PlainData ret;

	WriteStream out;
	
	// Prepare sizes
	uint32_t num_vertices = nimesh.vertices.size();
	out.Add(&num_vertices);

	uint32_t num_normals = nimesh.normals.size();
	out.Add(&num_normals);

	uint32_t num_uvs = nimesh.uvs.size();
	out.Add(&num_uvs);

	bool half = nimesh.indices.size() == 0;
	uint32_t num_indices = (half) ? nimesh.h_indices.size() : nimesh.indices.size();
	uint32_t idx_size = (half) ? sizeof(uint16_t) : sizeof(uint32_t);
	out.Add(&half);
	out.Add(&num_indices);

	out.AddArr(nimesh.vertices.data(), num_vertices);
	out.AddArr(nimesh.normals.data(), num_normals);
	out.AddArr(nimesh.uvs.data(), num_uvs);
	if (half) out.AddArr(nimesh.h_indices.data(), num_indices);
	else out.AddArr(nimesh.indices.data(), num_indices);

	datapair out_data = out.GetOwnership();
	ret.data = (char*)out_data.first;
	ret.size = out_data.second;

	return ret;
}

PlainData ConvertAssimpMesh(const aiMesh* aimesh) {
	PlainData ret;
	NIMesh mesh;

	mesh.vertices.resize(aimesh->mNumVertices);
	memcpy(mesh.vertices.data(), aimesh->mVertices, aimesh->mNumVertices * sizeof(float3));
	
	if (aimesh->HasNormals()) {
		mesh.normals.resize(aimesh->mNumVertices);
		memcpy(mesh.normals.data(), aimesh->mNormals, aimesh->mNumVertices * sizeof(float3));
	}

	if (aimesh->HasFaces()) {
		mesh.indices.resize(aimesh->mNumFaces * 3);
		for (int i = 0; i < aimesh->mNumFaces; ++i) {
			memcpy(&mesh.indices[i * 3], aimesh->mFaces[i].mIndices, sizeof(uint32_t) * 3);
		}
	}
	
	// Only allow 1 set of uvs for now
	if (aimesh->HasTextureCoords(0)) {
		mesh.uvs.resize(aimesh->mNumVertices);
		if (aimesh->mNumUVComponents[0] == 2) {
			for (int i = 0; i < aimesh->mNumVertices; ++i) {
				memcpy(&mesh.uvs[i], &aimesh->mTextureCoords[0][i], sizeof(float2));
			}
		}
		else {
			memcpy(mesh.uvs.data(), aimesh->mTextureCoords[0], aimesh->mNumVertices * sizeof(float3));
		}
	}

	//mesh.material.uid = aimesh->mMaterialIndex;



	// TODO: Base Color / Bounding Box / 

	return SerializeNIMesh(mesh);
}


